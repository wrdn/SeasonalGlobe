#pragma once

#include "KParticleEmitter.h"
#include "Shader.h"

struct KPointEmitter
{
public:
	f32 spread;
};

float randflt(float max, float min)
{
    float random = ((float) rand()) / (float) RAND_MAX;

    // generate (in your case) a float between 0 and (4.5-.78)
    // then add .78, giving you a float between .78 and 4.5
    float range = max - min;  
    return (random*range) + min;
};

class KPointParticleEmitter : public KParticleEmitter
{
private:
	KPointEmitter pointEmitterInfo;
	Color4f startingColour, endingColour;

	Texture *gradientMap;
	Shader *gradientMapShader;

	void Emit(KParticle &p, void *data)
	{
		//startingColour = Color4f(0.8,0.6,0,1);
		//endingColour = Color4f(0.8,0.2,0,0.3);
		startingColour = Color4f(0.1f, 0.1f, 0.1f, 1.0f);
		endingColour = Color4f(0.1f, 0.1f, 0.1f, 0.6f);

		p.pos = GetEmitterOrigin();
		p.color = startingColour;
		
		//p.energy = 2.0; //( ((float)rand() / RAND_MAX) * 0.5f);
		p.energy = randflt(1.5f, 3.5f);
		p.originalEnergy = p.energy;

		//p.velocity.x(pointEmitterInfo.spread * 0.235f * (float)(rand() / RAND_MAX));
		//p.velocity.y( 1.5f );
		//p.velocity.z(pointEmitterInfo.spread * 0.235f * (float)(rand() / RAND_MAX));
		
		//p.velocity.x(((float)rand()/RAND_MAX) * 2 - 1 );
		//p.velocity.y( (float)rand()/RAND_MAX );
		//p.velocity.z( ((float)rand()/RAND_MAX) * 2 - 1 );
		//p.velocity.x(((float)rand()/RAND_MAX) * pointEmitterInfo.spread - 1);

		p.velocity.x( randflt(-pointEmitterInfo.spread, pointEmitterInfo.spread));
		p.velocity.y(1.0f);
		p.velocity.z( randflt(-pointEmitterInfo.spread, pointEmitterInfo.spread));

		p.velocity.normalize();
	};

	void Update(const f32 dt)
	{
		u32 emittedThisFrame = GetRateOfEmission();
		for(u32 i=0;i<GetMaxEmitterParticles();++i)
		{
			KParticle &p = GetParticles()[i];

			p.pos += p.velocity * dt; //(p.velocity * gravityVector) + (p.velocity * windDirection);
			p.energy -= dt; //1.0f;

			/*f32 d = 1.0 / (p.energy);
			f32 _r = lerp(startingColour.r(), endingColour.r(), d);
			f32 _g = lerp(startingColour.g(), endingColour.g(), d);
			f32 _b = lerp(startingColour.b(), endingColour.b(), d);
			f32 _a = lerp(startingColour.a(), endingColour.a(), d);
			p.color = Color4f(_r, _g, _b, _a);*/

			//f32 d = 1.0f / p.energy; // lerp input

			if(p.energy < 0) // particle dead
			{
				if(emittedThisFrame > 0)
				{
					Emit(p, (void*)&dt);
					--emittedThisFrame;
					continue;
				}
			}
		}
	};

	virtual	void InitParticles()
	{
		for(u32 i=0;i<GetMaxEmitterParticles();++i)
		{
			Emit(GetParticles()[i], 0);
		}
	};

public:
	void SetParticleSpread(const f32 spread) { pointEmitterInfo.spread = spread; };
	const f32 GetParticleSpread() const { return pointEmitterInfo.spread; };

	void SetGradientMap(Texture *t) { gradientMap = t; };

	void SetGradientMapShader(Shader *s) { gradientMapShader = s; };
};