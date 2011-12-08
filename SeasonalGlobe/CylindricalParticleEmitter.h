#pragma once

#include "ParticleEmitter.h"
#include "float3.h"

// Emitter to generate particles in the shape of a cylinder between startPos and endPos
class CylindricalParticleEmitter : public ParticleEmitter
{
private:
	float3 startPos, endPos,
		direction; // direction recalculated whenever SetStartPosition or SetEndPosition called

	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p, const GameTime &gameTime);
public:
	CylindricalParticleEmitter();
	~CylindricalParticleEmitter();

	void SetStartPosition(const float3 &f)
	{
		startPos = f;
		direction = endPos - startPos;
	};
	void SetEndPosition(const float3 &f)
	{
		endPos = f;
		direction = endPos - startPos;
	};

	const float3& GetStartPosition() const { return startPos; };
	const float3& GetEndPosition() const { return endPos; };
};