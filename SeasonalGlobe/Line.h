#pragma once

#include "float2.h"

struct Line2D
{
public:
	float2 start, end;

	Line2D() : start(), end() { };
	Line2D(float2 _start, float2 _end)
		: start(_start), end(_end)
	{ };
};