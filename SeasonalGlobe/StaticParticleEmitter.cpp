#include "StaticParticleEmitter.h"

StaticParticleEmitter::StaticParticleEmitter() : particlesStatic(true), currentParticleIndex(0)
{
};

StaticParticleEmitter::~StaticParticleEmitter()
{
};

void StaticParticleEmitter::SetParticlesStaticState(const bool particlesAreStatic)
{
	particlesStatic = particlesAreStatic;
};

const bool StaticParticleEmitter::GetParticlesStaticState() const
{
	return particlesStatic;
};

void StaticParticleEmitter::Emit(Particle &p)
{
	p.velocity = float3();
};

void StaticParticleEmitter::UpdateParticleProperties(Particle &p, const GameTime &gameTime)
{
	p.energy = 1000;
};

const bool StaticParticleEmitter::AddParticle(const Particle &p)
{
	if(currentParticleIndex <= GetLocalParticleMaximum())
	{
		GetParticles()[currentParticleIndex++] = p;
		return true;
	}
	return false;
};