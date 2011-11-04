#include "Model.h"
#include "util.h"

ModelVBO::ModelVBO() : modeldata_vboid(0), indices_vboid(0) { };
ModelVBO::~ModelVBO() { };

Model::Model(void)
	: vertexCount(0), vertex_data(0), normalCount(0), normal_data(0), 
	uvCount(0), uv_data(0), texCount(0), textures(0), triCount(0), triSet(0),
	INDICES_PER_TRIANGLE(6)
{
}

Model::~Model(void)
{
	SAFE_DELETE_ARRAY(vertex_data);
	SAFE_DELETE_ARRAY(normal_data);
	SAFE_DELETE_ARRAY(uv_data);
	SAFE_DELETE_ARRAY(triSet);
	triCount = 0;
};

void Model::RecalculateNormals()
{
};

ModelVBO Model::BuildVBO()
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