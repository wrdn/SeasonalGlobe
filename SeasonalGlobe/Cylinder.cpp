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
	f32 current_height = height;
	f32 stack_increment = height / (stacks - 1);
	f32 radius_increment = (topRadius - bottomRadius) / (stacks-1);
	f32 angle = 0;

	// slices*2+2 for top and bottom, (stacks*slices)+stacks for body (with 'stacks' replicated vertices)
	const u32 VertexArraySize = (slices*2) + 2 + (stacks*slices) + stacks;
	VERTEX *vertexArray = new VERTEX[VertexArraySize];
	u32 vertexInsertionPos = 0;

	// Top (centre)
	vertexArray[vertexInsertionPos++] = VERTEX(float3(0,height,0), float3(0,1,0), float2(0.5, 0.5));
	
	// Bottom (centre)
	vertexArray[vertexInsertionPos++] = VERTEX(float3(0,0,0), float3(0,-1,0), float2(0.5, 0.5));

	// Top vertices disk
	for( u32 currSlice = 0; currSlice < slices; ++currSlice, angle+=sliceincrement)
	{
		vertexArray[vertexInsertionPos++] = VERTEX(
			float3( topRadius * cos(angle), height, -topRadius * sin(angle) ),
			float3(0,1,0),
			float2(0.5f - sin(angle) * topRadius / (2.0f*topRadius), 0.5f + cos(angle) * topRadius / (2.0f*topRadius)));
	}

	// Bottom vertices disk
	angle = 0;
	for( u32 currSlice = 0; currSlice < slices; ++currSlice, angle+=sliceincrement)
	{
		vertexArray[vertexInsertionPos++] = VERTEX(
			float3( bottomRadius * cos(angle), 0, -bottomRadius * sin(angle) ),
			float3(0,-1,0),
			float2((0.5f - sin(angle) * bottomRadius / (2.0f*bottomRadius)), (0.5f - cos(angle) * bottomRadius / (2.0f*bottomRadius))));
	}

	// Body vertices
	angle = 0;
	f32 currentRadius = topRadius;
	f32 v=1;
	f32 vincrement = 1.0f / (float)(stacks-1);
	
	for ( u32 currStack = 0; currStack < stacks; ++currStack )
	{
		// Replicate first vertex (this will be connected to the last vertex with correct UVs)
		vertexArray[vertexInsertionPos++] = VERTEX(
				float3(currentRadius*cos(angle), current_height, -currentRadius*sin(angle)),
				float3(cos(angle), 0, -sin(angle)), float2( 1.0f , v ));

		for (u32 currSlice = 0; currSlice < slices; ++currSlice )
		{
			f32 u = (float)currSlice / (float)(slices);

			vertexArray[vertexInsertionPos++] = VERTEX(
				float3(currentRadius*cos(angle), current_height, -currentRadius*sin(angle)),
				float3(cos(angle), 0, -sin(angle)), float2( u , v ));
			
			angle += sliceincrement;
		}
		current_height -= stack_increment;
		currentRadius -= radius_increment;
		v -= vincrement;
		angle = 0;
	}

	// Indices
	/*
	Array size:
	Top: slices*3
	Bottom: slices*3
	Top+Bottom=slices*6
	Body: (slices*(stacks-1))*6
	stacks + (Body): stacks is for the replicated vertices
	*/
	const u32 indexArraySize = (slices*6) + ((stacks*3) + ((slices*(stacks-1))*6));
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
	vertexIndex = slices*2+2+1;
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
		indexArray[indexInsertionPos++] = vertexIndex+slices;
		indexArray[indexInsertionPos++] = vertexIndex+slices+1;

		u32 replicatedVertexIndex = vertexIndex - slices;

		indexArray[indexInsertionPos++] = replicatedVertexIndex + slices + 1;
		indexArray[indexInsertionPos++] = replicatedVertexIndex;
		indexArray[indexInsertionPos++] = vertexIndex;

		indexArray[indexInsertionPos++] = replicatedVertexIndex + slices + 1;
		indexArray[indexInsertionPos++] = replicatedVertexIndex + slices;
		indexArray[indexInsertionPos++] = vertexIndex+slices+1;

		vertexIndex += 2; // move over the replicated vertex
	}

	this->SetVertexArray(vertexArray, VertexArraySize);
	this->SetIndicesArray(indexArray, indexArraySize);
	return this->BuildVBO();
};