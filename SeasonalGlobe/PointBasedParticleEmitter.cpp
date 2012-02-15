#include "PointBasedParticleEmitter.h"

PointBasedParticleEmitter::PointBasedParticleEmitter() : particleSpread(1.5)
{
};

PointBasedParticleEmitter::~PointBasedParticleEmitter()
{
};

void PointBasedParticleEmitter::Emit(Particle &p)
{
	p.pos = GetEmitterOrigin();
	
	p.color = startColor;
	//p.col.SetColor(startColor.x(), startColor.y(), startColor.z(), startColor.a());

	p.energy = randflt(GetMinParticleLife(), GetMaxParticleLife());
	p.pada = p.energy;
	//p.originalEnergy = p.energy;

	p.velocity.x( randflt(-particleSpread, particleSpread));
	p.velocity.y(1.0f);
	p.velocity.z( randflt(-particleSpread, particleSpread));

	p.velocity.normalize();
};

void PointBasedParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	p.color = startColor.vec_lerp(endColor, 1-(1.0f/p.pada)*p.energy);
};