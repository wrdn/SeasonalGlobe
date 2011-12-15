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
	f32 lR = lerp(startColor.r(), endColor.r(), 1-(1.0f/p.pada)*p.energy);
	f32 lG = lerp(startColor.g(), endColor.g(), 1-(1.0f/p.pada)*p.energy);
	f32 lB = lerp(startColor.b(), endColor.b(), 1-(1.0f/p.pada)*p.energy);
	f32 lA = lerp(startColor.a(), endColor.a(), 1-(1.0f/p.pada)*p.energy);

	p.color = Color4f(lR, lG, lB, lA);
};