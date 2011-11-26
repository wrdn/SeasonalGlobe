#pragma once

#include "Model.h"

class Sphere : public Model
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

	void test()
	{
		VERTEX *v = new VERTEX[100];
		SetVertexArray(v, 100);

		u32 *i = new u32[100];
		SetIndicesArray(i, 100);
	};

	bool CreateSphere(f32 _radius, u32 _slices, u32 _stacks);
};