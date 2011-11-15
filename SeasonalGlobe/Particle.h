#pragma once

#include "float3.h"
#include "Color.h"
#include <vector>

const f32 GRAVITY = -9.81f; //m/s^2

// particles can be batched by the texture they use
// therefore the texture need not be stored with each particle
struct Particle
{
public:
	float3 pos; // 12
	float3 velocity; // 12
	float3 scale; // 12
	f32 age, timeToLive; // 8
	ColorU32 color; // 4
};

// represents a "set" of particles e.g. snow or smoke particles
class ParticleEmitter
{
public:
	static const u32 MAX_PARTICLES = 1000;
	ParticleEmitter();
	void SetTextureID(u32 _id);

	const bool IsActive() const;
	void SetActive(bool _isActive);

	// Each emitter (typically very few will exist) can update and draw itself
	void Update(f32 dt);
	void Render();

private:
	Particle _particles[ParticleEmitter::MAX_PARTICLES]; // each particle emitter has a maximum
	u32 texID; // texture to use for particles in emitter
	bool isActive;
};

// Billboarding particle system
class ParticleSystem
{
private:

	// pointers for safety (memory won't move)
	std::vector<ParticleEmitter*> particleEmitters;

	bool isActive; // deactivate entire particle engine

	// Incremented when ParticleEmitter is deleted
	// When GetParticleEmitter called, final ID is: id-delCount
	// This way, items can be deleted, but IDs will always be valid
	// to the users of the particle system - cheaper than an std::map
	// or search (with IDs stored in objects)
	u32 delCount;

public:
	ParticleSystem();
	~ParticleSystem();

	void Update(const f32 dt); // loops over the emitters and updates each
	void Render() const; // loops over the emitters and renders each

	const bool IsActive() const;
	void SetActive(bool _isActive); // enable/disable system

	u32 AddParticleEmitter(); // returns the vector ID
	ParticleEmitter* GetParticleEmitter(const u32 id) const;
	bool DeleteParticleEmitter(const u32 id);

	// Deletes all the emitters and resets variables (isActive set true, delCount to 0)
	void Cleanup();
};