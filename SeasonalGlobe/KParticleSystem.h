#pragma once

#include "KParticleEmitter.h"
#include <vector>

// manages all the particle emitters
class KParticleSystem
{
private:
	struct KEmitterHandle
	{
	public:
		KParticleEmitter * emitter;
		KEmitterHandle() : emitter(0) { };
		KEmitterHandle(KParticleEmitter *p) : emitter(p) { };
	};

	std::vector<KParticleEmitter*> emitterSet;
	std::vector<KEmitterHandle> handles;
public:
	template<class T>
	u32 AddEmitter() // Add emitter of specific type e.g. HemisphericalEmitter
	{
		emitterSet.push_back(new T());
		handles.push_back(KEmitterHandle(emitterSet.back()));
		return handles.size()-1;
	};

	bool RemoveEmitter(u32 index);

	// dont try and delete the returned value, use RemoveEmitter() instead
	KParticleEmitter * const GetEmitter(u32 id);

	// resets everything
	void Clean();

	void Update(const f32 dt);
	void Draw();

	KParticleSystem();
	~KParticleSystem();
};