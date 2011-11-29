#include "OBJFile.h"
#include "strutils.h"

#include <map>
#include <sstream>
#include <sys/stat.h>
using namespace std;

OBJFile::OBJFile() : models(0), modelCount(0)
{
};

OBJFile::~OBJFile()
{
	/*try
	{
		for(std::vector<Model*>::iterator i = models->begin(); i != models->end(); ++i)
		{
			if(*i)
			{
				delete *i;
				*i = NULL;
			}
		}
	}
	catch(...) { }*/
	//SAFE_DELETE_ARRAY(models);

	if(models)
	{
		for(u32 i=0;i<modelCount;++i)
			delete models[i];
		
		delete [] models;
	}
};

void OBJFile::Draw()
{
	/*for(std::vector<Model*>::iterator i=models->begin(); i < models->end(); ++i)
	{
		(*i)->Draw();
	}*/
	for(u32 i=0;i<modelCount;++i)
		models[i]->Draw();
};

void OBJFile::BuildModelVBOs()
{
	/*for(std::vector<Model*>::iterator i=models->begin();i<models->end();++i)
	{
		(*i)->BuildVBO();
	}*/
	for(u32 i=0;i<modelCount;++i)
		models[i]->BuildVBO();
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
		u32 basicSz = st.st_size / 32;

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
	
	const u32 VERTEX_COUNT = pos-cp-commentCount;
	if(!VERTEX_COUNT)
	{
		SAFE_DELETE(activeModel);
		return false; // no verts
	}

	f32* vertex_data = new f32[VERTEX_COUNT * 3];
	while(cp < pos) // Read vertices
	{
		stringstream str(objFile[cp]);
		str >> tmp;
		if(tmp[0] == '#') { ++cp; continue; } // ignore comments
		str >> vertex_data[insertionPos] >> vertex_data[insertionPos+1] >> vertex_data[insertionPos+2];
		insertionPos += 3, ++cp;
	}

	/*****************************************************/
	/**************** READ NORMALS ***********************/
	/*****************************************************/
	u32 oldPos = pos; bool normals=true; commentCount=0, insertionPos = 0;
	while((stringstream(objFile[pos]) >> tmp) && tmp != "vn")
	{
		// found faces, no normals, reset to old pos in vector so we can look for UVs
		if(tmp == "f") { normals=false; pos = oldPos; break; }
		++pos;
	}
	cp=pos;

	f32 *normal_data=0; u32 NORMAL_COUNT=0;
	if(normals) // found normals (existance optional)
	{
		while( (stringstream(objFile[pos]) >> tmp) && (tmp == "vn" || tmp[0] == '#') ) // count normals
		{
			if(tmp[0] == '#') { ++commentCount; }
			++pos;
		}
		NORMAL_COUNT = pos - cp - commentCount; // normals optional

		if(NORMAL_COUNT)
		{
			normal_data = new f32[NORMAL_COUNT * 3];
			while(cp < pos) // read normals
			{
				stringstream str(objFile[cp]);
				str >> tmp;
				if(tmp[0] == '#') { ++cp; continue; }
				str >> normal_data[insertionPos] >> normal_data[insertionPos+1] >> normal_data[insertionPos+2];
				insertionPos += 3, ++cp;
			}
		}
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

	f32 *uv_data=0; u32 UV_COUNT=0;
	if(uvs) // found UVs (existance optional)
	{
		while( (stringstream(objFile[pos]) >> tmp) && (tmp == "vt" || tmp[0] == '#') ) // count uvs
		{
			if(tmp[0] == '#') { ++commentCount; }
			++pos;
		}
		UV_COUNT = pos - cp - commentCount;

		if(UV_COUNT) // UVs optional
		{
			uv_data = new f32[UV_COUNT * 2];
			while(cp < pos) // read uvs
			{
				stringstream str(objFile[cp]);
				str >> tmp;
				if(tmp[0] == '#') { ++cp; continue; }
				str >> uv_data[insertionPos] >> uv_data[insertionPos+1];
				insertionPos += 2, ++cp;
			}
		}
	}

	/*****************************************************/
	/***************** READ FACES ************************/
	/*****************************************************/
	while((stringstream(objFile[pos]) >> tmp) && tmp != "f") ++pos; // find faces
	cp=pos, commentCount=0;
	while( pos < objFile.size() && (stringstream(objFile[pos]) >> tmp) && (tmp == "f" || tmp[0] == '#' || tmp[0] == 's') ) // count faces
	{
		if(tmp[0] == '#' || tmp[0] == 's') { ++commentCount; } // ignore comments and smoothing groups
		++pos;
	}
	const u32 TRIANGLE_COUNT = pos - cp - commentCount;
	if(!TRIANGLE_COUNT) // faces required
	{
		SAFE_DELETE_ARRAY(vertex_data);
		SAFE_DELETE_ARRAY(normal_data);
		SAFE_DELETE_ARRAY(uv_data);
		SAFE_DELETE(activeModel);
		return false;
	}

	// Face reading logic dependant on what exists in the file and
	// what has been read. v+vn most likely, then v+vn+vt, then v+vt, then v
	
	// Index array of constant size (regardless of file format)
	std::map<unsigned long,u32> hashMap;
	const u32 array_sz = TRIANGLE_COUNT * 3;
	u32* indexArray = new u32[array_sz];
	insertionPos = 0;

	VERTEX *vertexArray = new VERTEX[array_sz];
	memset(vertexArray,0,sizeof(VERTEX) * array_sz);
	u32 vertexArrayInsertionPos = 0;

#pragma region Vertices and Normals
	if(normal_data && !uv_data) // v+vn
	{
		while(cp < pos)
		{
			stringstream str(objFile[cp]); c8 tmpc;
			str >> tmp;
			if(tmp[0] == '#' || tmp[0] == 's') { ++cp; continue; }
			++cp;

			for(c8 ti=0;ti<3;++ti)
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
					clamp(indices[0], 1, VERTEX_COUNT);
					clamp(indices[1], 1, NORMAL_COUNT);
					
					indices[0] -= 1; indices[1] -= 1;

					// Copy data
					vertexArray[vertexArrayInsertionPos].pos.set(&vertex_data[indices[0]*3]);
					vertexArray[vertexArrayInsertionPos].normal.set(&vertex_data[indices[1]*3]);

					hashMap[sh] = vertexArrayInsertionPos;
					indexArray[insertionPos++] = vertexArrayInsertionPos;
					++vertexArrayInsertionPos;
				}
			} // end of index and vertex parsing loop
		}
		activeModel->SetVertexArray(vertexArray, vertexArrayInsertionPos);
		activeModel->SetIndicesArray(indexArray, array_sz);
	}
#pragma endregion

#pragma region Vertices, Normals and UVs
	else if(normal_data && uv_data) // v+vn+vt, v/vt/vn
	{
		// This code is largely identical to the code for Vertices and Normals, except the
		// texture coordinates are also read and set
		while(cp < pos)
		{
			stringstream str(objFile[cp]); c8 tmpc;
			str >> tmp;
			if(tmp[0] == '#' || tmp[0] == 's') { ++cp; continue; }
			++cp;

			for(c8 ti=0;ti<3;++ti)
			{
				string iset;
				str >> iset;
				unsigned long sh = hash_djb2((uc8*)iset.c_str());
				std::map<unsigned long,u32>::iterator foundIndex = hashMap.find(sh);

				// Found index, so insert the existing VERTEX indice
				if(foundIndex != hashMap.end())
				{
					indexArray[insertionPos++] = foundIndex->second;
				}
				else // Couldn't find index
				{
					// Parse the set to get the indices of the vertex/normal/texture coordinate, v/vt/vn
					stringstream indexParser(iset); u32 indices[3];
					indexParser >> indices[0] >> tmpc >> indices[1] >> tmpc >> indices[2];

					clamp(indices[0], 1, VERTEX_COUNT);
					clamp(indices[1], 1, UV_COUNT);
					clamp(indices[2], 1, NORMAL_COUNT);
					indices[0] -= 1; indices[1] -= 1; indices[2] -= 1;

					// Copy data
					vertexArray[vertexArrayInsertionPos].pos.set(&vertex_data[indices[0]*3]);
					vertexArray[vertexArrayInsertionPos].uvs.set(&uv_data[indices[1]*2]);
					vertexArray[vertexArrayInsertionPos].normal.set(&normal_data[indices[2]*3]);

					hashMap[sh] = vertexArrayInsertionPos;
					indexArray[insertionPos++] = vertexArrayInsertionPos;
					++vertexArrayInsertionPos;
				}
			} // end of index and vertex parsing loop
		}
		activeModel->SetIndicesArray(indexArray, array_sz);
		activeModel->SetVertexArray(vertexArray, vertexArrayInsertionPos);
	}
#pragma endregion

	else if(uv_data && !normal_data) // v+vt
	{
		throw; // to implement
	}
	else // v
	{
		throw; // to implement
	}

	// Add the model
	if(!models)
	{
		modelCount = 1;
		models = new Model*[modelCount];
		models[0] = activeModel;
	}
	else
	{
		Model **newModelSet =  new Model*[modelCount+1];
		memcpy(newModelSet, models, modelCount);
		SAFE_DELETE_ARRAY(models);
		newModelSet[modelCount] = activeModel;
		modelCount += 1;
	}

	// Cleanup
	SAFE_DELETE_ARRAY(vertex_data);
	SAFE_DELETE_ARRAY(normal_data);
	SAFE_DELETE_ARRAY(uv_data);

	return true;
};