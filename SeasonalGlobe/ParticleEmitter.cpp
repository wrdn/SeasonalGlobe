#include "ParticleEmitter.h"

// Constructors / Destructors
ParticleEmitter::ParticleEmitter() : billboardType(Spherical), model(0), localParticleMaximum(GLOBAL_MAX_PARTICLES_PER_EMITTER),
	doUpdate(true), doDraw(true), doEmit(true), rateOfEmission(20), DtOverflow(0), activity(0.2f), isActive(true), applyRotations(true), minLife(1.5f), maxLife(3.5f),
	emitterShader(0), colorMap(0), sourceAlphaBlendFunction(GL_ONE)
{
	glex::Load();

	for(int i=0;i<GLOBAL_MAX_PARTICLES_PER_EMITTER;++i)
	{
		particles[i].color = float4(0,0,0,0);
	}
};

ParticleEmitter::~ParticleEmitter()
{
	// nullify any pointers the emitter was using for safety
	model = 0;
	emitterShader = 0;
};

// Enable textures and shader, and draw all the particles
void ParticleEmitter::Draw()
{
	if(!isActive || !emitterShader || !emitterShader->Valid()) { return; }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GetSourceAlphaBlendFunction());

	glPushMatrix();

	// enable shader
	if(emitterShader)
	{
		emitterShader->Activate();
	}

	// enable textures
	if(alphaMap)
	{
		glActiveTexture(alphaMap->GetTextureSlot());
		alphaMap->Activate();
	}
	if(colorMap)
	{
		glActiveTexture(colorMap->GetTextureSlot());
		colorMap->Activate();
	}

	glTranslatef(emitterOrigin.x(), emitterOrigin.y(), emitterOrigin.z());

	if(billboardType == Spherical) // Spherical Billboarding
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];
			if(p.energy <= 0) { continue; }
			glPushMatrix();
			glColor4fv(p.color.GetVec());
			glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());
			SphericalBillboardAdjust();
			glRotatef(p.rotation_z, 0,0,1);
			glScalef(p.size.x(), p.size.y(), p.size.z());
			model->Draw();
			glPopMatrix();
		}
	}
	else if(billboardType == Cylindrical) // Cylindrical Billboarding
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];
			if(p.energy <= 0) { continue; }
			glPushMatrix();
			glColor4fv(p.color.GetVec());
			glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());
			CylindricalBillboardAdjust();
			glRotatef(p.rotation_z, 0,0,1);
			glScalef(p.size.x(), p.size.y(), p.size.z());
			model->Draw();
			glPopMatrix();
		}
	}
	else // no billboarding (no rotation adjustment)
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];
			if(p.energy <= 0) { continue; }
			glPushMatrix();
			glColor4f(p.color.x(), p.color.y(), p.color.z(), p.color.w());
			glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());
			glRotatef(p.rotation_x, 1,0,0);
			glRotatef(p.rotation_z, 0,0,1);
			glScalef(p.size.x(), p.size.y(), p.size.z());
			model->Draw();
			glPopMatrix();
		}
	}

	glPopMatrix();
	glDisable(GL_BLEND); // disable blending

	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
};

// Update active particle properties, including position, energy, and apply forces.
// Emit new particles as neccessary
void ParticleEmitter::Update(const GameTime &gameTime)
{
	if(!isActive) { return; }

	DtOverflow += gameTime.GetDeltaTime();
	int newParticles = (int)(DtOverflow * rateOfEmission * activity);
	DtOverflow -= newParticles / (rateOfEmission * activity);
	
	for(u32 i=0;i<GetLocalParticleMaximum();++i)
	{
		Particle &p = particles[i];

		p.energy -= gameTime.GetDeltaTime();
		p.pos += p.velocity * gameTime.GetDeltaTime();

		for(std::vector<float3>::const_iterator it=forceVectors.begin(); it != forceVectors.end(); ++it)
			p.pos += (*it) * gameTime.GetDeltaTime();

		p.oldPos = p.pos;

		UpdateParticleProperties(p);

		if(rateOfEmission < 0)
		{
			if(p.energy <= 0)
			{
				Emit(p);
			}
		}
		else
		{
			if(p.energy <= 0 && newParticles-- > 0)
			{
				Emit(p);
			}
		}
	}
};

// Activate shader with alpha map
void ParticleEmitter::ActivateShader()
{
	glActiveTexture(GL_TEXTURE0);
	emitterShader->SetUniform("AlphaMap", alphaMap);
	emitterShader->SetUniform("SphericalBillboarding", billboardType);
	alphaMap->Activate();
};

// Deactivate shader and alpha map
void ParticleEmitter::DeactivateShader()
{
	alphaMap->Deactivate();
	emitterShader->Deactivate();
	glActiveTexture(GL_TEXTURE0);
};

// Matrix billboard adjustments for cylindrical particle
void ParticleEmitter::CylindricalBillboardAdjust() const
{
	f32 mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	mat[0] = mat[10] = 1.0f;
	mat[1] = mat[2] = mat[3] = mat[4] = 
		mat[6] = mat[7] = mat[8] = mat[9] =
		mat[11] = 0.0f;
	glLoadMatrixf(mat);
};

// Matrix billboard adjustments for spherical particle
void ParticleEmitter::SphericalBillboardAdjust() const
{
	f32 mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	mat[0] = mat[10] = mat[5] = 1.0f;
	mat[1] = mat[2] = mat[3] = mat[4] = 
		mat[6] = mat[7] = mat[8] = mat[9] =
		mat[11] = 0.0f;
	glLoadMatrixf(mat);
};

// ACCESSORS AND MUTATORS

void ParticleEmitter::SetBillboardType(const BillboardType btype) { billboardType = btype; };
void ParticleEmitter::SetModel(MeshHandle m) { model = m; };

void ParticleEmitter::SetEmitterOrigin(const float3& f) { emitterOrigin = f; };
const float3& ParticleEmitter::GetEmitterOrigin() const { return emitterOrigin; };

void ParticleEmitter::SetLocalParticleMaximum(const u32 localMax)
{
	localParticleMaximum = min(localMax, GLOBAL_MAX_PARTICLES_PER_EMITTER);
};
const u32 ParticleEmitter::GetLocalParticleMaximum() const { return localParticleMaximum; };

void ParticleEmitter::DoUpdate(const bool shouldUpdate) { doUpdate = shouldUpdate; };
const bool ParticleEmitter::DoUpdate() const { return doUpdate; };
void ParticleEmitter::DoDraw(const bool shouldDraw) { doDraw = shouldDraw; };
const bool ParticleEmitter::DoDraw() const { return doDraw; };
void ParticleEmitter::DoEmit(const bool shouldEmit) { doEmit = shouldEmit; };
const bool ParticleEmitter::DoEmit() const { return doEmit; };

void ParticleEmitter::SetActive(const bool _isActive) { isActive = _isActive; }
const bool ParticleEmitter::IsActive() const { return isActive; }

void ParticleEmitter::SetRateOfEmission(const i32 rate) { rateOfEmission = rate; };
const i32 ParticleEmitter::GetRateOfEmission() const { return rateOfEmission; };

void ParticleEmitter::SetMaxParticleLife(const f32 maxLifeInSeconds) { maxLife = maxLifeInSeconds; };
const f32 ParticleEmitter::GetMaxParticleLife() const { return maxLife; };
void ParticleEmitter::SetMinParticleLife(const f32 minLifeInSeconds) { minLife = minLifeInSeconds; };
const f32 ParticleEmitter::GetMinParticleLife() const { return minLife; };

void ParticleEmitter::ClearForces() { forceVectors.clear(); };
void ParticleEmitter::AddForce(const float3 &f) { forceVectors.push_back(f); };
const std::vector<float3>& ParticleEmitter::GetForces() const { return forceVectors; };

void ParticleEmitter::SetShader(const ShaderHandle shader) { emitterShader = shader; };
ShaderHandle ParticleEmitter::GetShader() const { return emitterShader; };

void ParticleEmitter::SetAlphaMap(TextureHandle t)
{
	alphaMap = t;
	alphaMap->SetTextureSlot(SLOT_GL_TEXTURE_0);
};
TextureHandle ParticleEmitter::GetAlphaMap() const { return alphaMap; };

void ParticleEmitter::SetSourceAlphaBlendFunction(const GLenum blendfunc) { sourceAlphaBlendFunction = blendfunc; };
const GLenum ParticleEmitter::GetSourceAlphaBlendFunction() const { return sourceAlphaBlendFunction; };

void ParticleEmitter::ResetParticleEmitter()
{
	for(int i=0;i<GLOBAL_MAX_PARTICLES_PER_EMITTER;++i)
	{
		particles[i].color = float4(0,0,0,0);
		particles[i].energy = -1;
	}
};