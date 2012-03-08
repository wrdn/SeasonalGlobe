#include "CylindricalParticleEmitter.h"
#include "util.h"

CylindricalParticleEmitter::CylindricalParticleEmitter()
	: startPos(), endPos(), direction(), startColor(1,0,0,1), endColor(1, 0.28f, 0, 0.8f)
{
};

CylindricalParticleEmitter::~CylindricalParticleEmitter()
{
};

void CylindricalParticleEmitter::Emit(Particle &p)
{
	// Emit particles along the line startpos + (t * direction), for 0<=t<=1
	
	f32 t = randflt(0.0f , 1.0f);

	p.pos = this->startPos + (t * this->direction);

	p.velocity = float3(0.2f, randflt(0, 1), 0.2f);
	p.velocity.normalize();

	p.size.setall(0.5f);

	p.color = Color4f(1,0,0,1);

	p.energy = randflt(0, 1.0);

	/*f32 Angle = DEGTORAD( randflt(0, 360) );
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

	p.rotation.setall(-90);*/
};

void CylindricalParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	p.color = startColor.vec_lerp(endColor, 1.0f / p.energy);
};