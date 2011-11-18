#include "Sphere.h"
#include "util.h"
#include <math.h>

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
	VERTEX top( float3(0,radius,0), float3(0,1,0), float2(0,1));
	verts[vertexCount] = top;
	++vertexCount;

	// Bottom
	VERTEX bottom(float3(0,-radius,0), float3(0,-1,0), float2(0,0));
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
			
			VERTEX v(
				float3(radius*temp_radius*x, radius*y, radius*temp_radius*z),
				float3(temp_radius*x, y, temp_radius*z),
				float2((float)j/(float)slices, temp_tex));
			verts[vertexCount] = v;
			++vertexCount;
		}
		VERTEX lv = verts[temp_vcount];
		lv.uvs.y(temp_tex);
		verts[vertexCount] = lv;
		++vertexCount;
	}

	Model &sphereModel = GetModel();
	sphereModel.SetVertexArray(verts, vertexCount);
	
	indexCount = 0;
	vertexCount = 2;
	
	u32 indicesArraySz = ((2+(stacks-1)*(slices+1)*2) * 3) - 6;
	u32 *indicesArray = new u32[indicesArraySz];
	int index=0;

	// Top
	for(u32 j=0; j < slices; ++j)
	{
		indicesArray[index++] = 0;
		indicesArray[index++] = vertexCount;
		indicesArray[index++] = vertexCount+1;
		++vertexCount;
	}

	vertexCount -= (slices);

	// Middle (stacks)
	for(u32 i=0;i<(stacks-2); ++i)
	{
		for(u32 j=0;j<=slices;++j)
		{
			indicesArray[index++] = vertexCount;
			indicesArray[index++] = slices+vertexCount;
			indicesArray[index++] = slices+vertexCount+1;

			indicesArray[index++] = vertexCount;
			indicesArray[index++] = slices+vertexCount+1;
			indicesArray[index++] = vertexCount+1;

			++vertexCount;
		}
	}

	// Bottom
	for(u32 j=0; j <= slices; ++j)
	{
		indicesArray[index++] = 1;
		indicesArray[index++] = vertexCount+slices-j;
		indicesArray[index++] = vertexCount+slices-j-1;
	}

	sphereModel.SetIndicesArray(indicesArray, indicesArraySz);
	sphereModel.BuildVBO();

	return false;
};