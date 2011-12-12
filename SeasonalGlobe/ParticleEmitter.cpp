#include "ParticleEmitter.h"

// Constructors / Destructors
ParticleEmitter::ParticleEmitter() : billboardType(Spherical), model(0), localParticleMaximum(GLOBAL_MAX_PARTICLES_PER_EMITTER),
	doUpdate(true), doDraw(true), doEmit(true), rateOfEmission(20), applyRotations(true), minLife(1.5f), maxLife(3.5f),
	emitterShader(0), sourceAlphaBlendFunction(GL_ONE)
{
	glex::Load();
};

ParticleEmitter::~ParticleEmitter()
{
	// nullify any pointers the emitter was using for safety
	model = 0;
	emitterShader = 0;
};

void ParticleEmitter::Draw(/*const GameTime &gameTime*/)
{
	if(!emitterShader || !emitterShader->Valid()) return;

	glPushMatrix();

	ActivateShader();
	
	glTranslatef(emitterOrigin.x(), emitterOrigin.y(), emitterOrigin.z());

	if(billboardType == Spherical) // only need 1 check, so duplicate most of the drawing code
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];
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
	else
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];
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
	
	/*for(u32 i=0;i<GetLocalParticleMaximum();++i)
	{
		Particle &p = particles[i];

		glPushMatrix();

		glColor4fv(p.color.GetVec());
		
		glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());

		if(billboardType == Spherical)
		{
			SphericalBillboardAdjust();
		}
		else
		{
			CylindricalBillboardAdjust();
		}

		glRotatef(p.rotation_z, 0,0,1);
		glScalef(p.size.x(), p.size.y(), p.size.z());
		model->Draw();

		glPopMatrix();
	}*/

	DeactivateShader();

	glPopMatrix();

	glColor4f(1,1,1,1);
};

void ParticleEmitter::Update(const GameTime &gameTime)
{
	f32 dt = gameTime.GetDeltaTime();
	u32 emittedThisFrame = rateOfEmission;
	
	for(u32 i=0;i<GetLocalParticleMaximum();++i)
	{
		Particle &p = particles[i];

		p.energy -= dt;
		p.pos += p.velocity * dt;

		// If you don't want to apply forces, just call ClearForces() to remove all of them
		for(std::vector<float3>::const_iterator it=forceVectors.begin(); it != forceVectors.end(); ++it)
				p.pos += (*it) * dt;

		// Called before check for death. Feel free to use this function to kill a particle (by setting energy to < 0)
		UpdateParticleProperties(p/*, gameTime*/);

		if(p.energy <= 0/* && emittedThisFrame > 0*/ )
		{
			Emit(p);
			--emittedThisFrame;
			continue;
		}
	}
};

void ParticleEmitter::ActivateShader(/*const GameTime &gameTime*/)
{
	emitterShader->Activate();
	glActiveTexture(GL_TEXTURE0);
	emitterShader->SetUniform("AlphaMap", alphaMap);
	emitterShader->SetUniform("SphericalBillboarding", billboardType);
	alphaMap.Activate();
};

void ParticleEmitter::DeactivateShader()
{
	alphaMap.Deactivate();
	emitterShader->Deactivate();
	glActiveTexture(GL_TEXTURE0);
};

// Matrix billboard adjustments (These should be done in the shader)
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

#pragma region Accessors and Mutators
void ParticleEmitter::SetBillboardType(const BillboardType btype) { billboardType = btype; };
void ParticleEmitter::SetModel(const Model *m) { model = (Model*)m; };

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

void ParticleEmitter::SetRateOfEmission(const u32 rate) { rateOfEmission = rate; };
const u32 ParticleEmitter::GetRateOfEmission() const { return rateOfEmission; };

void ParticleEmitter::SetMaxParticleLife(const f32 maxLifeInSeconds) { maxLife = maxLifeInSeconds; };
const f32 ParticleEmitter::GetMaxParticleLife() const { return maxLife; };
void ParticleEmitter::SetMinParticleLife(const f32 minLifeInSeconds) { minLife = minLifeInSeconds; };
const f32 ParticleEmitter::GetMinParticleLife() const { return minLife; };

void ParticleEmitter::ClearForces() { forceVectors.clear(); };
void ParticleEmitter::AddForce(const float3 &f) { forceVectors.push_back(f); };
const std::vector<float3>& ParticleEmitter::GetForces() const { return forceVectors; };

void ParticleEmitter::SetShader(const Shader *shader) { emitterShader = (Shader*)shader; };
const Shader* ParticleEmitter::GetShader() const { return emitterShader; };

void ParticleEmitter::SetAlphaMap(const Texture &t)
{
	alphaMap = t;
	alphaMap.SetTextureSlot(SLOT_GL_TEXTURE_0);
};
const Texture& ParticleEmitter::GetAlphaMap() const { return alphaMap; };

void ParticleEmitter::SetSourceAlphaBlendFunction(const GLenum blendfunc) { sourceAlphaBlendFunction = blendfunc; };
const GLenum ParticleEmitter::GetSourceAlphaBlendFunction() const { return sourceAlphaBlendFunction; };
#pragma endregion