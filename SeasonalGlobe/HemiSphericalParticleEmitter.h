#pragma once

#include "ParticleEmitter.h"

class HemiSphericalParticleEmitter : public ParticleEmitter
{
private:
	f32 hemisphere_radius;

	float4 startColor, endColor;

	f32 timeToFall;

	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p);

public:

	HemiSphericalParticleEmitter();
	~HemiSphericalParticleEmitter();

	void SetHemiSphereRadius(const f32 radius) { hemisphere_radius = radius; };
	const f32 GetHemiSphereRadius() const { return hemisphere_radius; };

	void SetTimeToFall(const f32 f) { timeToFall = f; }
	const f32 GetTimeToFall() const { return timeToFall; }
};