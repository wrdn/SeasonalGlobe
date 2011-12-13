#include "HemiSphericalParticleEmitter.h"
#include "SphericalCamera.h"

HemiSphericalParticleEmitter::HemiSphericalParticleEmitter() : hemisphere_radius(2), minAngle(0), maxAngle(0),
	startColor(1,1,1,1), endColor(1,1,1,0)
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

	p.color = startColor;
};

void HemiSphericalParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	if(p.pos.y() > EPSILON)
	{
		p.energy = randflt(3,5);
		p.pada = p.energy;
	}
	else
	{
		p.velocity = float3();

		f32 r = lerp(startColor.r(), endColor.r(), 1-(1.0f/p.pada)*p.energy);
		f32 g = lerp(startColor.g(), endColor.g(), 1-(1.0f/p.pada)*p.energy);
		f32 b = lerp(startColor.b(), endColor.b(), 1-(1.0f/p.pada)*p.energy);
		f32 a = lerp(startColor.a(), endColor.a(), 1-(1.0f/p.pada)*p.energy);

		p.color = float4(r,g,b,a);
	}
};