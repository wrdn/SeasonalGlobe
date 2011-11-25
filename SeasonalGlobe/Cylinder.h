#pragma once

#include "Model.h"

class Cylinder : public Model
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

	const f32 GetTopRadius() const { return topRadius; };
	const f32 GetBottomRadius() const { return bottomRadius; };
	const f32 GetHeight() const { return height; };

	const u32 GetSlices() const { return slices; };
	const u32 GetStacks() const { return stacks; };
};

