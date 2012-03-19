#pragma once

#include "float2.h"

// Bounded 2D line with start and end position
struct Line2D
{
public:
	float2 start, end;

	Line2D() {};
	Line2D(const float2 &_start, const float2 &_end)
		: start(_start), end(_end) {};
};