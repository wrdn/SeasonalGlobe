#include "Model.h"
#include "util.h"
#include "float4.h"

#include "float3.h"

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

Model::Model()
	: vertexArray(0), vertexArraySize(0), indicesArray(0), indicesArraySize(0),
	triangleDrawCount(0), drawMode(GL_FILL), triDrawMethod(TRIANGLES)
{
	glex::Load();
};

Model::~Model()
{
	//SAFE_DELETE_ARRAY(vertexArray);
	if(vertexArray)
	{
		delete vertexArray;
	}

	SAFE_DELETE_ARRAY(indicesArray);

	triangleDrawCount = 0;

	if(mvbo.modeldata_vboid)
		glDeleteBuffers(1, &mvbo.modeldata_vboid);
	if(mvbo.indices_vboid)
		glDeleteBuffers(1, &mvbo.indices_vboid);

	mvbo.modeldata_vboid = mvbo.indices_vboid = 0;
};

bool Model::BuildVBO()
{
	// No data
	if(!vertexArray || !vertexArraySize ||
		!indicesArray || !indicesArraySize)
	{
		return false;
	}

	if(mvbo.modeldata_vboid)
		glDeleteBuffers(1, &mvbo.modeldata_vboid);
	if(mvbo.indices_vboid)
		glDeleteBuffers(1, &mvbo.indices_vboid);
	mvbo.modeldata_vboid = mvbo.indices_vboid = 0;

	glGenBuffers(1, &mvbo.modeldata_vboid);
	if(!mvbo.modeldata_vboid) { return false; };
	glBindBuffer(GL_ARRAY_BUFFER, mvbo.modeldata_vboid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * vertexArraySize, vertexArray, GL_STATIC_DRAW);

	//glVertexPointer(3, GL_FLOAT, sizeof(VERTEX), BUFFER_OFFSET(VERTEX::POS_BUFFER_OFFSET));
	//glNormalPointer(GL_FLOAT, sizeof(VERTEX), BUFFER_OFFSET(VERTEX::NORMAL_BUFFER_OFFSET));
	//glTexCoordPointer(2, GL_FLOAT, sizeof(VERTEX), BUFFER_OFFSET(VERTEX::UV_BUFFER_OFFSET));

	glGenBuffers(1, &mvbo.indices_vboid);
	if(!mvbo.indices_vboid)
	{ 
		glDeleteBuffers(1, &mvbo.modeldata_vboid);
		mvbo.modeldata_vboid = mvbo.indices_vboid = 0;
		return false;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mvbo.indices_vboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesArraySize*sizeof(u32), indicesArray, GL_STATIC_DRAW);

	return true;
};

void Model::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);

	glBindBuffer(GL_ARRAY_BUFFER, mvbo.modeldata_vboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mvbo.indices_vboid);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(VERTEX), BUFFER_OFFSET(VERTEX::POS_BUFFER_OFFSET));
	glNormalPointer(GL_FLOAT, sizeof(VERTEX), BUFFER_OFFSET(VERTEX::NORMAL_BUFFER_OFFSET));
	glTexCoordPointer(2, GL_FLOAT, sizeof(VERTEX), BUFFER_OFFSET(VERTEX::UV_BUFFER_OFFSET));

	glDrawElements(triDrawMethod, indicesArraySize, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

ModelVBO::ModelVBO() : modeldata_vboid(0), indices_vboid(0)
{
};

ModelVBO::~ModelVBO()
{
};

void Model::DrawVertexPoints()
{
	glBegin(GL_POINTS);

	for(u32 i=0;i<vertexArraySize;++i)
	{
		glVertex3fv(vertexArray[i].pos.vec);
	}

	glEnd();
};