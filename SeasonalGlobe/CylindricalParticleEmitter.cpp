#include "CylindricalParticleEmitter.h"

CylindricalParticleEmitter::CylindricalParticleEmitter()
{
};

CylindricalParticleEmitter::~CylindricalParticleEmitter()
{
};

void CylindricalParticleEmitter::Emit(Particle &p)
{
	f32 Angle = DEGTORAD( randflt(0, 360) );
	f32 radius = 0.08f;
	f32 height = 2.0f;

	p.pos.set(
		cos(Angle) * radius,
		sin(Angle) * radius,
		randflt(0, height));

	p.velocity = float3(0.2f, randflt(0, 1), 0.2f);
	p.velocity.normalize();

	//p.velocity = float3(randflt(-0.4, 0.4), randflt(0,1), randflt(-0.4, 0.4));
	//p.velocity.normalize();

	p.size.setall(0.5f);
	p.color = Color4f(1,0,0,1);
	p.energy = randflt(0, 1.0);

	p.rotation.setall(-90);
};

void CylindricalParticleEmitter::UpdateParticleProperties(Particle &p, const GameTime &gameTime)
{
	float4 startColor(1,0,0,1);
	float4 endColor(1,0.28f,0,0.8f);

	f32 lR = lerp(startColor.r(), endColor.r(), 1.0f / p.energy);
	f32 lG = lerp(startColor.g(), endColor.g(), 1.0f / p.energy);
	f32 lB = lerp(startColor.b(), endColor.b(), 1.0f / p.energy);
	f32 lA = lerp(startColor.a(), endColor.a(), 1.0f / p.energy);

	p.color = Color4f(lR, lG, lB, lA);
};