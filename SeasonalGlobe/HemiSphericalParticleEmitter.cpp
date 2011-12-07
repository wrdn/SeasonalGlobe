#include "HemiSphericalParticleEmitter.h"
#include "SphericalCamera.h"

HemiSphericalParticleEmitter::HemiSphericalParticleEmitter() : hemisphere_radius(2)
{
};

HemiSphericalParticleEmitter::~HemiSphericalParticleEmitter()
{
};

void HemiSphericalParticleEmitter::Emit(Particle &p)
{
	/*SphericalCamera scam;
	scam.spherical_coords[0] = 11;
	scam.spherical_coords[1] = randflt(2*PI, 0);
	scam.spherical_coords[1] = randflt(2*PI, 0);

	f32 _pos[3];
	scam.SphericalToCartesian(_pos);
	f32 xpos = _pos[0], ypos = _pos[1], zpos = _pos[2];

	p.pos = float3(xpos,ypos,zpos);*/

	f32 ZAxis = randflt(hemisphere_radius, -hemisphere_radius);
	f32 phi = randflt(PI,0);
	f32 theta = asin(ZAxis/hemisphere_radius);
	
	f32 xpos = hemisphere_radius * cos(theta) * cos(phi);
	f32 ypos = hemisphere_radius * cos(theta) * sin(phi);
	f32 zpos = hemisphere_radius * sin(theta);

	p.pos = float3(xpos,ypos,zpos);

	//p.energy = randflt(1.5f, 3.5f);
	//p.originalEnergy = p.energy;
	
	p.velocity.x( randflt(0.4f,-0.4f));
	p.velocity.y(-1.0f);
	p.velocity.z( randflt(0.4f, -0.4f));
	
	p.velocity.normalize();

	p.size.set(0.25f, 0.25f, 0.25f);
};

void HemiSphericalParticleEmitter::UpdateParticleProperties(Particle &p, const GameTime &gameTime)
{
	if(p.pos.y() > EPSILON)
		p.energy = 100;
	else
		p.energy = -1;
};