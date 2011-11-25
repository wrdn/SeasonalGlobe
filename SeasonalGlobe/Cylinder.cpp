#include "Cylinder.h"


Cylinder::Cylinder(void) : topRadius(0), bottomRadius(0), height(0), slices(0), stacks(0)
{
}

Cylinder::~Cylinder(void)
{
}

#include <vector>
using namespace std;

bool Cylinder::Create(f32 _topRadius, f32 _bottomRadius, f32 _height, u32 _slices, u32 _stacks)
{
	topRadius = _topRadius;
	bottomRadius = _bottomRadius;
	height = _height;
	slices = _slices;
	stacks = _stacks;

	f32 sliceincrement = PI * 2.0f / slices;
	f32 half_height = height * 0.5f;
	f32 current_height = half_height;
	f32 stack_increment = height / (stacks - 1);
	f32 radius_increment = (topRadius - bottomRadius) / (stacks-1);
	f32 angle = 0;

	const u32 VertexArraySize = (slices*2) + (stacks*slices) + 2;
	VERTEX *vertexArray = new VERTEX[VertexArraySize];
	u32 vertexInsertionPos = 0;

	// Top (centre)
	vertexArray[vertexInsertionPos++] = VERTEX(float3(0,half_height,0), float3(0,1,0), float2(0.5, 0.5));
	
	// Bottom (centre)
	vertexArray[vertexInsertionPos++] = VERTEX(float3(0,-half_height,0), float3(0,-1,0), float2(0.5, 0.5));

	// Top vertices disk
	for( u32 currSlice = 0; currSlice < slices; ++currSlice, angle+=sliceincrement)
	{
		vertexArray[vertexInsertionPos++] = VERTEX(
			float3( topRadius * cos(angle), half_height, -topRadius * sin(angle) ),
			float3(0,1,0),
			float2(cos(angle) * 0.25f, -sin(angle) * 0.25f));
	}

	// Bottom vertices disk
	angle = 0;
	for( u32 currSlice = 0; currSlice < slices; ++currSlice, angle+=sliceincrement)
	{
		vertexArray[vertexInsertionPos++] = VERTEX(
			float3( bottomRadius * cos(angle), -half_height, -bottomRadius * sin(angle) ),
			float3(0,-1,0),
			float2(1-cos(angle) * 0.25f, 1-sin(angle) * 0.25f));
	}

	// Body vertices
	angle = 0;
	f32 currentRadius = topRadius;
	f32 v=0;
	f32 vincrement = 1.0f / (float)(stacks);
	//f32 uincrement = 1.0f / (float)slices;
	
	for ( u32 currStack = 0; currStack < stacks; ++currStack )
	{
		for (u32 currSlice = 0; currSlice < slices; ++currSlice )
		{
			f32 u = (float)currSlice / (float)(slices-1);

			vertexArray[vertexInsertionPos++] = VERTEX(
				float3(currentRadius*cos(angle), current_height, -currentRadius*sin(angle)),
				float3(cos(angle), 0, -sin(angle)), float2( u , v ));
			//cout << currStack << "," << currSlice << ": " << vertexArray[vertexInsertionPos-1].uvs;

			angle += sliceincrement;
		}

		//vertexArray[vertexInsertionPos-slices-1].uvs = float2(0.8, v);

		current_height -= stack_increment;
		currentRadius -= radius_increment;
		v += vincrement;
	}


	// Indices
	/*
	Array size:
	Top: slices*3
	Bottom: slices*3
	Top+Bottom=slices*6
	Body: (slices*(stacks-1))*6
	*/
	const u32 indexArraySize = (slices*6) + ((slices*(stacks-1))*6);
	u32 *indexArray = new u32[indexArraySize];
	u32 indexInsertionPos = 0;
	u32 vertexIndex = 2;

	// Top Indices
	for(u32 j=0; j < slices-1; ++j)
	{
		indexArray[indexInsertionPos++] = 0;
		indexArray[indexInsertionPos++] = vertexIndex;
		indexArray[indexInsertionPos++] = vertexIndex+1;
		++vertexIndex;
	}
	indexArray[indexInsertionPos++] = 0;
	indexArray[indexInsertionPos++] = slices+1;
	indexArray[indexInsertionPos++] = 2;

	// Bottom Indices
	vertexIndex = slices + 2;
	for(u32 j=0; j < slices-1; ++j)
	{
		indexArray[indexInsertionPos++] = 1;
		indexArray[indexInsertionPos++] = vertexIndex+1;
		indexArray[indexInsertionPos++] = vertexIndex;
		++vertexIndex;
	}
	indexArray[indexInsertionPos++] = 1;
	indexArray[indexInsertionPos++] = slices+2;
	indexArray[indexInsertionPos++] = slices*2+1;

	// Body indices
	vertexIndex = slices*2+2;
	for( u32 currStack = 0; currStack < stacks-1; ++currStack )
	{
		for (u32 currSlice = 0; currSlice < slices-1; ++currSlice )
		{
			indexArray[indexInsertionPos++] = vertexIndex;
			indexArray[indexInsertionPos++] = vertexIndex+slices;
			indexArray[indexInsertionPos++] = vertexIndex+slices+1;

			indexArray[indexInsertionPos++] = vertexIndex;
			indexArray[indexInsertionPos++] = vertexIndex+slices+1;
			indexArray[indexInsertionPos++] = vertexIndex+1;

			++vertexIndex;
		}

		indexArray[indexInsertionPos++] = vertexIndex;
		indexArray[indexInsertionPos++] = vertexIndex + slices;
		indexArray[indexInsertionPos++] = vertexIndex+1;

		indexArray[indexInsertionPos++] = vertexIndex+1;
		indexArray[indexInsertionPos++] = vertexIndex-slices+1;
		indexArray[indexInsertionPos++] = vertexIndex;

		++vertexIndex;
	}

	GetModel().SetVertexArray(vertexArray, VertexArraySize);
	GetModel().SetIndicesArray(indexArray, indexArraySize);
	return GetModel().BuildVBO();
};

/*
	vector<u32> indices;
	u32 vertexIndex = 2;

	// Top Indices
	for(u32 j=0; j < slices-1; ++j)
	{
		indices.push_back(0);
		indices.push_back(vertexIndex);
		indices.push_back(vertexIndex+1);
		++vertexIndex;
	}
	indices.push_back(0);
	indices.push_back(slices+1);
	indices.push_back(2);

	// Bottom Indices
	vertexIndex = slices + 2;
	for(u32 j=0; j < slices-1; ++j)
	{
		indices.push_back(1);
		indices.push_back(vertexIndex+1);
		indices.push_back(vertexIndex);
		++vertexIndex;
	}
	indices.push_back(1);
	indices.push_back(slices+2);
	indices.push_back(slices*2+1);

	// Body indices
	vertexIndex = slices*2+2;
	for( u32 currStack = 0; currStack < stacks-1; ++currStack )
	{
		for (u32 currSlice = 0; currSlice < slices-1; ++currSlice )
		{
			indices.push_back(vertexIndex);
			indices.push_back(vertexIndex+slices);
			indices.push_back(vertexIndex+slices+1);

			indices.push_back(vertexIndex);
			indices.push_back(vertexIndex+slices+1);
			indices.push_back(vertexIndex+1);

			++vertexIndex;
		}
		indices.push_back(vertexIndex);
		indices.push_back(vertexIndex + slices);
		indices.push_back(vertexIndex+1);

		indices.push_back(vertexIndex+1);
		indices.push_back(vertexIndex-slices+1);
		indices.push_back(vertexIndex);

		++vertexIndex;
	}

	u32 *indexArray = new u32[indices.size()];
	memcpy(indexArray, &indices[0], sizeof(u32) * indices.size());

	GetModel().SetVertexArray(vertexArray, VertexArraySize);
	GetModel().SetIndicesArray(indexArray, indices.size());
	return GetModel().BuildVBO();
*/