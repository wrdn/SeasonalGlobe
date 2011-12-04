#include "KParticleSystem.h"

bool KParticleSystem::RemoveEmitter(u32 index)
{
	if(index < handles.size())
	{
		delete handles[index].emitter;
		handles[index].emitter = 0; // invalidate emitter but keep handle
		return true;
	}
	return false;
};

KParticleEmitter * const KParticleSystem::GetEmitter(u32 id)
{
	if(id < handles.size())
	{
		return handles[id].emitter;
	}
	return 0;
};

void KParticleSystem::Clean()
{
	for(u32 i=0;i<handles.size();++i)
	{
		delete handles[i].emitter;
	}
	handles.clear();
	emitterSet.clear();
};

KParticleSystem::KParticleSystem()
{
};

KParticleSystem::~KParticleSystem()
{
	Clean();
};

void KParticleSystem::Update(const f32 dt)
{
	for(std::vector<KParticleEmitter*>::const_iterator it = emitterSet.begin(); it != emitterSet.end(); ++it)
	{
		(*it)->Update(dt);
	}
};

void KParticleSystem::Draw()
{
	for(std::vector<KParticleEmitter*>::const_iterator it = emitterSet.begin(); it != emitterSet.end(); ++it)
	{
		(*it)->Draw();
	}
};