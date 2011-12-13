#pragma once
#include "ParticleEmitter.h"

enum ParticleState
{
	Static,
	ChangingColor,
	ParticleFalling,
	ParticleDieing,
};

// In this emitter, the emit function does nothing but set the velocity to (0,0,0)
// UpdateParticleProperties will reset the particles energy each frame, ensuring they
// will never die. This may be used for particles representing objects that will not die (e.g. leaves).
// We could later add functionality to finally make the leaves fall (e.g. by setting a "static_emitter" flag
// The user of the class must add each particle by hand, using the AddParticle(...) function
class StaticParticleEmitter : public ParticleEmitter
{
private:
	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/);

	bool particlesStatic; // are the particles actually static?

	// maintains the index of the last particle we added, must be in range 0...local particle maximum
	// AddParticle(...) will fail if currentParticleIndex equals local particle maximum
	u32 currentParticleIndex;

	bool updateColor; // leaves change from green to brown in autumn
	Color4f startColor, endColor, fadeOutColor, fadeInColor;


	bool particlesFalling; // leaves fall to ground
	bool particlesDieing; // particles fading out
	bool particlesFadingIn;

	f32 timeToChangeColor; // time for change from green leaf to brown leaf (default: 5)
	f32 timeToFadeOut, timeToFadeIn; // time 
	
	f32 timeToFall;
	f32 maxYHeight;

public:
	StaticParticleEmitter();
	~StaticParticleEmitter();

	// Adds the particle at currentParticleIndex. Does NOT change any particle properties
	const bool AddParticle(const Particle &p);

	// Accessors and Mutators
	void SetParticlesStaticState(const bool particlesAreStatic);
	const bool GetParticlesStaticState() const;

	const bool UpdatingColor() const { return updateColor; }
	const Color4f& GetStartColor() const { return startColor; }
	const Color4f& GetEndColor() const { return endColor; }
	
	void DoColorUpdate(const bool doUpdateColor) { updateColor = doUpdateColor; }
	void SetStartColor(const Color4f &c)
	{
		startColor = c;

		fadeInColor = startColor;
		fadeInColor.w(0);
	}
	void SetEndColor(const Color4f &c)
	{
		endColor = c;
		
		fadeOutColor = endColor;
		fadeOutColor.w(0);
	}

	void InitiateParticleFall();

	void InitiateParticleFadeIn(); // fades from (startColor.rgb,0) to startColor
	void InitiateParticleFadeOut(); // fade particles from (endColor.rgba) to (endColor.rgb, 0)

	void InitiateMainColorChange(); // change from startColor to endColor
};