#pragma once

#include "GameTime.h"
#include "Particle.h"
#include "Model.h"
#include "Shader.h"
#include <vector>

const u32 GLOBAL_MAX_PARTICLES_PER_EMITTER = 40000;

enum BillboardType { Cylindrical = 0, Spherical = 1 };

class ParticleEmitter : public glex // base abstract emitter class
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
	u32 rateOfEmission;

	// True by default, used to specify if rotations will occur
	// This variable was added to allow you to reuse the rotation vector
	// in a particle if rotations are not required
	bool applyRotations;

	// could be used when emitting particles to set their life
	f32 minLife, maxLife;

	// Forces
	std::vector<float3> forceVectors;

	// Drawing data
	Shader *emitterShader;
	Texture alphaMap; // at a minumum, particles should have a alpha texture
	GLenum sourceAlphaBlendFunction; // e.g. GL_ONE, GL_ONE_MINUS_SRC_ALPHA

	// NOTE: the matrix trick for spherical/cylindrical billboarding should be done in the shader
	void CylindricalBillboardAdjust() const;
	void SphericalBillboardAdjust() const;

	// Derived classes need to inherit this function which is used to set the data on new
	// particles. If required by the class, gdata could be used to pass information into
	// the emit function.
	
	//virtual void Emit(Particle &p, void *gdata)=0;
	virtual void Emit(Particle &p)=0;

	// called before drawing particles
	// if needed, you should set the uniform variables here
	// defaults to activating the shader and setting a uniform called "AlphaMap"
	virtual void ActivateShader(/*const GameTime &gameTime*/);

	// Do any post-draw deactivations. By default, the alpha map is deactivated
	// the shader is deactivated and the active texture is set to GL_TEXTURE0
	virtual void DeactivateShader();

	// Called in the update function. Use this function to update for example, particle colours.
	// The velocity and forces will be applied always, so do not apply them in this function.
	virtual void UpdateParticleProperties(Particle &p, const GameTime &gameTime)=0;


protected:
	// Only emitters need access to particles. This protected function allows derived
	// emitters to get the particle array. No other code needs it, so it is protected
	Particle * const GetParticles() const { return (Particle*)particles; };

public:

	ParticleEmitter();
	virtual ~ParticleEmitter();

	// Activates the shader, then drawing each particle in turn (up to localParticleMaximum)
	// If emitterShader is NULL, it will set the colour using fixed function functions and draw particles
	void Draw(/*const GameTime &gameTime*/);

	void Update(const GameTime &gameTime); // Defaults to applying forces (or just adding velocity is applyForces=false)

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

	void SetRateOfEmission(const u32 rate);
	const u32 GetRateOfEmission() const;

	void SetMaxParticleLife(const f32 maxLifeInSeconds);
	const f32 GetMaxParticleLife() const;
	void SetMinParticleLife(const f32 minLifeInSeconds);
	const f32 GetMinParticleLife() const;

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