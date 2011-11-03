#include "OBJFile.h"
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

OBJFile::OBJFile()
{
};

OBJFile::~OBJFile()
{
	for(u32 i=0;i<models.size();++i)
		SAFE_DELETE(models[i]);
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

		// Cleanup the vector we made before leaving
		for(u32 i=0;i<objData.size();++i)
			SAFE_DELETE(objData[i]);

		return ret;
	}
	return false;
};

// ParseOBJFile(...) starts by removing whitespace from lines, and removing comments.
// This will not effect the data, as we are simply moving or removing COPIES of pointers
// The vector is passed by value, so the stuff in it will be copied - hence we can mess with the vector
// and remove stuff, without effecting the original vector (passed as a function parameter)
bool OBJFile::ParseOBJFile(std::vector<c8*> objFile)
{
	if(!objFile.size()) { return false; } // No file contents
	// Remove leading whitespace
	for(u32 i=0;i<objFile.size();++i)
	{
		c8* d = objFile[i];
		while(*d == ' ') d++;
		objFile[i] = d;

		if(*d == '#') { objFile.erase(objFile.begin()+i); --i; } // remove comments
	}
	if(!objFile.size()) { return false; } // The only file contents were comments

	Model *activeModel = new Model();
	models.push_back(activeModel);

	/* READ VERTICES */
	i32 pos=0;
	while(*objFile[pos] != 'v' && (*objFile[pos]+1) != ' ' && (u32)pos<objFile.size()) { ++pos; }
	i32 cp=pos;
	while(*objFile[pos] == 'v' && *(objFile[pos]+1) == ' ') ++activeModel->vertexCount, ++pos;

	activeModel->vertex_data = new f32[activeModel->vertexCount * 3];

	i32 insertionPos = 0;
	while(cp < pos)
	{
		stringstream str(objFile[cp]); char cr[4];
		str >> cr >> activeModel->vertex_data[insertionPos] >> activeModel->vertex_data[insertionPos+1] >> activeModel->vertex_data[insertionPos+2];
		++cp;
		insertionPos+=3;
	}

	/* READ NORMALS */
	bool foundNormals = false, foundFaces=false;
	while((*objFile[pos] != 'v' && *(objFile[pos]+1) != 'n' && *(objFile[pos]+2) != ' ') && (u32)pos < objFile.size())
	{
		if(*objFile[pos] == 'f' && *(objFile[pos]+1) == ' ') { foundFaces = true; break; }
		++pos;
	}
	foundNormals = !foundFaces;

	cp = pos;
	while((*objFile[pos] == 'v' && *(objFile[pos]+1) == 'n' && *(objFile[pos]+2) == ' '))
		++activeModel->normalCount, ++pos;

	activeModel->normal_data = new f32[activeModel->normalCount * 3];

	insertionPos = 0;
	while(cp < pos)
	{
		stringstream str(objFile[cp]); char cr[4];
		str >> cr >> activeModel->normal_data[insertionPos] >> activeModel->normal_data[insertionPos+1] >> activeModel->normal_data[insertionPos+2];
		++cp;
		insertionPos+=3;
	}

	/* READ FACES (assumes contiguous, not interspersed with vt's etc ) */
	while(*objFile[pos] != 'f' && *objFile[pos]+1 != ' ' && (u32)pos<objFile.size()) ++pos;

	cp = pos;
	while(pos<objFile.size() && (*objFile[pos] == 'f' && *(objFile[pos]+1) == ' '))
	{
		++activeModel->triCount;
		++pos;
	}
	insertionPos = 0;

	// For the purpose of testing, just read vertices at the moment, but assume that
	// vertices and normals exist e.g. 1//2 3//4 5//6 (v//vn format)
	// For now, ignore UV coordinates, so ALL triangles have 6 data members (3 vertices and 3 normals)
	if(activeModel->vertexCount && activeModel->normalCount &&
		!activeModel->uvCount) // vertices and uvs available
	{
		activeModel->SetIndicesPerTriangle(6);
		activeModel->triSet = new u32[activeModel->triCount * activeModel->GetIndicesPerTriangle()];

		while(cp < pos) // read in data
		{
			int indices[6]; // Indices 0,2 and 4 are vertex indices. Indices 1,3 and 5 are normal indices

			stringstream str(objFile[cp]); char junk;
			//str >> junk >>
			//	indices[0] >> junk >> junk >> indices[1] >>
			//	indices[2] >> junk >> junk >> indices[3] >>
			//	indices[4] >> junk >> junk >> indices[5];
			//sscanf(inp, "%d//%d %d//%d %d//%d", &vi1, &ni1, &vi2, &ni2, &vi3, &ni3);
			char *inp = objFile[cp]+2;
			sscanf(inp,"%d//%d %d//%d %d//%d", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4],
				&indices[5]);

			activeModel->triSet[insertionPos]   = (indices[0]-1)*3; //v1
			activeModel->triSet[insertionPos+1] = (indices[1]-1)*3; //n1
			activeModel->triSet[insertionPos+2] = (indices[2]-1)*3; //v2
			activeModel->triSet[insertionPos+3] = (indices[3]-1)*3; //n2
			activeModel->triSet[insertionPos+4] = (indices[4]-1)*3; //v3
			activeModel->triSet[insertionPos+5] = (indices[5]-1)*3; //n3

			++cp, insertionPos += 6;
		}
	}
	else
	{
		return false;
	}

	return true;
};

/* OLD FACE READING CODE:

// We now need to read the faces. Though we should not make assumptions when parsing, the format of the faces should be predictable.
// (A) If there were no normals and no uvs, the format would be "50 40 70"
// (B) If there were vertices and normals (or vertices and uvs), the format would be "50//34 40//76 70//59"
// (C) If there were vertices, normals and uvs, the format would be "50/39/34 40/72/76 70/45/59"

// The triangle index data format is: VertexNormalUV - VertexNormalUV - VertexNormalUV. We can live without UVs, but if Normals dont exist, we still
// add the space for them, then calculate them at runtime

if(activeModel->vertexCount && (!activeModel->normalCount && !activeModel->uvCount)) // read verts
{
activeModel->SetIndicesPerTriangle(6);
activeModel->triSet = new u32[activeModel->triCount * 6];

while(cp < pos)
{
char *inp = objFile[cp]+2;

int vi1=0, vi2=0, vi3=0;

// Leave out every odd element - these will contain the normal references (interleaved)
sscanf(inp, "%d %d %d", vi1, vi2, vi3);

activeModel->triSet[insertionPos] = vi1*3;
activeModel->triSet[insertionPos+2] = vi2*3;
activeModel->triSet[insertionPos+4] = vi3*3;

// Zero out spaces for the normals (which should be calculated)
activeModel->triSet[insertionPos+1] = 0;
activeModel->triSet[insertionPos+3] = 0;
activeModel->triSet[insertionPos+5] = 0;

++cp;
insertionPos += 6;
}
}
else if(activeModel->vertexCount && activeModel->normalCount && !activeModel->uvCount) // read verts and normals
{
activeModel->SetIndicesPerTriangle(6);
activeModel->triSet = new u32[activeModel->triCount * 6];

while(cp < pos)
{
int vi1=0, vi2=0, vi3=0, ni1=0, ni2=0, ni3=0;

char *inp = objFile[cp]+2;
sscanf(inp, "%d//%d %d//%d %d//%d", &vi1, &ni1, &vi2, &ni2, &vi3, &ni3);

activeModel->triSet[insertionPos]   = vi1*3; //v1
activeModel->triSet[insertionPos+1] = ni1*3; //n1
activeModel->triSet[insertionPos+2] = vi2*3; //v2
activeModel->triSet[insertionPos+3] = ni2*3; //n2
activeModel->triSet[insertionPos+4] = vi3*3; //v3
activeModel->triSet[insertionPos+5] = ni3*3; //n3

++cp;
insertionPos += 6;
}

}
else if(activeModel->vertexCount && activeModel->uvCount && !activeModel->normalCount) // read verts and uvs
{
activeModel->SetIndicesPerTriangle(9); // 9 indices per triangle (3 for vertices, 3 for normals, and 3 for tex coords)
activeModel->triSet = new u32[activeModel->triCount * 9];

while(cp < pos)
{
char *inp = objFile[cp]+2;
int v1i,uv1, v2i, uv2, v3i,uv3;

sscanf(inp,"%d//%d %d//%d %d//%d",&v1i, &uv1, &v2i, &uv2, &v3i, &uv3);

activeModel->triSet[insertionPos]   = v1i*3;
activeModel->triSet[insertionPos+2] = uv1*2; // texture coords are 2D, so every indice refers to N*2 position in array
activeModel->triSet[insertionPos+3] = v2i*3;
activeModel->triSet[insertionPos+5] = uv2*2;
activeModel->triSet[insertionPos+6] = v3i*3;
activeModel->triSet[insertionPos+7] = uv3*2;

// Zero out space for the normals
activeModel->triSet[insertionPos+1] = 0;
activeModel->triSet[insertionPos+4] = 0;
activeModel->triSet[insertionPos+7] = 0;

++cp;
insertionPos += 9;
}

printf("Attempting to read vertices and UV coordinates\n");
}
else if(activeModel->vertexCount && activeModel->normalCount && activeModel->uvCount) // read verts, normals and uvs
{
activeModel->SetIndicesPerTriangle(9); // 9 indices per triangle (3 for vertices, 3 for normals, and 3 for tex coords)
activeModel->triSet = new u32[activeModel->triCount * 9];

while(cp < pos)
{
// Format: v/vt/vn
char *inp = objFile[cp]+2;

int v1i, v2i, v3i, vn1, vn2, vn3, uv1, uv2, uv3;

sscanf(inp,"%d/%d/%d %d/%d/%d %d/%d/%d", &v1i,&uv1,&vn1,&v2i,&uv2,&vn2,&v3i,&uv3,&vn3);

activeModel->triSet[insertionPos]   = v1i*3;
activeModel->triSet[insertionPos+1] = vn1*3;
activeModel->triSet[insertionPos+2] = uv1*2;

activeModel->triSet[insertionPos+3] = v2i*3;
activeModel->triSet[insertionPos+4] = vn2*3;
activeModel->triSet[insertionPos+5] = uv2*2;

activeModel->triSet[insertionPos+6] = v3i*3;
activeModel->triSet[insertionPos+7] = vn3*3;
activeModel->triSet[insertionPos+8] = uv3*2;

++cp;
insertionPos += 9;
}
}
*/