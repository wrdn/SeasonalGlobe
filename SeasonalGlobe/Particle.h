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
	float3 pos, velocity, size; // 36
	Color4f color; // 48
	f32 energy; // 52
	f32 rotation_z; // rotation only ever applies to the Z axis, 56
	
	// variables to pad structure to 64 bytes, use these for whatever you want
	// e.g. the fire emitter may use one of them to store the index of the line
	// the particle should be emitted across
	f32 pada;
	f32 padb;

	Particle();
	~Particle();
};