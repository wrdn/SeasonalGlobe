#pragma once

#include "Color.h"
#include "float3.h"

// Base particle structure containing common attributes
struct Particle
{
public:
	float3 pos, oldPos, velocity, size;
	Color4f color;
	f32 energy; // time to live

	// only x and z rotation supported
	f32 rotation_z;
	f32 rotation_x;

	f32 pada; // used for multiple purposes, depending on particle emitter e.g. fire particle emitter stores particle line ID in this variable

	Particle();
	~Particle();

	// invalidates (kills) the particle by setting energy (time to live) to -1 and setting color to (0,0,0,0), so it would never be displayed
	void InvalidateParticle()
	{
		energy = -1;
		color.zero();
	};
};