#pragma once

#include "Color.h"
#include "float3.h"

struct Particle
{
public:
	float3 pos, velocity, size;
	Color4f color;
	f32 energy;
	f32 originalEnergy;

	Particle();
	~Particle();
};