#include "Model.h"
#include "util.h"
#include "float4.h"

ModelVBO::ModelVBO() : modeldata_vboid(0), indices_vboid(0) { };
ModelVBO::~ModelVBO() { };

Model::Model(void)
	: vertex_data(0), normal_data(0), uv_data(0), textures(0), triSet(0), 
	INDICES_PER_TRIANGLE(6), vertexCount(0), normalCount(0), uvCount(0), texCount(0), triCount(0)
{ };

Model::~Model(void)
{
	try
	{
		SAFE_DELETE_ARRAY(vertex_data);
		SAFE_DELETE_ARRAY(normal_data);
		SAFE_DELETE_ARRAY(uv_data);
		SAFE_DELETE_ARRAY(triSet);
	} catch(...) { };

	triCount = 0;
};

bool Model::RecalculatePerVertexNormals()
{
	if(!GetTriSet() || !GetTriCount()) return false; // no faces

	// Delete old normal data (if any)
	if(normal_data)
	{
		delete [] normal_data;
		normal_data = 0;
		normalCount = 0;
	}
	
	if(INDICES_PER_TRIANGLE == 6) // Verts and Normals
	{
		// Number of normals (to put into triSet)
		normalCount = GetTriCount() * Model::FLOATS_PER_VERTEX_NORMAL; // 1 normal per vertex

		// Size of the array of normals (NOTE: may contain duplicates)
		u32 normal_array_sz = normalCount * 3; // 3 verts per face
		
		normal_data = new f32[normal_array_sz];
		u32 ipos=0; // insertion pos into normal_data
		float4 s1,s2;

		// Loop over triSet so we can find the vertex indices
		u32 triSetSz = GetTriCount() * GetIndicesPerTriangle();
		for( u32 i = 0; i < triSetSz; i += GetIndicesPerTriangle() )
		{
			float4 v1 = float4::FromXYZ(&vertex_data[triSet[i]]);
			float4 v2 = float4::FromXYZ(&vertex_data[triSet[i+2]]);
			float4 v3 = float4::FromXYZ(&vertex_data[triSet[i+4]]);

			// First normal
			s1 = v2-v1; s2 = v3-v1;
			s1.cross(s2, &normal_data[ipos]);

			// Second normal
			s1 = v3-v2; s2 = v1-v2;
			s1.cross(s2, &normal_data[ipos+3]);

			// Third normal
			s1 = v1-v3; s2 = v2-v3;
			s1.cross(s2, &normal_data[ipos+6]);

			// Set indices (1,3,5)
			triSet[i+1] = ipos;   // v0 normal index
			triSet[i+3] = ipos+3; // v1 normal index
			triSet[i+5] = ipos+6; // v2 normal index

			ipos += 9;
		}
		return true;
	}
	else if(INDICES_PER_TRIANGLE == 9) // Verts, UVs and Normals
	{
		// TODO: WRITE THIS

		normalCount = GetTriCount() * Model::FLOATS_PER_VERTEX_NORMAL; // 1 normal per vertex
		normal_data = new f32[normalCount];
	}

	return false;
};

ModelVBO Model::BuildVBO() const
{
	//ModelVBO mvbo;
	//return mvbo;
	return ModelVBO();
};

u32 Model::GetIndicesPerTriangle() const
{
	return INDICES_PER_TRIANGLE;
};

bool Model::SetIndicesPerTriangle(c8 indicesPerTri)
{
	if(indicesPerTri < 6 || indicesPerTri > 9) return false;

	INDICES_PER_TRIANGLE = indicesPerTri;
	return true;
};