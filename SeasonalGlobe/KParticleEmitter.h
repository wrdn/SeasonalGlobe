#pragma once

#include "KParticle.h"
#include "Texture.h"
#include "Model.h"
#include <GXBase.h>

typedef float3 ForceVector;
const float3 GRAVITY_VECTOR(0, -9.81f, 0);

enum BillboardType { Spherical, Cylindrical };

class KParticleEmitter
{
public:
	static const u32 GLOBAL_MAX_PARTICLES = 1000;
private:
	KParticle particles[GLOBAL_MAX_PARTICLES];

	// can set and change texture pointer, but can't change the object
	const Texture *emitterTexture;

	ForceVector gravityVector; // can change gravity (essentially a force) :)
	ForceVector windDirection;
	float3 emitterOrigin;

	GLenum sourceAlphaBlendFunction;

	u32 rateOfEmission; // particles to emit per update

	BillboardType billboardType;

	Model billboardModel;

	u32 maxEmitterParticles; // must be <= GLOBAL_MAX_PARTICLES

	// emit particle p. To emit particles you need to instantiate a
	// derived emitter with a specific emission function
	// Emmision functions set the data on parameter p. General data could
	// also be passed in then casted to the appropriate type, through the data void*
	virtual void Emit(KParticle &p, void *data)=0;
	virtual	void InitParticles()=0;
protected:
	KParticle* GetParticles() const { return (KParticle*)particles; };
public:
	void Init() { InitParticles(); };

	KParticleEmitter();
	virtual ~KParticleEmitter(); // virtual for safety (ensures derived class destructors called)

	void Draw(); // dumb function, draw all the particles at their appropriate positions

	// Defaults to adding velocity, wind direction and gravity,
	// but can be overriden to add other things in derived classes
	virtual void Update(const f32 dt);

	

	#pragma region Accessors and Mutators
	const BillboardType GetBillboardType() const { return billboardType; };
	void SetBillboardType(const BillboardType b) { billboardType = b; };

	const u32 GetRateOfEmission() const { return rateOfEmission; };
	void SetRateOfEmission(const u32 rate) { rateOfEmission = rate; };

	const float3& GetEmitterOrigin() const { return emitterOrigin; };
	void SetEmitterOrigin(const float3 &f) { emitterOrigin = f; };

	const float3& GetGravityVector() const { return gravityVector; };
	void SetGravityVector(const float3 &f) { gravityVector = f; };

	const float3& GetWindDirection() const { return windDirection; };
	void SetWindDirection(const float3& f) { windDirection = f; };

	const Texture * const GetTexture() const { return emitterTexture; };
	void SetTexture(const Texture * t) { emitterTexture = t; };

	const GLenum GetSourceAlphaBlendFunction() const { return sourceAlphaBlendFunction; };
	void SetSourceAlphaBlendFunction(const GLenum f) { sourceAlphaBlendFunction = f; };

	const Model & GetBillboardModel() const { return billboardModel; };
	void SetModel(const Model & _billboardModel) { billboardModel = _billboardModel; };

	const u32 GetMaxEmitterParticles() const { return maxEmitterParticles; };
	void SetMaxEmitterParticles(const u32 maxparticles) { maxEmitterParticles =  min(GLOBAL_MAX_PARTICLES,maxparticles); };

	#pragma endregion
};