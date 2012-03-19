#pragma once

#include "GameTime.h"
#include "ParticleEmitter.h"
#include <vector>

// The particle system manages all particles. Though written much later, the resource manager design was influenced by
// the particle system design, collecting resource handles
// The ParticleSystem uses emitter handles (struct containing pointer to emitter) and emitters. A new emitter can be created
// which will create a new emitter, and create an associated handle for it. The user is then given an integer ID for the emitter
// handle. Handles are never deleted, however the associated memory can be. This way, the integer emitter handles will NEVER
// be invalid, even when the emitter is deleted (and pointer nullified).
// With an alternative approach, we may have delete the emitter, and removed it from the vector. If vector indices were used,
// this deletion operation would invalidate IDs to all emitters created after the deleted one was created.
// Example: Given emitters with IDs: 0,1,2,3,4,5. If emitter 2 was deleted, indices 3,4,5 would become indices 2,3,4 (old index - 1)
// With this approach, regardless of whether an emitter is deleted, the emitter handle (which is never deleted) still points to the memory
// location of the emitters beyond the deleted one. Thus the handles are always valid.
class ParticleSystem
{
private:

	// An emitter handle simply contains a pointer to an emitter - thus, we can return a handle with no emitter,
	// rather than simply a NULL referenced
	struct EmitterHandle
	{
	public:
		ParticleEmitter * emitter;
		EmitterHandle() : emitter(0) { };
		explicit EmitterHandle(ParticleEmitter *p) : emitter(p) { };
	};

	std::vector<ParticleEmitter*> emitterSet; // the actual particle emitters
	std::vector<EmitterHandle> handles; // the emitter handle to the associated emitter

	MeshHandle defaultModel;
public:

	ParticleSystem();
	~ParticleSystem();

	// Add emitter of specific type e.g. HemisphericalEmitter
	template<class T>
	u32 AddEmitter()
	{
		emitterSet.push_back(new T());
		emitterSet.back()->SetModel(defaultModel);
		handles.push_back(EmitterHandle(emitterSet.back()));
		return handles.size()-1;
	};

	// Remove emitter at index
	bool RemoveEmitter(u32 index);

	// Returns the emitter at specified id (or NULL if id invalid)
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

	// Sets default model for particle emitters (usually a quad)
	void SetDefaultModel(MeshHandle m) { defaultModel = m; };
	MeshHandle GetDefaultModel() const { return defaultModel; };

	// Remove all the emitters (doesn't alter defaultModel)
	void Clean();

	// Calls Update all particle emitters
	void Update(const GameTime &gameTime);

	// Calls Draw on all particle emitters
	void Draw();
};