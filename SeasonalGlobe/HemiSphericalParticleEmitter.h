#include "ParticleEmitter.h"

class HemiSphericalParticleEmitter : public ParticleEmitter
{
private:
	f32 hemisphere_radius;

	// min and max angles on the hemisphere where particles will be generated
	f32 minAngle, maxAngle;

	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p, const GameTime &gameTime);

public:

	HemiSphericalParticleEmitter();
	~HemiSphericalParticleEmitter();

	void SetMinAngle(const f32 minangle) { minAngle = minangle; };
	void SetMaxAngle(const f32 maxangle) { maxAngle = maxangle; };
	const f32 GetMinAngle() const { return minAngle; };
	const f32 GetMaxAngle() const { return maxAngle; };

	void SetHemiSphereRadius(const f32 radius) { hemisphere_radius = radius; };
	const f32 GetHemiSphereRadius() const { return hemisphere_radius; };
};