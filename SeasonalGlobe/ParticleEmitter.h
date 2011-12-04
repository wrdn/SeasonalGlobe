#pragma once

#include "Particle.h"
#include "Model.h"
#include "Shader.h"
#include <vector>

const u32 GLOBAL_MAX_PARTICLES_PER_EMITTER = 1000;

enum BillboardType { Spherical, Cylindrical };

class ParticleEmitter // base abstract emitter class
{
private:
	Particle particles[GLOBAL_MAX_PARTICLES_PER_EMITTER]; // particles
	BillboardType billboardType;

	// usually we will only have 1 model
	// Set the default in particle system and it will be propogated to new emitters
	Model *model;

	// Emitter data
	float3 emitterOrigin;
	u32 localParticleMaximum;
	bool doUpdate, doDraw, doEmit; // can turn off specific emitter functionality

	// Forces
	bool applyForces; // false by default i.e. only add velocity, dont apply a force to it
	std::vector<float3> forceVectors;

	// Drawing data
	Shader *emitterShader;
	Texture alphaMap; // at a minumum, particles should have a alpha texture
	GLenum sourceAlphaBlendFunction; // e.g. GL_ONE, GL_ONE_MINUS_SRC_ALPHA

protected:
	// Only emitters need access to particles. This protected function allows derived
	// emitters to get the particle array. No other code needs it, so it is protected
	Particle * const GetParticles() const { return (Particle*)particles; };

	// Derived classes need to inherit this function which is used to set the data on new
	// particles. If required by the class, gdata could be used to pass information into
	// the emit function.
	virtual void Emit(Particle &p, void *gdata)=0;
public:

	virtual void Draw(); // Defaults to activating the shader, then drawing each particle in turn (up to localParticleMaximum)
	virtual void Update(const f32 dt); // Defaults to applying forces (or just adding velocity is applyForces=false)

	#pragma region Accessors and Mutators
	void SetBillboardType(const BillboardType btype);
	void SetModel(const Model *m);
	
	void SetEmitterOrigin(const float3& f);
	const float3& GetEmitterOrigin() const;
	
	void SetLocalParticleMaximum(const u32 localMax);
	const u32 GetLocalParticleMaximum() const;
	
	void DoUpdate(const bool shouldUpdate);
	const bool DoUpdate() const;
	void DoDraw(const bool shouldDraw);
	const bool DoDraw() const;
	void DoEmit(const bool shouldEmit);
	const bool DoEmit() const;

	void ApplyForces(const bool shouldApplyForces);
	const bool ApplyForces() const;
	void ClearForces();
	void AddForce(const float3 &f);
	const std::vector<float3>& GetForces() const;

	void SetShader(const Shader *shader);
	const Shader* GetShader() const;

	void SetAlphaMap(const Texture t);
	const Texture& GetAlphaMap() const;

	void SetSourceAlphaBlendFunction(const GLenum blendfunc);
	const GLenum GetSourceAlphaBlendFunction() const;
	#pragma endregion
};