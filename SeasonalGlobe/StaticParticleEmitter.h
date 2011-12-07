#pragma once
#include "ParticleEmitter.h"

// In this emitter, the emit function does nothing but set the velocity to (0,0,0)
// UpdateParticleProperties will reset the particles energy each frame, ensuring they
// will never die. This may be used for particles representing objects that will not die (e.g. leaves).
// We could later add functionality to finally make the leaves fall (e.g. by setting a "static_emitter" flag
// The user of the class must add each particle by hand, using the AddParticle(...) function
class StaticParticleEmitter : public ParticleEmitter
{
private:
	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p, const GameTime &gameTime);

	bool particlesStatic; // are the particles actually static?

	// maintains the index of the last particle we added, must be in range 0...local particle maximum
	// AddParticle(...) will fail if currentParticleIndex equals local particle maximum
	u32 currentParticleIndex;

public:
	StaticParticleEmitter();
	~StaticParticleEmitter();

	void SetParticlesStaticState(const bool particlesAreStatic);
	const bool GetParticlesStaticState() const;

	// Adds the particle at currentParticleIndex. Does NOT change any particle properties
	const bool AddParticle(const Particle &p);
};