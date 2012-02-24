#include "HemiSphericalParticleEmitter.h"

HemiSphericalParticleEmitter::HemiSphericalParticleEmitter() : hemisphere_radius(2),
	startColor(1,1,1,1), endColor(1,1,1,0), timeToFall(3)
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
	p.oldPos = p.pos;
	p.rotation_x = 0;

	p.velocity.x( randflt(-0.4f,0.4f));
	p.velocity.y( -(hemisphere_radius/timeToFall) );
	p.velocity.z( randflt(-0.4f, 0.4f));
	
	p.size.setall( randflt(0.35f, 0.55f) );

	p.color = startColor;
};

void HemiSphericalParticleEmitter::UpdateParticleProperties(Particle &p)
{
	// Particle in the points defined by the roof
	if( (( p.pos.x() >= -7.35 && p.pos.x() <= -3.2 )
		&& (p.pos.z() >= -1.35 && p.pos.z() <= 1.6) 
		&& (p.pos.y() >= 1.9 && p.pos.y() <= 3.4) ))
	{
		float4 C(-3.3f, 3.7f, 0.1f);
		float4 B(-3.3f, 1.65f, 1.65f);
		f32 xDistance = abs(C.z() - B.z());
		f32 yDistance = abs(C.y() - B.y());
		f32 T = abs(p.pos.z() - C.z());
		f32 Q = C.y() - ((T / xDistance) * yDistance);
		if(p.pos.y() < abs(Q))
		{
			p.velocity = float3();
			p.pos.y( Q + 0.16f );
			p.rotation_x = 5;
		}
	}

	if(p.pos.y() > EPSILON && p.rotation_x < 1)
	{
		p.energy = randflt(3,4);
		p.pada = p.energy;
		p.rotation_x = 0;
	}
	else if(p.pos.y() < EPSILON)
	{
		p.velocity.zero();
		p.color = startColor.vec_lerp(endColor, 1-(1.0f/p.pada)*p.energy);
	}

	if(p.rotation_x > 1)
	{
		p.color = startColor.vec_lerp(endColor, 1-(1.0f/p.pada)*p.energy);
	}
};