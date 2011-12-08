#pragma once

#include "ParticleEmitter.h"

class ParticleLine // line particles will be generated along
{
private:
	float3 startPos, endPos, direction;

	// depth of the tree (branch depth) the particles are at
	// may be useful if we want to fade by depth
	u32 lineDepth;

	void CalculateDirection() { direction = endPos - startPos; };
public:
	ParticleLine() : startPos(), endPos(), direction() { };
	~ParticleLine() { };

	void SetLineDepth(u32 d) { lineDepth = d; };
	void SetStartPosition(const float3 &start)
	{
		startPos = start;
		CalculateDirection();
	};
	void SetEndPosition(const float3 &end)
	{
		endPos = end;
		CalculateDirection();
	};
	const u32 GetLineDepth() const { return lineDepth; };
	const float3& GetStartPosition() const { return startPos; };
	const float3& GetEndPosition() const { return endPos; };
	const float3& GetDirection() const { return direction; };
};

class FireParticleEmitter : public ParticleEmitter
{
private:
	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p, const GameTime &gameTime);
public:
};