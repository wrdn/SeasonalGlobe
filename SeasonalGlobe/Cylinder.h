#pragma once

#include "CustomModel.h"

class Cylinder : public CustomModel
{
private:
	f32 radius, height;
	u32 slices, stacks;
public:
	Cylinder(void);
	~Cylinder(void);

	void Create(f32 _radius, f32 _height, u32 _slices, u32 _stacks);
};

