#include "Particle.h"

ParticleEmitter::ParticleEmitter()
	: texID(0), isActive(true) // non valid ID
{
};

ParticleSystem::ParticleSystem() : isActive(true), delCount(0)
{
};

ParticleSystem::~ParticleSystem()
{
	Cleanup();
};

const bool ParticleSystem::IsActive() const { return isActive; };
void ParticleSystem::SetActive(bool _isActive) { isActive = _isActive; };

u32 ParticleSystem::AddParticleEmitter() // returns the vector ID
{
	particleEmitters.push_back(new ParticleEmitter());
	return particleEmitters.size()-1;
};

ParticleEmitter* ParticleSystem::GetParticleEmitter(const u32 id) const
{
	const u32 pos = id-delCount;
	if(pos < particleEmitters.size())
	{
		return particleEmitters[pos];
	}
	return NULL;
};

bool ParticleSystem::DeleteParticleEmitter(const u32 id)
{
	const u32 pos = id-delCount;
	if(pos < particleEmitters.size())
	{
		if(particleEmitters[pos])
		{
			ParticleEmitter *p = particleEmitters[pos];
			delete p;
			p = NULL;
		}

		particleEmitters.erase(particleEmitters.begin() + pos);

		++delCount;
		return true;
	}
	return false;
};

void ParticleSystem::Update(const f32 dt)
{
	if(isActive)
	{
		// Update all the active emitters
		for( std::vector<ParticleEmitter*>::const_iterator it=particleEmitters.begin(); it != particleEmitters.end(); ++it)
		{
			if((*it)->IsActive())
			{
				(*it)->Update(dt);
			}
		}
	}
};

void ParticleSystem::Render() const
{
	// Render all the active emitters
	if(isActive)
	{
		for( std::vector<ParticleEmitter*>::const_iterator it = particleEmitters.begin(); it != particleEmitters.end(); ++it)
		{
			if((*it)->IsActive())
			{
				(*it)->Render();
			}
		}
	}
};

void ParticleSystem::Cleanup()
{
	isActive = false; // prevents updates during cleanup operation
	for(std::vector<ParticleEmitter*>::iterator it = particleEmitters.begin(); it != particleEmitters.end(); ++it)
	{
		if(*it)
		{
			delete *it;
			*it = NULL;
		}
	}
	delCount = 0;
	isActive = true;
};

const bool ParticleEmitter::IsActive() const { return isActive; };

void ParticleEmitter::SetActive(bool _isActive) { isActive = _isActive; };

void ParticleEmitter::Update(f32 dt)
{
	// get rid of unreferenced formal parameter warning
	// (remove when Update(f32 dt) implemented)
	if(dt) { }
};

void ParticleEmitter::Render()
{
};