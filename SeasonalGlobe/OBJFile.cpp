#include "OBJFile.h"
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

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

bool OBJFile::ParseOBJFile(const std::vector<c8*> &objFile)
{
	if(!objFile.size()) return false;

	Model *activeModel = new Model();
	models.push_back(activeModel);

#pragma region Read Vertices
	/*****************************************************/
	/**************** READ VERTICES **********************/
	/*****************************************************/
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
	/*****************************************************/
	/**************** READ NORMALS ***********************/
	/*****************************************************/

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
	/*****************************************************/
	/****************** READ UVs *************************/
	/*****************************************************/

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


	/*****************************************************/
	/***************** READ FACES ************************/
	/*****************************************************/

	/* POSSIBLE SITUATIONS (V=Vertex, N=Normal, T=UV):
	EXISTS
	V 1 1 1 1
	N 0 1 0 1
	T 0 0 1 1

	The vertices should always exist, but the normals/UVs may not. It is possible to have v, v//vt, v//vn, or v/vt/vn
	If the vertices do not exist (for whatever reason), return false.
	*/

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
				triSet[insertionPos++] = (indices[0] - 1) * 3; //v0
				triSet[insertionPos++] = (indices[1] - 1) * 3; //n0
				// Second Vertex:
				triSet[insertionPos++] = (indices[2] - 1) * 3; //v1
				triSet[insertionPos++] = (indices[3] - 1) * 3; //n1
				// Third Vertex:
				triSet[insertionPos++] = (indices[4] - 1) * 3; //v2
				triSet[insertionPos++] = (indices[5] - 1) * 3; //n2

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

			/* TODO: RECALCULATE NORMALS HERE */

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
			}

			/* TODO: RECALCULATE NORMALS HERE */

			activeModel->SetTriSet(triSet);
		}
#pragma endregion

		return true;
	}

	return false; // no vertices
};