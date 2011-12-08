#include "FireParticleEmitter.h"

FireParticleEmitter::FireParticleEmitter() : currentParticleAdditionIndex(0), maxParticlesPerLine(20), startColor(1,0,0,1), endColor(1, 0.28f, 0, 0.8f)
{
};

FireParticleEmitter::~FireParticleEmitter()
{
};

void FireParticleEmitter::Emit(Particle &p)
{
	ParticleLine &pline = particleLines[(u32)p.pada]; // line the particle needs to be emitted on

	// Ray equation: O + tD, where O is origin, D is direction and t is distance along line. End point
	// of ParticleLine is startPos + (1*direction)
	f32 t = randflt(0, 1);
	p.pos = pline.GetStartPosition() + (t * pline.GetDirection());

	p.velocity = float3(0.2f, randflt(0,1), 0.2f);
	p.velocity.normalize();

	p.size.setall(0.5f);

	p.color = startColor;
	p.energy = randflt(0, 1.0);
};

void FireParticleEmitter::UpdateParticleProperties(Particle &p, const GameTime &gameTime)
{
	f32 lR = lerp(startColor.r(), endColor.r(), 1.0f / p.energy);
	f32 lG = lerp(startColor.g(), endColor.g(), 1.0f / p.energy);
	f32 lB = lerp(startColor.b(), endColor.b(), 1.0f / p.energy);
	f32 lA = lerp(startColor.a(), endColor.a(), 1.0f / p.energy);

	p.color = Color4f(lR, lG, lB, lA);
};

void FireParticleEmitter::AddLine(const ParticleLine &line)
{
	particleLines.push_back(line);
	u32 index = particleLines.size()-1;
	
	for(u32 i=currentParticleAdditionIndex; i < (currentParticleAdditionIndex + maxParticlesPerLine); ++i)
	{
		GetParticles()[i].pada = index;
	}
};