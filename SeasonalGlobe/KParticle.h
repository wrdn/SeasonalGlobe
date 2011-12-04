#pragma once

#include "float3.h"
#include "Color.h"

// Basic particle Plain Old Data (POD) structure. No logic or functions as these
// are provided by KParticleEmitters
struct KParticle
{
public:
	float3 pos, velocity, size;
	Color4f color;
	f32 energy;
	f32 originalEnergy;

	KParticle() : pos(), velocity(0,1,0), size(1.0f),
		color(Color::WHITE), energy(0)
	{
	};
	~KParticle() { };
};