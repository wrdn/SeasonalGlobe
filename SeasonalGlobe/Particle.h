#pragma once

#include "Color.h"
#include "float3.h"

// 64 bytes
// Try to keep the Particle <= 64 bytes
// Note: if the memory is needed to keep under 64 bytes,
// ColorU32 could be used (represent Color as int)
// The rotation vector could also be reduced (only allow rotations
// on certain axis). The size vector could also be replaced by a single
// float if we used uniform scaling only
struct Particle
{
public:
	float3 pos, velocity, size, rotation;
	Color4f color;
	f32 energy;

	Particle();
	~Particle();
};