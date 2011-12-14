#pragma once

#include "ParticleEmitter.h"
#include "float3.h"

// Emitter to generate particles in the shape of a cylinder between startPos and endPos
class CylindricalParticleEmitter : public ParticleEmitter
{
private:
	float3 startPos, endPos,
		direction; // direction recalculated whenever SetStartPosition or SetEndPosition called

	Color4f startColor, endColor;

	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/);
public:
	CylindricalParticleEmitter();
	~CylindricalParticleEmitter();

	void SetStartPosition(const float3 &f)
	{
		startPos = f;
		direction = endPos; direction -= startPos;
	};
	void SetEndPosition(const float3 &f)
	{
		endPos = f;
		direction = endPos; direction -= startPos;
	};

	const float3& GetStartPosition() const { return startPos; };
	const float3& GetEndPosition() const { return endPos; };

	void SetStartColor(const Color4f &startcol) { startColor = startcol; };
	void SetEndColor(const Color4f &endcol) { endColor = endcol; };
	const Color4f& GetStartColor() const { return startColor; };
	const Color4f& GetEndColor() const { return endColor; };
};