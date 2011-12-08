#pragma once

#include "ParticleEmitter.h"

class CylindricalParticleEmitter : public ParticleEmitter
{
private:
	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p, const GameTime &gameTime);

public:

	CylindricalParticleEmitter();
	~CylindricalParticleEmitter();
};