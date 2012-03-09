#pragma once

#include "GraphicsObject.h"

//! Origin of cylinder is its centre
class Cylinder : public GraphicsObject
{
private:
	f32 topRadius,
		bottomRadius,
		height;
	u32 slices, stacks;
public:

	//! constructor initialises everything to 0
	Cylinder(void);
	
	~Cylinder(void);

	//! Function to create the mesh for the cylinder programmatically
	bool Create(f32 _topRadius, f32 _bottomRadius, f32 _height, u32 _slices, u32 _stacks);

	const f32 GetTopRadius() const { return topRadius; };
	const f32 GetBottomRadius() const { return bottomRadius; };
	const f32 GetHeight() const { return height; };

	const u32 GetSlices() const { return slices; };
	const u32 GetStacks() const { return stacks; };
};