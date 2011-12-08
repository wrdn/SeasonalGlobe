#include "Particle.h"

Particle::Particle() : pos(), velocity(), size(1), rotation_z(0), color(Color::WHITE),
	energy(0), pada(-1), padb(-1)
{
};

Particle::~Particle()
{
};