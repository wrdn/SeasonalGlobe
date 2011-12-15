#include "Particle.h"

Particle::Particle() : pos(), velocity(), size(1), color(Color4f(0,0,0,0)), energy(0), rotation_z(0),
	rotation_x(0), pada(-1)
{
};

Particle::~Particle()
{
};