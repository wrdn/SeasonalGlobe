#include "Model.h"
#include "util.h"

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

void Model::RecalculateNormals() const
{
};

ModelVBO Model::BuildVBO() const
{
	ModelVBO mvbo;
	return mvbo;
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