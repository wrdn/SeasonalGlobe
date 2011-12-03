#pragma once

#include "float3.h"
#include "Texture.h"
#include "Color.h"
#include <vector>
#include "Model.h"

const f32 GRAVITY = -9.81f; //m/s^2

// particles can be batched by the texture they use
// therefore the texture need not be stored with each particle
struct Particle
{
public:
	bool alive;
	float3 pos; // 12
	float3 velocity; // 12
	float3 scale; // 12
	f32 age, timeToLive; // 8
	ColorU32 color; // 4

	Particle() : alive(false)
	{
	};

	void SetDefaultValues()
	{
		//timeToLive = 1.0f;

		//timeToLive = (float)rand() % 5;
		timeToLive = rand() % 1500;
		//timeToLive = rand()%3 * 0.3f;

		//color.SetColor(Color::FromInt(Color::ToInt(Color::RED)));
		color.SetColor(Color::ToInt(Color::WHITE));
		alive=true;

		float n = (float)rand() / RAND_MAX;
		float x = n * 2 - 1;

		n = (float)rand()/RAND_MAX;
		float y = n;

		n = (float)rand()/RAND_MAX;
		float z = n * 2 - 1;

		velocity = float3(x,y,z);
		velocity.normalize();
		velocity *= 3;

		pos = float3();
	};
};

// represents a "set" of particles e.g. snow or smoke particles
class ParticleEmitter
{
private:
	Model BillboardModel; // square

	static const u32 MAX_PARTICLES = 50;
	Particle particles[ParticleEmitter::MAX_PARTICLES];

	bool isActive;
	float3 emitterOrigin;
	Texture tex; // texture to use for particles in emitter

public:
	ParticleEmitter();
	~ParticleEmitter();

	Particle * const GetParticles() { return particles; };

	const Texture& GetTexture() const { return tex; };
	void SetTexture(const Texture t) { tex = t; };

	const bool IsActive() const { return isActive; };
	void SetActive(bool _isActive) { isActive = _isActive; };

	const float3& GetEmitterOrigin() const { return emitterOrigin; };
	void SetEmitterOrigin(float3 &f) { emitterOrigin = f; };

	// Each emitter (typically very few will exist) can update and draw itself
	void Update(const f32 dt);
	void Draw(f32 dt);

	// Only call this once to create the quad we will be drawing
	void Init()
	{
		VERTEX *billboardVerts = new VERTEX[4];
		billboardVerts[0] = VERTEX( float3(-0.5f,1,0), float3(0,0,1), float2(0,1) );//top left
		billboardVerts[1] = VERTEX( float3(0.5f,1,0), float3(0,0,1), float2(1,1) );//top right
		billboardVerts[2] = VERTEX( float3(-0.5f,0,0), float3(0,0,1), float2(0,0) );//bottom left
		billboardVerts[3] = VERTEX( float3(0.5f,0,0), float3(0,0,1), float2(1,0) );//bottom right
		BillboardModel.SetVertexArray(billboardVerts, 4);

		u32 *indices = new u32[6];
		indices[0] = 0; indices[1] = 2;
		indices[2] = 3; indices[3] = 0;
		indices[4] = 3; indices[5] = 1;

		BillboardModel.SetIndicesArray(indices, 6);
		BillboardModel.BuildVBO();
	};

	void InitParticles();
};