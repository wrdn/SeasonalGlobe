#include "HemiSphericalParticleEmitter.h"
#include "SphericalCamera.h"

HemiSphericalParticleEmitter::HemiSphericalParticleEmitter() : hemisphere_radius(2), minAngle(0), maxAngle(0)
{
};

HemiSphericalParticleEmitter::~HemiSphericalParticleEmitter()
{
};

void HemiSphericalParticleEmitter::Emit(Particle &p)
{
	f32 ZAxis = randflt(-hemisphere_radius, hemisphere_radius);
	f32 phi = randflt(0,PI);
	f32 theta = asin(ZAxis/hemisphere_radius);
	
	f32 xpos = hemisphere_radius * cos(theta) * cos(phi);
	f32 ypos = hemisphere_radius * cos(theta) * sin(phi);
	f32 zpos = hemisphere_radius * sin(theta);

	p.pos = float3(xpos,ypos,zpos);

	p.velocity.x( randflt(-0.4f,0.4f));
	p.velocity.y(-1.0f);
	p.velocity.z( randflt(-0.4f, 0.4f));
	
	p.velocity.normalize();

	p.size.setall( randflt(0.25f, 0.35f) );
	//p.size.set(0.25f, 0.25f, 0.25f);
};

void HemiSphericalParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	if(p.pos.y() > EPSILON)
		p.energy = 100;
	else
		p.energy = -1;
};