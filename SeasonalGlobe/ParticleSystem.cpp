#include "ParticleSystem.h"

bool ParticleSystem::RemoveEmitter(u32 index)
{
	if(index < handles.size())
	{
		delete handles[index].emitter;
		handles[index].emitter = 0; // invalidate emitter but keep handle
		return true;
	}
	return false;
};
/*
ParticleEmitter * const ParticleSystem::GetEmitter(u32 id)
{
	if(id < handles.size())
	{
		return handles[id].emitter;
	}
	return 0;
};
*/
void ParticleSystem::Clean()
{
	for(u32 i=0;i<handles.size();++i)
	{
		delete handles[i].emitter;
	}
	handles.clear();
	emitterSet.clear();
};

ParticleSystem::ParticleSystem() : defaultModel(0)
{
};

ParticleSystem::~ParticleSystem()
{
	try
	{
		Clean();
		defaultModel = 0; // nullify but dont delete
	}
	catch(...) { }
};

void ParticleSystem::Update(const GameTime &gameTime)
{
	for(std::vector<ParticleEmitter*>::const_iterator it = emitterSet.begin(); it != emitterSet.end(); ++it)
	{
		(*it)->Update(gameTime);
	}
};

void ParticleSystem::Draw(/*const GameTime &gameTime*/)
{
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	for(std::vector<ParticleEmitter*>::const_iterator it = emitterSet.begin(); it != emitterSet.end(); ++it)
	{
		glBlendFunc(GL_SRC_ALPHA, (*it)->GetSourceAlphaBlendFunction());
		(*it)->Draw(/*gameTime*/);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
};