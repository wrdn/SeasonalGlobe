#include "OBJFile.h"
#include "strutils.h"

#include <map>
#include <sstream>
#include <sys/stat.h>
using namespace std;

OBJFile::OBJFile()
{
};

OBJFile::~OBJFile()
{
	try
	{
		for(std::vector<Model*>::iterator i = models.begin(); i != models.end(); ++i)
		{
			if(*i)
			{
				delete *i;
				*i = NULL;
			}
		}
	}
	catch(...) { }
};

void OBJFile::Draw()
{
	for(std::vector<Model*>::iterator i=models.begin(); i < models.end(); ++i)
	{
		(*i)->Draw();
	}
};

void OBJFile::BuildModelVBOs()
{
	for(std::vector<Model*>::iterator i=models.begin();i<models.end();++i)
	{
		(*i)->BuildVBO();
	}
};

bool OBJFile::ParseOBJFile(const c8* filename)
{
	if(file_exists(filename))
	{
		// Basic heuristic (based on reading example OBJ files). Each line is ~32 characters, assuming ASCII, we can divide the file size
		// in bytes by 32 to get the approximate number of lines in the file. Using this as the original size for the vector, we have to
		// reallocate and move memory FAR less often
		struct stat st;
		stat(filename, &st);
		int basicSz = st.st_size / 32;

		std::vector<c8*> objData = read_src_to_vec(filename, false, basicSz);
		bool ret = ParseOBJFile(objData);

		for(std::vector<c8*>::iterator i = objData.begin(); i != objData.end(); ++i)
		{
			delete [] *i;
			*i = NULL;
		}

		return ret;
	}
	return false;
};

// Revised object loader
bool OBJFile::ParseOBJFile(const std::vector<c8*> &objFile)
{
	if(!objFile.size()) { return false; }

	Model *activeModel = new Model();

	/*****************************************************/
	/**************** READ VERTICES **********************/
	/*****************************************************/
	string tmp; u32 pos=0, commentCount=0, insertionPos=0;
	while( (stringstream(objFile[pos]) >> tmp) && tmp != "v" ) ++pos; // find vertices
	u32 cp=pos;
	while( (stringstream(objFile[pos]) >> tmp) && (tmp == "v" || tmp[0] == '#') ) // count vertices
	{
		if(tmp[0] == '#') { ++commentCount; }
		++pos;
	}
	activeModel->SetVertexCount(pos - cp - commentCount);
	if(!activeModel->GetVertexCount())
	{
		SAFE_DELETE(activeModel);
		return false; // no verts
	}

	f32* vertex_data = new f32[activeModel->GetVertexCount() * Model::FLOATS_PER_VERTEX_POS];
	while(cp < pos) // Read vertices
	{
		stringstream str(objFile[cp]);
		str >> tmp;
		if(tmp[0] == '#') { ++cp; continue; } // ignore comments
		str >> vertex_data[insertionPos] >> vertex_data[insertionPos+1] >> vertex_data[insertionPos+2];
		insertionPos += 3, ++cp;
	}
	activeModel->SetVertexArray(vertex_data);

	/*****************************************************/
	/**************** READ NORMALS ***********************/
	/*****************************************************/
	int oldPos = pos; bool normals=true; commentCount=0, insertionPos = 0;
	while((stringstream(objFile[pos]) >> tmp) && tmp != "vn")
	{
		// found faces, no normals, reset to old pos in vector so we can look for UVs
		if(tmp == "f") { normals=false; pos = oldPos; break; }
		++pos;
	}
	cp=pos;

	f32 *normal_data=0;
	if(normals) // found normals (existance optional)
	{
		while( (stringstream(objFile[pos]) >> tmp) && (tmp == "vn" || tmp[0] == '#') ) // count normals
		{
			if(tmp[0] == '#') { ++commentCount; }
			++pos;
		}
		activeModel->SetNormalCount(pos - cp - commentCount);

		normal_data = new f32[activeModel->GetNormalCount() * Model::FLOATS_PER_VERTEX_NORMAL];
		while(cp < pos) // read normals
		{
			stringstream str(objFile[cp]);
			str >> tmp;
			if(tmp[0] == '#') { ++cp; continue; }
			str >> normal_data[insertionPos] >> normal_data[insertionPos+1] >> normal_data[insertionPos+2];
			insertionPos += 3, ++cp;
		}
		activeModel->SetNormalArray(normal_data);
	}

	/*****************************************************/
	/****************** READ UVs *************************/
	/*****************************************************/
	bool uvs=true; commentCount = 0, insertionPos = 0; pos = oldPos;
	while((stringstream(objFile[pos]) >> tmp) && tmp != "vt") // find uvs
	{
		if(tmp == "f") { uvs=false; pos=oldPos; break; }
		++pos;
	}
	cp=pos;

	f32 *uv_data=0;
	if(uvs) // found UVs (existance optional)
	{
		while( (stringstream(objFile[pos]) >> tmp) && (tmp == "vt" || tmp[0] == '#') ) // count uvs
		{
			if(tmp[0] == '#') { ++commentCount; }
			++pos;
		}
		activeModel->SetUVCount(pos - cp - commentCount);

		uv_data = new f32[activeModel->GetUVCount() * Model::FLOATS_PER_VERTEX_UV];
		while(cp < pos) // read uvs
		{
			stringstream str(objFile[cp]);
			str >> tmp;
			if(tmp[0] == '#') { ++cp; continue; }
			str >> uv_data[insertionPos] >> uv_data[insertionPos+1];
			insertionPos += 2, ++cp;
		}
		activeModel->SetUVArray(uv_data);
	}

	/*****************************************************/
	/***************** READ FACES ************************/
	/*****************************************************/
	while((stringstream(objFile[pos]) >> tmp) && tmp != "f") ++pos; // find faces
	cp=pos, commentCount=0;
	while( pos < objFile.size() && (stringstream(objFile[pos]) >> tmp) && (tmp == "f" || tmp[0] == '#') ) // count faces
	{
		if(tmp[0] == '#') { ++commentCount; }
		++pos;
	}
	activeModel->SetTriCount(pos - cp - commentCount);
	if(!activeModel->GetTriCount()) // faces required
	{
		delete activeModel;
		return false;
	}

	// Face reading logic dependant on what exists in the file and
	// what has been read. v+vn most likely, then v+vn+vt, then v+vt, then v
	
	// Index array of constant size (regardless of file format)
	std::map<unsigned long,u32> hashMap;
	const u32 array_sz = activeModel->GetTriCount() * Model::GetIndicesPerTriangle();
	u32* indexArray = new u32[array_sz];
	//memset(indexArray,0, sizeof(u32) * array_sz);
	insertionPos = 0;

	// Size of VERTEX array also constant, created then may be made smaller (better to allocate
	// memory once, than use a vector and allocate many VERTEXes
	VERTEX *vertexArray = new VERTEX[array_sz];
	memset(vertexArray,0,sizeof(VERTEX) * array_sz);
	u32 vertexArrayInsertionPos = 0;

#pragma region Vertices and Normals
	if(activeModel->GetNormalArray() && !activeModel->GetUVArray()) // v+vn
	{
		while(cp < pos)
		{
			stringstream str(objFile[cp]); char tmpc;
			str >> tmp;
			if(tmp[0] == '#') { ++cp; continue; }
			++cp;

			for(char ti=0;ti<3;++ti)
			{
				string iset;
				str >> iset;
				unsigned long sh = hash_djb2((unsigned char*)iset.c_str());
				std::map<unsigned long,u32>::iterator foundIndex = hashMap.find(sh);

				// Found index, so insert the existing VERTEX indice
				if(foundIndex != hashMap.end())
				{
					indexArray[insertionPos++] = foundIndex->second;
				}
				else // Couldn't find index
				{
					// Parse the set to get the indices of the vertex/normal
					stringstream indexParser(iset); u32 indices[2];
					indexParser >> indices[0] >> tmpc >> tmpc >> indices[1];
					clamp(indices[0], 1, activeModel->GetVertexCount());
					clamp(indices[1], 1, activeModel->GetNormalCount());
					
					indices[0] -= 1; indices[1] -= 1;

					// Copy data
					memcpy(vertexArray[vertexArrayInsertionPos].pos, &vertex_data[indices[0]*3], sizeof(f32)*3);
					memcpy(vertexArray[vertexArrayInsertionPos].norm, &normal_data[indices[1]*3], sizeof(f32)*3);

					hashMap[sh] = vertexArrayInsertionPos;
					indexArray[insertionPos++] = vertexArrayInsertionPos;
					++vertexArrayInsertionPos;
				}
			} // end of index and vertex parsing loop
		}
		activeModel->SetIndicesArray(indexArray);
		activeModel->SetVertexData(vertexArray);
		activeModel->realVertexDataSz = vertexArrayInsertionPos;
	}
#pragma endregion

#pragma region Vertices, Normals and UVs
	else if(activeModel->GetNormalArray() && activeModel->GetUVArray()) // v+vn+vt, v/vt/vn
	{
		// This code is largely identical to the code for Vertices and Normals, except the
		// texture coordinates are also read and set
		while(cp < pos)
		{
			stringstream str(objFile[cp]); char tmpc;
			str >> tmp;
			if(tmp[0] == '#') { ++cp; continue; }
			++cp;

			for(char ti=0;ti<3;++ti)
			{
				string iset;
				str >> iset;
				unsigned long sh = hash_djb2((unsigned char*)iset.c_str());
				std::map<unsigned long,u32>::iterator foundIndex = hashMap.find(sh);

				// Found index, so insert the existing VERTEX indice
				if(foundIndex != hashMap.end())
				{
					indexArray[insertionPos++] = foundIndex->second;
				}
				else // Couldn't find index
				{
					// Parse the set to get the indices of the vertex/normal/texture coordinate
					// v/vt/vn
					stringstream indexParser(iset); u32 indices[3];
					indexParser >> indices[0] >> tmpc >> indices[1] >> tmpc >> indices[2];

					clamp(indices[0], 1, activeModel->GetVertexCount());
					clamp(indices[1], 1, activeModel->GetUVCount());
					clamp(indices[2], 1, activeModel->GetNormalCount());
					indices[0] -= 1; indices[1] -= 1; indices[2] -= 1;

					// Copy data
					memcpy(vertexArray[vertexArrayInsertionPos].pos, &vertex_data[indices[0]*3], sizeof(f32)*3);
					memcpy(vertexArray[vertexArrayInsertionPos].uvs, &uv_data[indices[1]*2], sizeof(f32)*2);
					memcpy(vertexArray[vertexArrayInsertionPos].norm, &normal_data[indices[2]*3], sizeof(f32)*3);

					hashMap[sh] = vertexArrayInsertionPos;
					indexArray[insertionPos++] = vertexArrayInsertionPos;
					++vertexArrayInsertionPos;
				}
			} // end of index and vertex parsing loop
		}
		activeModel->SetIndicesArray(indexArray);
		activeModel->SetVertexData(vertexArray);
		activeModel->realVertexDataSz = vertexArrayInsertionPos;
	}
#pragma endregion

	else if(activeModel->GetUVArray() && !activeModel->GetNormalArray()) // v+vt
	{
		throw; // to implement
	}

	else // v
	{
		throw; // to implement
	}

	models.push_back(activeModel); // only add the model if everything succeeds
	return true;
};

/*
bool OBJFile::ParseOBJFile2(const std::vector<c8*> &objFile)
{
	if(!objFile.size()) return false;

	Model *activeModel = new Model();
	models.push_back(activeModel);

	#pragma region Read Vertices
	u32 pos=0;

	// Locate and count the vertices
	string tmp;
	while( (stringstream(objFile[pos]) >> tmp) && tmp != "v" ) ++pos; // find vertices
	u32 cp=pos, commentCount=0;
	while( (stringstream(objFile[pos]) >> tmp) && (tmp == "v" || tmp[0] == '#') ) // count vertices
	{
		if(tmp[0] == '#') { ++commentCount; }
		++pos;
	}
	activeModel->SetVertexCount(pos - cp - commentCount);
	u32 insertionPos = 0;

	// Read vertices
	f32* vertex_data = new f32[activeModel->GetVertexCount() * Model::FLOATS_PER_VERTEX_POS];
	while(cp < pos)
	{
		stringstream str(objFile[cp]);
		str >> tmp; // ignore any comments
		if(tmp[0] != '#')
		{
			str >> vertex_data[insertionPos] >> vertex_data[insertionPos+1] >> vertex_data[insertionPos+2];
			insertionPos += 3;
		}
		++cp;
	}
	activeModel->SetVertexArray(vertex_data);
	#pragma endregion

	#pragma region Read Normals

	// Locate and count normals (must come BEFORE face definitions if they exist)
	int oldPos = pos; bool normals=true;
	while((stringstream(objFile[pos]) >> tmp) && tmp != "vn")
	{
		// found faces, no normals, reset to old pos in vector so we can look for UVs
		if(tmp == "f") { normals=false; pos = oldPos; break; }
		++pos;
	}
	cp=pos, commentCount=0;

	if(normals) // We only get here is there were normals in the file
	{
		while( (stringstream(objFile[pos]) >> tmp) && (tmp == "vn" || tmp[0] == '#') ) // count normals
		{
			if(tmp[0] == '#') { ++commentCount; }
			++pos;
		}
		activeModel->SetNormalCount(pos - cp - commentCount);
		insertionPos = 0;

		// Read normals
		f32* normal_data = new f32[activeModel->GetVertexCount() * Model::FLOATS_PER_VERTEX_NORMAL];
		while(cp < pos)
		{
			stringstream str(objFile[cp]);
			str >> tmp;
			if(tmp[0] != '#') // ignore any comments
			{
				str >> normal_data[insertionPos] >> normal_data[insertionPos+1] >> normal_data[insertionPos+2];
				insertionPos += 3;
			}
			++cp;
		}
		activeModel->SetNormalArray(normal_data);
	} // end if(normals)
	#pragma endregion

	#pragma region Read UVs

	// Locate the UVs (must come BEFORE face definitions, if there exist)
	oldPos = pos; bool uvs=true;
	while((stringstream(objFile[pos]) >> tmp) && tmp != "vt")
	{
		if(tmp == "f") { uvs=false; pos=oldPos; break; } // found faces, no uvs
		++pos;
	}
	cp=pos; commentCount=0;

	if(uvs)
	{
		while( (stringstream(objFile[pos]) >> tmp) && (tmp == "vt" || tmp[0] == '#') ) // count uvs
		{
			if(tmp[0] == '#') { ++commentCount; }
			++pos;
		}
		activeModel->SetUVCount(pos - cp - commentCount);
		insertionPos = 0;

		// Read UVs
		f32* uv_data = new f32[activeModel->GetUVCount() * Model::FLOATS_PER_VERTEX_UV];
		while(cp < pos)
		{
			stringstream str(objFile[cp]);
			str >> tmp;
			if(tmp[0] != '#')
			{
				str >> uv_data[insertionPos] >> uv_data[insertionPos+1] >> uv_data[insertionPos+2];
				insertionPos += 3;
			}
			++cp;
		}
		activeModel->SetUVArray(uv_data);
	}
	#pragma endregion

	// POSSIBLE SITUATIONS (V=Vertex, N=Normal, T=UV):
	// EXISTS
	// V 1 1 1 1
	// N 0 1 0 1
	// T 0 0 1 1

	The vertices should always exist, but the normals/UVs may not. It is possible to have v, v//vt, v//vn, or v/vt/vn
	If the vertices do not exist (for whatever reason), return false.
	

	// Locate and count faces
	while((stringstream(objFile[pos]) >> tmp) && tmp != "f") ++pos; // find faces
	cp=pos, commentCount=0;
	while( pos < objFile.size() && (stringstream(objFile[pos]) >> tmp) && (tmp == "f" || tmp[0] == '#') ) // count faces
	{
		if(tmp[0] == '#') { ++commentCount; }
		++pos;
	}
	activeModel->SetTriCount(pos - cp - commentCount);
	insertionPos = 0;

	// Read faces
	if(activeModel->GetVertexArray()) // Vertices exist
	{
		#pragma region Vertices and Normals

		// Normals, no UVs, format: v//vn v//vn v//vn
		if(activeModel->GetNormalArray() && !activeModel->GetUVArray())
		{
			activeModel->SetIndicesPerTriangle(6); // vertex pos (x,y,z) and normal (x,y,z)
			const u32 array_sz = activeModel->GetTriCount() * activeModel->GetIndicesPerTriangle();
			u32* triSet = new u32[array_sz];
			memset(triSet, 0, sizeof(u32) * array_sz); // zero out the array

			while(cp < pos)
			{
				stringstream str(objFile[cp]); char tmpc;
				str >> tmp;
				if(tmp[0] == '#') { ++cp; continue; }
				u32 indices[6];
				str >> 
					indices[0] >> tmpc >> tmpc >> indices[1] >>
					indices[2] >> tmpc >> tmpc >> indices[3] >>
					indices[4] >> tmpc >> tmpc >> indices[5];

				clamp(indices[0], 1, activeModel->GetVertexCount());
				clamp(indices[1], 1, activeModel->GetNormalCount());
				clamp(indices[2], 1, activeModel->GetVertexCount());
				clamp(indices[3], 1, activeModel->GetNormalCount());
				clamp(indices[4], 1, activeModel->GetVertexCount());
				clamp(indices[5], 1, activeModel->GetNormalCount());

				// First Vertex:
				triSet[insertionPos++] = (indices[0] - 1);// * 3; //v0
				triSet[insertionPos++] = (indices[1] - 1);// * 3; //n0
				// Second Vertex:
				triSet[insertionPos++] = (indices[2] - 1);// * 3; //v1
				triSet[insertionPos++] = (indices[3] - 1);// * 3; //n1
				// Third Vertex:
				triSet[insertionPos++] = (indices[4] - 1);// * 3; //v2
				triSet[insertionPos++] = (indices[5] - 1);// * 3; //n2

				++cp;
			}
			activeModel->SetTriSet(triSet);
		}
		#pragma endregion

		#pragma region Vertices, Normals and UVs
		// Normals and UVs, format: v/vt/vn v/vt/vn v/vt/vn
		else if(activeModel->GetNormalArray() && activeModel->GetUVArray())
		{
			activeModel->SetIndicesPerTriangle(9);
			const u32 array_sz = activeModel->GetTriCount() * activeModel->GetIndicesPerTriangle();
			u32* triSet = new u32[array_sz];
			memset(triSet, 0, sizeof(u32) * array_sz);

			while(cp < pos)
			{
				stringstream str(objFile[cp]); char tmpc;
				str >> tmp;
				if(tmp[0] == '#') { ++cp; continue; }
				u32 indices[9];
				str >>
					indices[0] >> tmpc >> indices[1] >> tmpc >> indices[2] >>
					indices[3] >> tmpc >> indices[4] >> tmpc >> indices[5] >>
					indices[6] >> tmpc >> indices[7] >> tmpc >> indices[8];

				clamp(indices[0], 1, activeModel->GetVertexCount());
				clamp(indices[1], 1, activeModel->GetUVCount());
				clamp(indices[2], 1, activeModel->GetNormalCount());
				clamp(indices[3], 1, activeModel->GetVertexCount());
				clamp(indices[4], 1, activeModel->GetUVCount());
				clamp(indices[5], 1, activeModel->GetNormalCount());
				clamp(indices[6], 1, activeModel->GetVertexCount());
				clamp(indices[7], 1, activeModel->GetUVCount());
				clamp(indices[8], 1, activeModel->GetNormalCount());

				// First Vertex:
				triSet[insertionPos++] = (indices[0] - 1) * 3; // v0
				triSet[insertionPos++] = (indices[1] - 1) * 2; // vt0
				triSet[insertionPos++] = (indices[2] - 1) * 3; // vn0
				// Second Vertex:
				triSet[insertionPos++] = (indices[3] - 1) * 3; // v0
				triSet[insertionPos++] = (indices[4] - 1) * 2; // vt0
				triSet[insertionPos++] = (indices[5] - 1) * 3; // vn0
				// Third Vertex:
				triSet[insertionPos++] = (indices[6] - 1) * 3; // v0
				triSet[insertionPos++] = (indices[7] - 1) * 2; // vt0
				triSet[insertionPos++] = (indices[8] - 1) * 3; // vn0

				++cp;
			}
			activeModel->SetTriSet(triSet);
		}
		#pragma endregion

		#pragma region Vertices and UVs (TODO: RECALC NORMALS)
		// UVs and no Normals, format: v/vt v/vt v/vt
		else if(activeModel->GetUVArray() && !activeModel->GetNormalArray())
		{
			activeModel->SetIndicesPerTriangle(9);
			const u32 array_sz = activeModel->GetTriCount() * activeModel->GetIndicesPerTriangle();
			u32* triSet = new u32[array_sz];
			memset(triSet, 0, sizeof(u32) * array_sz);

			while(cp < pos)
			{
				stringstream str(objFile[cp]); char tmpc;
				str >> tmp;
				if(tmp[0] == '#') { ++cp; continue; }

				u32 indices[6];
				str >> 
					indices[0] >> tmpc >> indices[1] >>
					indices[2] >> tmpc >> indices[3] >>
					indices[4] >> tmpc >> indices[5];

				clamp(indices[0], 1, activeModel->GetVertexCount());
				clamp(indices[1], 1, activeModel->GetUVCount());
				clamp(indices[2], 1, activeModel->GetVertexCount());
				clamp(indices[3], 1, activeModel->GetUVCount());
				clamp(indices[4], 1, activeModel->GetVertexCount());
				clamp(indices[5], 1, activeModel->GetUVCount());

				// Positions 2, 5 and 8 left for normals
				// First Vertex:
				triSet[insertionPos] = (indices[0] - 1) * 3; // v0
				triSet[insertionPos+1] = (indices[1] - 1) * 2; // vt0
				// Second Vertex:
				triSet[insertionPos+3] = (indices[3] - 1) * 3; // v0
				triSet[insertionPos+4] = (indices[4] - 1) * 2; // vt0
				// Third Vertex:
				triSet[insertionPos+6] = (indices[6] - 1) * 3; // v0
				triSet[insertionPos+7] = (indices[7] - 1) * 2; // vt0
			}

			activeModel->SetTriSet(triSet);
		}
		#pragma endregion

		#pragma region Vertices only (TODO: RECALC NORMALS)
		// No UVs and no Normals (vertices only), set vertices, and recalc normals (6 indices)
		// It is unlikely we'll ever be in this code, but we should cover the possibility
		// Format: v v v
		else
		{
			activeModel->SetIndicesPerTriangle(6); // vertex pos (x,y,z), recalculate normals (x,y,z)
			const u32 array_sz = activeModel->GetTriCount() * activeModel->GetIndicesPerTriangle();
			u32* triSet = new u32[array_sz];
			memset(triSet, 0, sizeof(u32) * array_sz);

			while(cp < pos)
			{
				stringstream str(objFile[cp]);
				str >> tmp;
				if(tmp[0] == '#') { ++cp; continue; }
				u32 indices[3]; // verts only, normals recalculated at end
				str >> indices[0] >> indices[1] >> indices[2]; // v0 v1 v2

				clamp(indices[0], 1, activeModel->GetVertexCount());
				clamp(indices[1], 1, activeModel->GetVertexCount());
				clamp(indices[2], 1, activeModel->GetVertexCount());

				// Positions 1, 3 and 5 will hold the normal indices
				triSet[insertionPos]   = (indices[0]-1) * 3;
				triSet[insertionPos+2] = (indices[1]-1) * 3;
				triSet[insertionPos+4] = (indices[2]-1) * 3;

				insertionPos += 6;
				++cp;
			}
			activeModel->SetTriSet(triSet);
			activeModel->RecalculatePerVertexNormals();
		}
		#pragma endregion

		return true;
	}

	return false; // no vertices
};
*/