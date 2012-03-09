#pragma once

#include "ParticleEmitter.h"

class PointBasedParticleEmitter : public ParticleEmitter
{
private:
	f32 particleSpread;
	Color4f startColor, endColor;

	void Emit(Particle &p);

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