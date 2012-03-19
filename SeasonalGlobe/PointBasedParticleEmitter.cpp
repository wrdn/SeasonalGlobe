#include "PointBasedParticleEmitter.h"
#include "util.h"

PointBasedParticleEmitter::PointBasedParticleEmitter() : particleSpread(1.5)
{
};

PointBasedParticleEmitter::~PointBasedParticleEmitter()
{
};

void PointBasedParticleEmitter::Emit(Particle &p)
{
	// generate particle in random direction (velocity) from emitter origin, with randomised time to live between min and max time
	p.pos = GetEmitterOrigin();
	
	p.color = startColor;
	
	p.energy = randflt(GetMinParticleLife(), GetMaxParticleLife());
	p.pada = p.energy;
	
	p.velocity.x( randflt(-particleSpread, particleSpread));
	p.velocity.y(1.0f);
	p.velocity.z( randflt(-particleSpread, particleSpread));

	p.velocity.normalize();
};

void PointBasedParticleEmitter::UpdateParticleProperties(Particle &p)
{
	// lerp color over particle lifetime
	p.color = startColor.vec_lerp(endColor, 1-(1.0f/p.pada)*p.energy);
};