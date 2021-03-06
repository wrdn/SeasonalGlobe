#pragma once

#include "float2.h"

// 2D circle with position and radius. Used with line circle collisions (in terrain loader)
struct Circle
{
public:
	float2 pos;
	f32 radius;

	Circle() : pos(), radius(0) { };
	Circle(const float2 &_pos, const f32 &_radius)
		: pos(_pos), radius(_radius)
	{ };
};