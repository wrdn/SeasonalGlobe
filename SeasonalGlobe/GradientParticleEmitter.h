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

	void Emit(Particle &p, void *gdata);
	void UpdateShader(const f32 dt);

public:
	GradientParticleEmitter();
	~GradientParticleEmitter();

	// Accessors and Mutators
	const f32 GetParticleSpread() const { return particleSpread; };
	void SetParticleSpread(const f32 spread) { particleSpread = spread; };
	void SetGradientMap(const Texture t)
	{
		gradientMap = t;
		gradientMap.SetTextureSlot(SLOT_GL_TEXTURE_1);
	};
};