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

	// random velocity between random float between 0 and 1 along y
	p.velocity = float3(0.2f, randflt(0, 1), 0.2f);
	p.velocity.normalize();

	p.size.setall(0.5f);

	p.color = Color4f(1,0,0,1);

	p.energy = randflt(0, 1.0);
};

void CylindricalParticleEmitter::UpdateParticleProperties(Particle &p)
{
	// lerp between start and end color
	p.color = startColor.vec_lerp(endColor, 1.0f / p.energy);
};