#pragma once

#include "ParticleEmitter.h"

// Emit particles over a hemisphere given a radius
class HemiSphericalParticleEmitter : public ParticleEmitter
{
private:
	f32 hemisphere_radius; // radius of hemisphere

	float4 startColor, endColor; // color to lerp between over particle lifetime

	f32 timeToFall; // how long it takes to reach the floor (snow)

	void Emit(Particle &p); // emit particle over hemisphere

	// This sets hemispherical particle color
	void UpdateParticleProperties(Particle &p);

public:

	HemiSphericalParticleEmitter();
	~HemiSphericalParticleEmitter();

	// Accessors and Mutators
	void SetHemiSphereRadius(const f32 radius) { hemisphere_radius = radius; };
	const f32 GetHemiSphereRadius() const { return hemisphere_radius; };

	void SetTimeToFall(const f32 f) { timeToFall = f; }
	const f32 GetTimeToFall() const { return timeToFall; }
};