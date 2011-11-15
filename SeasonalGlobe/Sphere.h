#pragma once

#include "Model.h"

class Sphere
{
private:
	Model sphereModel;
	f32 radius;
	u32 slices, stacks;
public:
	Sphere();
	bool CreateSphere(f32 _radius, u32 _slices, u32 _stacks);
	Model* GetModel() { return &sphereModel; };
};