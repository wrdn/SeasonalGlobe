#include "Sphere.h"
#include "util.h"

#include <vector>
using namespace std;
Sphere::Sphere() : radius(0), slices(0), stacks(0) { };

bool Sphere::CreateSphere(f32 _radius, u32 _slices, u32 _stacks)
{
	if( ( _stacks < 2 ) && ( _slices < 2 ) ) return false;

	radius = _radius;
	slices = _slices;
	stacks = _stacks;

	f32 stack_increment = 1.0f / (float)stacks; // y
	f32 slice_increment = PI*2.0f / (float)slices; // x
	f32 x,y,z;
	u32 vertexCount=0; u32 indexCount=0;
	u32 arraySize = (slices+1)*(stacks-1)+2;
	VERTEX *verts = new VERTEX[arraySize];

	// Top
	VERTEX top;
	top.pos[0] = 0; top.pos[1] = radius; top.pos[2] = 0;
	top.norm[0] = 0; top.norm[1] = 1; top.norm[2] = 0;
	top.uvs[0] = 0; top.uvs[1] = 1;
	verts[vertexCount] = top;
	++vertexCount;

	// Bottom
	VERTEX bottom;
	bottom.pos[0] = 0; bottom.pos[1] = -radius; bottom.pos[2] = 0;
	bottom.norm[0] = 0; bottom.norm[1] = -1; bottom.norm[2] = 0;
	bottom.uvs[0] = 0; bottom.uvs[1] = 0;
	verts[vertexCount] = bottom;
	++vertexCount;

	for( u32 i = 1; i < stacks; ++i )
	{
		y = sin(PI*(1/2.0f - stack_increment * (float)i));
		f32 temp_radius = cos(PI*(1/2.0f - stack_increment * (float)i));
		f32 temp_tex = 1.0f - stack_increment * (float)i;
		u32 temp_vcount = vertexCount;

		// Create vertices around the sphere (slices)
		for( u32 j = 0; j < slices; ++j )
		{
			x = cos((float)j * slice_increment);
			z = -sin((float)j*slice_increment);
			
			VERTEX v;
			v.pos[0] = radius * temp_radius * x;
			v.pos[1] = radius * y;
			v.pos[2] = radius * temp_radius * z;
			v.norm[0] = temp_radius * x;
			v.norm[1] = y;
			v.norm[2] = temp_radius * z;
			v.uvs[0] = (float)j / (float)slices;
			v.uvs[1] = temp_tex;
			verts[vertexCount] = v;
			++vertexCount;
		}
		VERTEX lv = verts[temp_vcount];
		lv.uvs[1] = temp_tex;
		verts[vertexCount] = lv;
		++vertexCount;
	}

	Model &sphereModel = GetModel();
	sphereModel.SetVertexArray(verts, vertexCount);
	
	indexCount = 0;
	vertexCount = 2;
	
	vector<int> indicesArray;

	// Top
	for(u32 j=0; j <= slices; ++j)
	{
		indicesArray.push_back(0);
		indicesArray.push_back(vertexCount);
		indicesArray.push_back(vertexCount+1);
		++vertexCount;
	}
	vertexCount -= (slices+1);

	// Middle (stacks)
	for(u32 i=0;i<(stacks-2); ++i)
	{
		for(u32 j=0;j<=slices;++j)
		{
			indicesArray.push_back(vertexCount);
			indicesArray.push_back(slices+vertexCount);
			indicesArray.push_back(slices+vertexCount+1);

			indicesArray.push_back(vertexCount);
			indicesArray.push_back(slices+vertexCount+1);
			indicesArray.push_back(vertexCount+1);

			++vertexCount;
		}
	}

	// Bottom
	for(u32 j=0; j <= slices; ++j)
	{
		indicesArray.push_back(1);
		indicesArray.push_back(vertexCount+slices-j);
		indicesArray.push_back(vertexCount+slices-j-1);
	}

	sphereModel.SetIndicesArray((u32*)&indicesArray[0], indicesArray.size());
	sphereModel.BuildVBO();

	return false;
};