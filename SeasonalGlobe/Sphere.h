#pragma once

#include "CustomModel.h"

class Sphere : public CustomModel
{
private:
	f32 radius;
	u32 slices, stacks;
public:
	Sphere();

	const f32 GetRadius() const { return radius; };
	const u32 GetSlices() const { return slices; };
	const u32 GetStacks() const { return stacks; };

	bool CreateSphere(f32 _radius, u32 _slices, u32 _stacks);
};