#include "Model.h"
#include "util.h"

ModelVBO::ModelVBO() : modeldata_vboid(0), indices_vboid(0) { };
ModelVBO::~ModelVBO() { };

Model::Model(void)
	: vertexCount(0), vertex_data(0), normalCount(0), normal_data(0), 
	uvCount(0), uv_data(0), texCount(0), textures(0), triCount(0), triSet(0),
	INDICES_PER_TRIANGLE(6)
	/*: vertexCount(0), vx(0), vy(0), vz(0),
	normalCount(0), nx(0), ny(0), nz(0),
	uvCount(0), tu(0), tv(0),
	texCount(0), textures(0),
	triCount(0), triSet(0),
	INDICES_PER_TRIANGLE(6)*/
{
}

Model::~Model(void)
{
	/*SAFE_DELETE_ARRAY(vx);
	SAFE_DELETE_ARRAY(vy);
	SAFE_DELETE_ARRAY(vz);
	SAFE_DELETE_ARRAY(nx);
	SAFE_DELETE_ARRAY(ny);
	SAFE_DELETE_ARRAY(nz);
	SAFE_DELETE_ARRAY(tu);
	SAFE_DELETE_ARRAY(tv);*/
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

	/*
	// VERTEX/NORMAL/UV DATA
	// Generate and bind buffer
	glGenBuffers(1, &mvbo.modeldata_vboid);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, mvbo.modeldata_vboid);

	glBufferDataARB(GL_ARRAY_BUFFER_ARB,
		(sizeof(f32)*3)*vertexCount,vertex_data,GL_STATIC_DRAW_ARB);

	glGenBuffers(1, &mvbo.indices_vboid);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, mvbo.indices_vboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(u32)*(triCount*INDICES_PER_TRIANGLE),triSet, GL_STATIC_DRAW);
	*/
	
	return mvbo;
};

i32 Model::GetIndicesPerTriangle()
{
	return INDICES_PER_TRIANGLE;
};

bool Model::SetIndicesPerTriangle(c8 indicesPerTri)
{
	if(indicesPerTri < 6 || indicesPerTri > 9) return false;

	INDICES_PER_TRIANGLE = indicesPerTri;
	return true;
};

	// Allocate enough memory
	/*i32 fsz = sizeof(f32)*3;
	i32 buffersz = (fsz*vertexCount) + (fsz*normalCount);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, buffersz,0, GL_STATIC_DRAW_ARB);

	// Copy data (vertices and normals for now)
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,0,fsz*vertexCount, vertex_data);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,fsz*vertexCount,
		fsz*normalCount, normal_data);

	// INDICES
	glGenBuffers(1, &mvbo.indices_vboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mvbo.indices_vboid);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(u32)*(triCount*6),triSet, GL_STATIC_DRAW);*/