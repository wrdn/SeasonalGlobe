#include "HemiSphericalParticleEmitter.h"
#include "SphericalCamera.h"

HemiSphericalParticleEmitter::HemiSphericalParticleEmitter() : hemisphere_radius(2),
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
		float4 C(-3.3, 3.7, 0.1);
		float4 B(-3.3, 1.65, 1.65);
		f32 xDistance = abs(C.z() - B.z());
		f32 yDistance = abs(C.y() - B.y());
		f32 T = abs(p.pos.z() - C.z());
		f32 Q = C.y() - ((T / xDistance) * yDistance);
		if(p.pos.y() < abs(Q))
		{
			p.velocity = float3();
			p.pos.y( Q + 0.1 );
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
		p.velocity = float3();

		f32 r = lerp(startColor.r(), endColor.r(), 1-(1.0f/p.pada)*p.energy);
		f32 g = lerp(startColor.g(), endColor.g(), 1-(1.0f/p.pada)*p.energy);
		f32 b = lerp(startColor.b(), endColor.b(), 1-(1.0f/p.pada)*p.energy);
		f32 a = lerp(startColor.a(), endColor.a(), 1-(1.0f/p.pada)*p.energy);

		p.color = float4(r,g,b,a);
	}

	if(p.rotation_x > 1)
	{
		f32 r = lerp(startColor.r(), endColor.r(), 1-(1.0f/p.pada)*p.energy);
		f32 g = lerp(startColor.g(), endColor.g(), 1-(1.0f/p.pada)*p.energy);
		f32 b = lerp(startColor.b(), endColor.b(), 1-(1.0f/p.pada)*p.energy);
		f32 a = lerp(startColor.a(), endColor.a(), 1-(1.0f/p.pada)*p.energy);
		p.color = float4(r,g,b,a);
	}
};