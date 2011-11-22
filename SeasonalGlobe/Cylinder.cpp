#include "Cylinder.h"


Cylinder::Cylinder(void) : radius(0), height(0), slices(0), stacks(0)
{
}

Cylinder::~Cylinder(void)
{
}

bool Cylinder::Create(f32 _radius, f32 _height, u32 _slices, u32 _stacks)
{
	radius = _radius;
	height = _height;
	slices = _slices;
	stacks = _stacks;

	// slices*2 for top and bottom, (slices*stacks) for body
	int vertArraySz = slices*2 + (slices*stacks);
	VERTEX *vertices = new VERTEX[vertArraySz];

	u32 indexArraySz = slices*(stacks-1)*4;
	u32 *indexArray = new u32[indexArraySz];

	f32 slice_inc = PI * 2.0f / slices;
	f32 half_height, current_height;
	half_height = current_height = height * 0.5f;
	f32 stack_inc = height / (stacks-1);

	// Top vertices
	u32 vcount = 0;
	f32 angle = 0;
	for (u32 slice = 0; slice < slices; ++slice)
	{
		vertices[vcount].pos.set(
			radius*cos(angle),
			half_height,
			-radius*sin(angle));
		
		vertices[vcount].normal.set(0,1,0);
		
		vertices[vcount].uvs.set( cos(angle) * 0.25f, sin(angle) * 0.25f);

		++vcount;
		angle += slice_inc;
	}

	// Body vertices
	//vcount = 0;
	angle = 0;
	for( u32 stack = 0; stack < stacks; ++stack)
	{
		for( u32 slice = 0; slice < slices; ++slice)
		{
			vertices[vcount].pos.set ( radius*cos(angle), current_height, -radius*sin(angle) );
			vertices[vcount].normal.set ( cos(angle), 0, -sin(angle) );
			vertices[vcount].uvs.set ( cos(angle)*0.25f, sin(angle)*0.25f );
			++vcount;
			angle += slice_inc;
		}
		current_height -= stack_inc;
	}

	// Bottom vertices
	angle = 0;
	for (u32 slice = 0; slice < slices; ++slice)
	{
		vertices[vcount].pos.set(
			radius*cos(angle),
			-half_height,
			-radius*sin(angle));
		
		vertices[vcount].normal.set(0,-1,0);
		
		vertices[vcount].uvs.set( 1-cos(angle)*0.25f, 1-sin(angle)*0.25f);

		++vcount;
		angle += slice_inc;
	}

	this->GetModel().SetVertexArray(vertices, vertArraySz);
	this->GetModel().SetIndicesArray(indexArray, indexArraySz);
	return this->GetModel().BuildVBO();
};