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


	/*****************************************************/
	/**************** READ NORMALS ***********************/
	/*****************************************************/

	// Locate and count normals
	while((stringstream(objFile[pos]) >> tmp) && tmp != "vn") ++pos; // find normals
	cp=pos, commentCount=0;
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

	/*****************************************************/
	/****************** READ UVs *************************/
	/*****************************************************/

	// . . .

	/*****************************************************/
	/***************** READ FACES ************************/
	/*****************************************************/

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
	if( (activeModel->GetVertexArray() && activeModel->GetNormalArray()) && (!activeModel->GetUVArray()) ) // verts and normals, format: v//vn
	{
		activeModel->SetIndicesPerTriangle(6);
		u32* triSet = new u32[activeModel->GetTriCount() * 6];
		
		int iter=0;
		while(cp < pos)
		{
			++iter;

			u32 indices[6];
			stringstream str(objFile[cp++]);
			char tmpc; // holds double slash between v and vn

			str >> tmp;
			if(tmp[0] == '#') { continue; } // ignore comments

			str >>
				indices[0] >> tmpc >> tmpc >> indices[1] >> // First vertex/normal
				indices[2] >> tmpc >> tmpc >> indices[3] >> // Second vertex/normal
				indices[4] >> tmpc >> tmpc >> indices[5];   // Third vertex/normal

			// Clamp indices to 1 < INDEX <= VertexCount | NormalCount
			clamp(indices[0], 1, activeModel->GetVertexCount());
			clamp(indices[1], 1, activeModel->GetNormalCount());
			clamp(indices[2], 1, activeModel->GetVertexCount());
			clamp(indices[3], 1, activeModel->GetNormalCount());
			clamp(indices[4], 1, activeModel->GetVertexCount());
			clamp(indices[5], 1, activeModel->GetNormalCount());

			triSet[insertionPos++] = (indices[0] - 1) * 3; //v0
			triSet[insertionPos++] = (indices[1] - 1) * 3; //n0
			triSet[insertionPos++] = (indices[2] - 1) * 3; //v1
			triSet[insertionPos++] = (indices[3] - 1) * 3; //n1
			triSet[insertionPos++] = (indices[4] - 1) * 3; //v2
			triSet[insertionPos++] = (indices[5] - 1) * 3; //n2
		}
		activeModel->SetTriSet(triSet);
	}
	else { return false; } // False if anything other than Vertices and Normals (dont currently support UVs)

	return true;
};