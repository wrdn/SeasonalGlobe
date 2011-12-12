#include "Particle.h"

Particle::Particle() : pos(), velocity(), size(1), color(Color::WHITE), energy(0), rotation_z(0),
	pada(-1), padb(-1)
{
};

Particle::~Particle()
{
};