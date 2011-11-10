#include "Floor.h"
#include "util.h"
#include <math.h>

Floor::Floor(void) : slices(0), radius(0)
{
}


Floor::~Floor(void)
{
}

void Floor::CreateFloor(u32 _slices, f32 _radius)
{
	slices = _slices;
	radius = _radius;

	u32 vertexCount = slices+1;
	VERTEX *floorVerts = new VERTEX[vertexCount]; // +1 for centre vertex (0,0,0)
	memset(floorVerts, 0, sizeof(VERTEX) * vertexCount);

	u32 *indexArray = new u32[slices * 3]; // 3 indices (verts) per tri
	memset(indexArray, 0, sizeof(u32) * (slices*3));

	f32 sliceinc = PI*2.0f/slices, angle=0; u32 indexipos=1;
	for(u32 i=1; i < vertexCount; ++i)
	{
		// V = (radius*cos(angle), 0, -radius*sin(angle)
		floorVerts[i].pos[0] = radius * cos(angle);
		floorVerts[i].pos[2] = -radius * sin(angle);

		// N = (0,1,0)
		floorVerts[i].norm[1] = 1;

		// U = cos(angle)*0.25f, V = sin(angle)*0.25f
		floorVerts[i].uvs[0] = cos(angle) * 0.25f;
		floorVerts[i].uvs[1] = sin(angle) * 0.25f;

		angle += sliceinc;

		indexArray[indexipos] = i;
		indexArray[indexipos+1] = i+1;
		indexipos += 3;
	}
	indexArray[(slices*3)-1] = 1;

	floorModel.realVertexDataSz = vertexCount;
	floorModel.SetVertexData(floorVerts);

	floorModel.SetTriCount(slices);

	floorModel.SetIndicesArray(indexArray);

	floorModel.BuildVBO();
};