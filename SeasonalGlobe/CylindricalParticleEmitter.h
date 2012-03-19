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

	// overrided emit particle (from ParticleEmitter). Used to emit a new particle in the cylinder shape
	void Emit(Particle &p);

	// Update properties of particle per ParticleEmitter::Update() call.
	// This is for non-common properties relating the particle to the particle emitter
	// This sets cylindrical particle color
	void UpdateParticleProperties(Particle &p);
public:
	CylindricalParticleEmitter();
	~CylindricalParticleEmitter();

	// Accessors and Mutators
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