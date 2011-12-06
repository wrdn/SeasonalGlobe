#include "GradientParticleEmitter.h"

GradientParticleEmitter::GradientParticleEmitter() : particleSpread(5)
{
	glex::Load();
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

void GradientParticleEmitter::UpdateShader(const GameTime &gameTime)
{
	glEnable(GL_TEXTURE_2D);

	Shader *shader = (Shader*)GetShader();

	Texture &shaderAlphaMap = (Texture&)GetAlphaMap();
	glActiveTexture(shaderAlphaMap.GetTextureSlot());
	shaderAlphaMap.Activate();

	glActiveTexture(gradientMap.GetTextureSlot());
	gradientMap.Activate();

	shader->SetUniform("AlphaMap", 0);
	shader->SetUniform("GradientMap", 1);
	shader->SetUniform("Time", gameTime.GetRunningTime());
};