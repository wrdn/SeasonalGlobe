#pragma once

#include "CustomModel.h"

class Cylinder : public CustomModel
{
private:
	f32 topRadius,
		bottomRadius,
		height;
	u32 slices, stacks;
public:
	Cylinder(void);
	~Cylinder(void);

	bool Create(f32 _topRadius, f32 _bottomRadius, f32 _height, u32 _slices, u32 _stacks);
};

