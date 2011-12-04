#include "Particle.h"

Particle::Particle() : pos(), velocity(), size(1), color(Color::WHITE),
	energy(0), originalEnergy(0)
{
};

Particle::~Particle()
{
};