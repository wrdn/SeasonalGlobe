#pragma once

#include "ParticleEmitter.h"

// Emits particles from a single point
// Uses a gradient map to find the particle colour
// Requires a gradient map shader
class GradientParticleEmitter : public ParticleEmitter
{
private:
	f32 particleSpread;
	Texture gradientMap;

public:
	GradientParticleEmitter();
	~GradientParticleEmitter();

	// Accessors and Mutators
	const f32 GetParticleSpread() const { return particleSpread; };
	void SetParticleSpread(const f32 spread) { particleSpread = spread; };
	void SetGradientMap(const Texture t) { gradientMap = t; };
};