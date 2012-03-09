#pragma once

#include "GameTime.h"
#include "ParticleEmitter.h"
#include <vector>

// manages all the particle emitters
class ParticleSystem
{
private:
	struct EmitterHandle
	{
	public:
		ParticleEmitter * emitter;
		EmitterHandle() : emitter(0) { };
		explicit EmitterHandle(ParticleEmitter *p) : emitter(p) { };
	};

	std::vector<ParticleEmitter*> emitterSet;
	std::vector<EmitterHandle> handles;

	MeshHandle defaultModel;
public:

	ParticleSystem();
	~ParticleSystem();

	template<class T>
	u32 AddEmitter() // Add emitter of specific type e.g. HemisphericalEmitter
	{
		emitterSet.push_back(new T());
		emitterSet.back()->SetModel(defaultModel);
		handles.push_back(EmitterHandle(emitterSet.back()));
		return handles.size()-1;
	};

	bool RemoveEmitter(u32 index);

	// Don't call "delete" on the pointer returned by this. Use RemoveEmitter(id) instead
	template<class T>
	T * const GetEmitter(u32 id)
	{
		if(id < handles.size())
		{
			return (T*)handles[id].emitter;
		}
		return 0;
	};

	void SetDefaultModel(MeshHandle m) { defaultModel = m; };
	MeshHandle GetDefaultModel() const { return defaultModel; };

	// Remove all the emitters (doesn't alter defaultModel)
	void Clean();

	void Update(const GameTime &gameTime);
	void Draw();
};