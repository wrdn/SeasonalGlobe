#include "GradientParticleEmitter.h"

GradientParticleEmitter::GradientParticleEmitter() : particleSpread(5)
{
};

GradientParticleEmitter::~GradientParticleEmitter()
{
};

void GradientParticleEmitter::Emit(Particle &p, void *gdata)
{
	p.pos = GetEmitterOrigin();
	p.color = Color::GREY;

	p.energy = randflt(1.5f, 3.5f);
	p.originalEnergy = p.energy;

	p.velocity.x( randflt(-particleSpread, particleSpread));
	p.velocity.y(1.0f);
	p.velocity.z( randflt(-particleSpread, particleSpread));

	p.velocity.normalize();
};

float K=0.0f;
void GradientParticleEmitter::UpdateShader(const f32 dt)
{
	//gradientMap.Activate();
	//GetAlphaMap().Activate();

	Shader* gradientShader = (Shader*)GetShader();
	//std::cout << gradientShader->GetUnformLocation("t") << ": " << dt << std::endl;

	gradientShader->Activate();

	K += dt;

	gradientShader->SetUniform("t", fract(K));
	//gradientShader->SetUniform("GradientMap", (const GLint)gradientMap.GetTextureSlotIndex());
	gradientShader->SetUniform("AlphaMap", (const GLint)GetAlphaMap().GetTextureSlotIndex());

	gradientShader->Deactivate();
};