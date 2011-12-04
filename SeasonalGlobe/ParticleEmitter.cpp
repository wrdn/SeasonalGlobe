#include "ParticleEmitter.h"

#pragma region Accessors and Mutators
void ParticleEmitter::SetBillboardType(const BillboardType btype) { billboardType = btype; };
void ParticleEmitter::SetModel(const Model *m) { model = (Model*)m; };

void ParticleEmitter::SetEmitterOrigin(const float3& f) { emitterOrigin = f; };
const float3& ParticleEmitter::GetEmitterOrigin() const { return emitterOrigin; };

void ParticleEmitter::SetLocalParticleMaximum(const u32 localMax) { localParticleMaximum = localMax; };
const u32 ParticleEmitter::GetLocalParticleMaximum() const { return localParticleMaximum; };

void ParticleEmitter::DoUpdate(const bool shouldUpdate) { doUpdate = shouldUpdate; };
const bool ParticleEmitter::DoUpdate() const { return doUpdate; };
void ParticleEmitter::DoDraw(const bool shouldDraw) { doDraw = shouldDraw; };
const bool ParticleEmitter::DoDraw() const { return doDraw; };
void ParticleEmitter::DoEmit(const bool shouldEmit) { doEmit = shouldEmit; };
const bool ParticleEmitter::DoEmit() const { return doEmit; };

void ParticleEmitter::ApplyForces(const bool shouldApplyForces) { applyForces = shouldApplyForces; };
const bool ParticleEmitter::ApplyForces() const { return applyForces; };
void ParticleEmitter::ClearForces() { forceVectors.clear(); };
void ParticleEmitter::AddForce(const float3 &f) { forceVectors.push_back(f); };
const std::vector<float3>& ParticleEmitter::GetForces() const { return forceVectors; };

void ParticleEmitter::SetShader(const Shader *shader) { emitterShader = (Shader*)shader; };
const Shader* ParticleEmitter::GetShader() const { return emitterShader; };

void ParticleEmitter::SetAlphaMap(const Texture t) { alphaMap = t; };
const Texture& ParticleEmitter::GetAlphaMap() const { return alphaMap; };

void ParticleEmitter::SetSourceAlphaBlendFunction(const GLenum blendfunc) { sourceAlphaBlendFunction = blendfunc; };
const GLenum ParticleEmitter::GetSourceAlphaBlendFunction() const { return sourceAlphaBlendFunction; };
#pragma endregion