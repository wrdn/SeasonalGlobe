#pragma once

#include "ParticleEmitter.h"
#include "Color.h"

// Emits particles from a single point
// Uses a gradient map to find the particle colour
// Requires a gradient map shader
class GradientParticleEmitter : public ParticleEmitter
{
private:
	f32 particleSpread;
	Texture gradientMap;

	Color4f startColor, endColor;

	void Emit(Particle &p, void *gdata); // emit particles from a point

	void UpdateShader(const GameTime &gameTime); // update the shader to set the textures

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

	const Color4f& GetStartColor() const { return startColor; };
	void SetStartColor(const Color4f &c) { startColor = c; };

	const Color4f& GetEndColor() const { return endColor; };
	void SetEndColor(const Color4f &c) { endColor = c; };

	void UpdateParticle(Particle &p, const GameTime &gameTime)
	{
	};
};