#include "ParticleEmitter.h"

class HemisphericalParticleEmitter : public ParticleEmitter
{
private:
	f32 hemisphere_radius;

	// min and max angles on the hemisphere where particles will be generated
	f32 minAngle, maxAngle;

	void Emit(Particle &p, void *data);

public:

	HemisphericalParticleEmitter();
	~HemisphericalParticleEmitter();
};