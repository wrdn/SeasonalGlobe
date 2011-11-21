#include "Cylinder.h"


Cylinder::Cylinder(void) : radius(0), height(0), slices(0), stacks(0)
{
}

Cylinder::~Cylinder(void)
{
}

void Cylinder::Create(f32 _radius, f32 _height, u32 _slices, u32 _stacks)
{
	radius = _radius;
	height = _height;
	slices = _slices;
	stacks = _stacks;

	// slices*2 for top and bottom, (slices*stacks) for body
	VERTEX *vertices = new VERTEX[slices*2 + (slices*stacks)];

	f32 slice_inc = PI * 2.0f / slices;
	f32 half_height, current_height;
	half_height = current_height = height * 0.5f;
	f32 stack_inc = height / (stacks-1);

	u32 vcount = 0;
	f32 angle = 0;
	for (u32 slice = 0; slice < slices; ++slice)
	{
		vertices[vcount].pos.set(
			radius*cos(angle), half_height, -radius*sin(angle));
		vertices[vcount].normal.set(0,1,0);
		vertices[vcount].uvs.set( cos(angle) * 0.25f, sin(angle) * 0.25f);

		++vcount;
		angle += slice_inc;
	}
};