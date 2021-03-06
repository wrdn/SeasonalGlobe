#pragma once
#include "ParticleEmitter.h"

// State of particle - specifically used for leaves
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
// This class is also used for leaves, and thus contains related states, including "particlesFalling"
class StaticParticleEmitter : public ParticleEmitter
{
private:
	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p);

	bool particlesStatic; // are the particles actually static?

	// maintains the index of the last particle we added, must be in range 0...local particle maximum
	// AddParticle(...) will fail if currentParticleIndex equals local particle maximum
	u32 currentParticleIndex;

	bool updateColor; // leaves change from green to brown in autumn
	Color4f startColor, endColor, fadeOutColor, fadeInColor;


	bool particlesFalling; // leaves fall to ground
	bool particlesDieing; // particles fading out
	bool particlesFadingIn; // are the leaves fading in?

	f32 timeToChangeColor; // time for change from green leaf to brown leaf (default: 5)
	f32 timeToFadeOut, timeToFadeIn;
	
	f32 timeToFall; // how long the leaves have to fall
	f32 maxYHeight;

public:
	StaticParticleEmitter();
	~StaticParticleEmitter();

	// Adds the particle at currentParticleIndex. Does NOT change any particle properties
	const bool AddParticle(const Particle &p);

	Particle* GetStaticParticles() const { return GetParticles(); }

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

	void SetParticlesDieing(bool b) { particlesDieing = b; }
	void SetParticlesFalling(bool b) { particlesFalling = b; }
	void SetParticlesFadingIn(bool b) { particlesFadingIn = b; }

	const f32 GetTimeToChangeColor() const { return timeToChangeColor; }
	const f32 GetTimeToFadeOut() const { return timeToFadeOut; }
	const f32 GetTimeToFadeIn() const { return timeToFadeIn; }
	const f32 GetTimeToFall() const { return timeToFall; }

	void SetTimeToChangeColor(f32 t) { timeToChangeColor = t; }
	void SetTimeToFadeOut(f32 t) { timeToFadeOut = t; }
	void SetTimeToFadeIn(f32 t) { timeToFadeIn = t; }
	void SetTimeToFall(f32 t) { timeToFall = t; }

	// Useful functions to change the state of the leaf particles
	void InitiateParticleFall();

	void InitiateParticleFadeIn(); // fades from (startColor.rgb,0) to startColor
	void InitiateParticleFadeOut(); // fade particles from (endColor.rgba) to (endColor.rgb, 0)

	void InitiateMainColorChange(); // change from startColor to endColor
};