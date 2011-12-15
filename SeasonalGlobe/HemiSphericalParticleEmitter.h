#pragma once

#include "ParticleEmitter.h"

struct CollisionFace
{
public:
	float3 v1, v2, v3;
	float3 N;

	void CalculateNormal()
	{
		float3 a = v2-v1;
		float3 b = v3-v1;
		N = a^b;
	};
};

class HemiSphericalParticleEmitter : public ParticleEmitter
{
private:
	f32 hemisphere_radius;

	float4 startColor, endColor;

	f32 timeToFall;

	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p);

	// 4 triangles used for collision against roof
	std::vector<CollisionFace> collisionFaces;

public:

	HemiSphericalParticleEmitter();
	~HemiSphericalParticleEmitter();

	void SetHemiSphereRadius(const f32 radius) { hemisphere_radius = radius; };
	const f32 GetHemiSphereRadius() const { return hemisphere_radius; };

	void SetTimeToFall(const f32 f) { timeToFall = f; }
	const f32 GetTimeToFall() const { return timeToFall; }
};