#pragma once

#include "GraphicsObject.h"

//! Represents a sphere, generated programmatically
//! Inherits GraphicsObject
class Sphere : public GraphicsObject
{
private:
	f32 radius;
	u32 slices, stacks;
public:
	Sphere();
	~Sphere();

	const f32 GetRadius() const { return radius; };
	const u32 GetSlices() const { return slices; };
	const u32 GetStacks() const { return stacks; };

	//! Create sphere programmatically
	bool Create(f32 _radius, u32 _slices, u32 _stacks);
};