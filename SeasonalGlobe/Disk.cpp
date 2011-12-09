#include "Disk.h"
#include "util.h"
#include <math.h>

Disk::Disk(void) : radius(0), slices(0)
{
}

Disk::~Disk(void)
{
}

bool Disk::Create(u32 _slices, f32 _radius)
{
	slices = _slices;
	radius = _radius;

	u32 vertexCount = slices+1;
	VERTEX *floorVerts = new VERTEX[vertexCount]; // +1 for centre vertex (0,0,0)
	memset(floorVerts, 0, sizeof(VERTEX) * vertexCount);

	u32 *indexArray = new u32[slices * 3]; // 3 indices (verts) per tri
	memset(indexArray, 0, sizeof(u32) * (slices*3));

	f32 sliceinc = PI*2.0f/slices, angle=0; u32 indexipos=1, i=1;
	for(;i < vertexCount-1;++i)
	{
		floorVerts[i].pos.set(radius*cos(angle), -radius*sin(angle), 0);
		floorVerts[i].normal.set(0,1,0);
		floorVerts[i].uvs.set(cos(angle)*0.25f, sin(angle)*0.25f);

		angle += sliceinc;

		indexArray[indexipos] = i;
		indexArray[indexipos+1] = i+1;
		indexipos += 3;
	}
	floorVerts[i].pos.set(radius*cos(angle), -radius*sin(angle),0);
	floorVerts[i].normal.set(0,1,0);
	floorVerts[i].uvs.set(cos(angle)*0.25f, sin(angle)*0.25f);

	indexArray[indexipos] = i;
	indexArray[indexipos+1] = 1;

	this->GetModel().SetVertexArray(floorVerts, vertexCount);
	this->GetModel().SetIndicesArray(indexArray, slices*3);
	return this->GetModel().BuildVBO();
};