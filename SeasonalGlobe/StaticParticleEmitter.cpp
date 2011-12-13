#include "StaticParticleEmitter.h"

StaticParticleEmitter::StaticParticleEmitter() : particlesStatic(true), currentParticleIndex(0), updateColor(false),
	startColor(1), endColor(1), particlesFalling(false), particlesDieing(false), timeToChangeColor(5), timeToFade(3)
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
	p.color = this->startColor;
	p.velocity = float3();
	
	if(particlesDieing)
		p.energy = timeToFade;
	else
		p.energy = timeToChangeColor;
	
	p.pada = p.energy; // NEED TO STORE ORIGINAL ENERGY TO LERP PROPERLY, t = 1-(1/original energy)*current energy
};

void StaticParticleEmitter::InitiateParticleFall()
{
	particlesFalling=true;
	for(int i=0;i<currentParticleIndex;++i)
	{
		Particle &p = GetParticles()[i];

		p.velocity.x( randflt(-0.3, 0.3));
		p.velocity.y(-1.0f);
		p.velocity.z( randflt(-0.3,0.3));
		p.velocity.normalize();
	}
};

void StaticParticleEmitter::InitiateParticleFade()
{
	particlesDieing = true;

	for(int i=0;i<currentParticleIndex;++i)
	{
		Particle &p = GetParticles()[i];
		p.velocity = float3();
		p.energy = timeToFade;
		p.pada = timeToFade;
		p.color = endColor;
	}
};

void StaticParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	if(particlesDieing)
	{
		f32 r = lerp(endColor.r(), fadeColor.r(), 1-(1.0f/p.pada)*p.energy);
		f32 g = lerp(endColor.g(), fadeColor.g(), 1-(1.0f/p.pada)*p.energy);
		f32 b = lerp(endColor.b(), fadeColor.b(), 1-(1.0f/p.pada)*p.energy);
		f32 a = lerp(endColor.a(), fadeColor.a(), 1-(1.0f/p.pada)*p.energy);

		p.color = Color4f(r,g,b,a);
		return;
	}

	if(particlesFalling)
	{
		if(p.pos.y() <= 0.1)
		{
			p.velocity = float3();
		}
		else
		{
			p.rotation_z += 0.1f;
			p.rotation_x += randflt(-0.5, 0.5);

			p.pos.x( p.pos.x() + randflt(-0.05,0.05) );
			p.pos.z( p.pos.z() + randflt(-0.03,0.03) );

			p.velocity.x( randflt(-0.2, 0.2));
			p.velocity.z( randflt(-0.2, 0.2));

		}
		p.energy=100; // falling particles dont die
		return;
	}

	if(particlesStatic)
	{
		p.energy = 1; // dont update particle color
		return;
	}

	if(updateColor)
	{
		f32 r = lerp(startColor.r(), endColor.r(), 1-(1.0f / p.pada)*p.energy);
		f32 g = lerp(startColor.g(), endColor.g(), 1-(1.0f / p.pada)*p.energy);
		f32 b = lerp(startColor.b(), endColor.b(), 1-(1.0f / p.pada)*p.energy);
		f32 a = lerp(startColor.a(), endColor.a(), 1-(1.0f / p.pada)*p.energy);

		p.color = Color4f(r, g, b, a);
	}
	else
	{
		p.color = this->startColor;
	}
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