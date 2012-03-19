#pragma once

#include "ParticleEmitter.h"

// Emit particles in given spread from a point (used for smoke)
class PointBasedParticleEmitter : public ParticleEmitter
{
private:
	f32 particleSpread; // spread (width) of particles

	// color to lerp between over particle lifetime
	Color4f startColor, endColor;

	// emit particle from point in spread (e.g. from chimney for smoke)
	void Emit(Particle &p);

	// sets particle color
	void UpdateParticleProperties(Particle &p);

public:
	PointBasedParticleEmitter();
	~PointBasedParticleEmitter();

	// Accessors and Mutators
	const f32 GetParticleSpread() const { return particleSpread; };
	void SetParticleSpread(const f32 spread) { particleSpread = spread; };

	const Color4f GetStartingColor() const { return startColor; };
	void SetStartingColor(const Color4f &c) { startColor = c; };
	const Color4f GetEndingColor() const { return endColor; };
	void SetEndingColor(const Color4f &c) { endColor = c; };
};