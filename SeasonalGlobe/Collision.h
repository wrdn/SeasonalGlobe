#pragma once

#include "Line.h"
#include "Circle.h"

struct CollisionPoint
{
public:
	float2 p1; f32 t1; // always the shortest distance
	float2 p2; f32 t2; // always the longest distance
};

bool LineCircleCollision(const Line2D &line, const Circle &c, CollisionPoint &cp);