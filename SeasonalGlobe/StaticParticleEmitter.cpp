#include "StaticParticleEmitter.h"
#include "util.h"

StaticParticleEmitter::StaticParticleEmitter() : particlesStatic(true), currentParticleIndex(0), updateColor(false),
	startColor(1), endColor(1), particlesFalling(false), particlesDieing(false), particlesFadingIn(false), timeToChangeColor(5), timeToFadeOut(3), timeToFadeIn(3),
	timeToFall(3), maxYHeight(0)
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
	// create particle with no velocity so it cannot move
	p.color = this->startColor;
	p.velocity = float3();
	
	// if particles dieing, set time to live to time to fade out, else set it to the time to change color (leaf changes from green to brown)
	if(particlesDieing)
		p.energy = timeToFadeOut;
	else
		p.energy = timeToChangeColor;
	
	p.pada = p.energy; // NEED TO STORE ORIGINAL ENERGY TO LERP PROPERLY, t = 1-(1/original energy)*current energy
};

// initialise falling of the particles
void StaticParticleEmitter::InitiateParticleFall()
{
	particlesFalling=true;
	for(u32 i=0;i<currentParticleIndex;++i)
	{
		Particle &p = GetParticles()[i];

		// randomise velocity slightly
		p.velocity.x( randflt(-0.3f, 0.3f));
		p.velocity.y( -(maxYHeight/timeToFall) );
		p.velocity.z( randflt(-0.3f,0.3f));
	}
};

// initialise fade out of particles (when leaves hit ground)
void StaticParticleEmitter::InitiateParticleFadeOut()
{
	particlesDieing = true;
	updateColor = true;

	for(u32 i=0;i<currentParticleIndex;++i)
	{
		Particle &p = GetParticles()[i];
		p.velocity = float3();
		p.energy = timeToFadeOut;
		p.pada = timeToFadeOut;
	}
};

// fade in particles on tree
void StaticParticleEmitter::InitiateParticleFadeIn()
{
	updateColor = true;
	particlesFadingIn = true;

	for(u32 i=0;i<currentParticleIndex;++i)
	{
		Particle &p = GetParticles()[i];
		p.velocity = float3();
		p.energy = timeToFadeIn;
		p.pada = timeToFadeIn;
		p.color = fadeInColor;
	}
};

// start changing the color of the particles
void StaticParticleEmitter::InitiateMainColorChange()
{
	updateColor = true;
	particlesStatic = false;
	particlesDieing = particlesFadingIn = particlesFalling = false;

	for(u32 i=0;i<currentParticleIndex;++i)
	{
		Particle &p = GetParticles()[i];
		p.velocity = float3();
		p.energy = timeToChangeColor;
		p.pada = timeToChangeColor;
		p.color = startColor;
	}
};

void StaticParticleEmitter::UpdateParticleProperties(Particle &p)
{
	if(particlesDieing)
	{
		if(p.energy <= EPSILON)
		{
			p.energy = EPSILON; return;
		}

		p.color = endColor.vec_lerp(fadeOutColor, 1-(1.0f/p.pada)*p.energy);
		return;
	}
	else if(particlesFadingIn)
	{
		if(p.energy <= EPSILON)
		{
			p.energy = EPSILON; return;
		}

		p.color = fadeInColor.vec_lerp(startColor, 1-(1.0f/p.pada)*p.energy);
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
			p.rotation_x += randflt(-0.5f, 0.5f);

			p.pos.x( p.pos.x() + randflt(-0.05f,0.05f) );
			p.pos.z( p.pos.z() + randflt(-0.03f,0.03f) );

			p.velocity.x( randflt(-0.2f, 0.2f));
			p.velocity.z( randflt(-0.2f, 0.2f));

		}
		p.energy=100; // falling particles dont die
		return;
	}
	
	if(updateColor)
	{
		if(p.energy <= EPSILON)
		{
			p.energy = EPSILON; return;
		}

		p.color = startColor.vec_lerp(endColor, 1-(1.0f / p.pada)*p.energy);
	}
	else
	{
		p.color = this->startColor;
	}

	if(particlesStatic)
	{
		p.energy = 1; // dont update particle color
		return;
	}
};

// add particle to emitter - function required so we can easily add individual leaves to the emitter
const bool StaticParticleEmitter::AddParticle(const Particle &p)
{
	if(currentParticleIndex <= GetLocalParticleMaximum())
	{
		maxYHeight = max(p.pos.y(), maxYHeight);

		GetParticles()[currentParticleIndex++] = p;
		return true;
	}
	return false;
};