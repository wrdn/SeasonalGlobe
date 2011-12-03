#pragma once

#include <vector>
#include "ctypes.h"
#include "ParticleEmitter.h"

class ParticleSystem
{
private:
	struct EmitterHandleBase
	{
	public:
		bool IsActive;
		ParticleEmitter * emitter;

		EmitterHandleBase() : IsActive(false), emitter(0) {};
		EmitterHandleBase(ParticleEmitter *p) : IsActive(false), emitter(p) {};
		~EmitterHandleBase() {};
	};

	// initially -1 (no max), set to 0 to allow none, or no new emitters, or N
	i32 maximumEmitters;

	std::vector<ParticleEmitter> emitters;
	std::vector<EmitterHandleBase> handles;

public:
	struct EmitterHandle_t
	{
	private:
		EmitterHandleBase *parentHandle;
	public:

		// required so we can have objects we assign to later
		// with the default constructor, the object is useless (no parent)
		EmitterHandle_t() : parentHandle(0) { };

		// since only code within ParticleSystem can access the base emitter handles,
		// the ParticleSystem class is the only code that can create new emitters
		// This is exactly the behaviour we want
		EmitterHandle_t(EmitterHandleBase *p)
		{
			parentHandle = p;
			if(p)
				parentHandle->IsActive = true;
		};

		ParticleEmitter * const Get()
		{
			return parentHandle->emitter;
		};

		void Invalidate()
		{
			if(parentHandle)
			{
				parentHandle->IsActive = false;
				parentHandle = 0;
			}
		};
	};

	void Init(i32 maximum_emitter_count=-1)
	{
		emitters.clear();
		handles.clear();

		maximumEmitters = maximum_emitter_count;
		if(maximumEmitters == 0) return; // wont have any emitters

		if(maximumEmitters > 0)
		{
			emitters.reserve(maximumEmitters);
			handles.reserve(maximumEmitters);
		}
		else if(maximumEmitters < 0)
		{
			emitters.reserve(10);
			handles.reserve(10);
			//handles.resize(emitters.size());
		}
		
		for(u32 i=0;i<emitters.capacity();++i)
		{
			emitters.push_back(ParticleEmitter());
			handles.push_back(EmitterHandleBase(&emitters.back()));
				//handles[i].emitter = &emitters[i];
		}
	};

	// Returns a new emitter if one exists
	// If maximumEmitters <0 it will always return a valid
	// emitter (either one not in use, or another will be
	// added to the system). If maximumEmitters is 0, it will
	// attempt to find an unused emitter in the current list
	// but will not add more.
	// If maximumEmitters > 0, it will search the current emitter
	// list, then add a new one. If the total number of emitters
	// is equal to maximumEmitters, we do not add a new one and an
	// invalid emitter is returned
	EmitterHandle_t GetEmitter()
	{
		// First search current emitter list
		for(u32 i=0;i<handles.size();++i)
		{
			if(!handles[i].IsActive)
			{
				return EmitterHandle_t(&handles[i]);
			}
		}

		// If we are here, we couldn't find one in the current list
		// If maximum emitters == 0, we return an invalid handle

		if(maximumEmitters < 0)
		{
			// Add new emitter and return it
			emitters.push_back(ParticleEmitter());
			handles.push_back(EmitterHandleBase(&emitters.back()));
			return EmitterHandle_t(&handles.back());
		}
		else if(maximumEmitters > 0)// maximum emitters > 0
		{
			if( (emitters.size()+1) <= (u32)maximumEmitters )
			{
				// Add emitter and return
				emitters.push_back(ParticleEmitter());
				handles.push_back(EmitterHandleBase(&emitters.back()));
				return EmitterHandle_t(&handles.back());
			}
		}

		return EmitterHandle_t(0); // return invalid emitter if we reach here
	};

	ParticleSystem(void);
	~ParticleSystem(void);

	void Update(const f32 dt) // update the active emitters
	{
		for(std::vector<EmitterHandleBase>::const_iterator it = handles.begin(); it < handles.end(); ++it)
		{
			if(it->IsActive)
				it->emitter->Update(dt);
		}
	};

	void Draw(f32 dt) const // draw the active emitters
	{
		for(std::vector<EmitterHandleBase>::const_iterator it = handles.begin(); it < handles.end(); ++it)
		{
			if(it->IsActive)
			{
				//glPushMatrix();
				it->emitter->Draw(dt);
				//glPopMatrix();
			}
		}
	};
};

typedef ParticleSystem::EmitterHandle_t EmitterHandle;