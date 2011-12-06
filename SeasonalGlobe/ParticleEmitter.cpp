#include "ParticleEmitter.h"

// Constructors / Destructors
ParticleEmitter::ParticleEmitter() : model(0), emitterShader(0), applyForces(false),
	sourceAlphaBlendFunction(GL_ONE), rateOfEmission(20), billboardType(Spherical),
	localParticleMaximum(GLOBAL_MAX_PARTICLES_PER_EMITTER), doUpdate(true), doDraw(true), doEmit(true)
{
	glex::Load();
};

ParticleEmitter::~ParticleEmitter()
{
	// nullify any pointers the emitter was using for safety
	model = 0;
	emitterShader = 0;
};

// Draw and Update
void ParticleEmitter::Draw(const GameTime &gameTime)
{
	if(emitterShader) // a shader should be included per emitter, but it is not required
	{
		emitterShader->Activate();

		if(emitterShader)
			UpdateShader(gameTime); // if you wish to use them, you are expected to override and activate textures etc in UpdateShader()

		for(u32 i=0; i<GetLocalParticleMaximum(); ++i)
		{
			Particle &p = GetParticles()[i];

			glPushMatrix();
			
			glScalef(p.size.x(), p.size.y(), p.size.z());
			glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());
			model->Draw();

			glPopMatrix();
		}
		emitterShader->Deactivate();
	}
	else //!emitterShader, loop, use glColor4f and draw
	{
		alphaMap.Activate();

		GLfloat currentColor[4];
		glGetFloatv(GL_CURRENT_COLOR, currentColor);

		for(u32 i=0; i<GetLocalParticleMaximum(); ++i)
		{
			Particle &p = GetParticles()[i];

			glPushMatrix();
			glColor4fv(p.color.GetVec());
			glScalef(p.size.x(), p.size.y(), p.size.z());
			glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());

			if(billboardType == Spherical) { SphericalBillboardAdjust(); }
			else { CylindricalBillboardAdjust(); }
			
			model->Draw();
			glPopMatrix();
		}

		glColor4fv(currentColor); // restore colour
	}
	alphaMap.Deactivate();
	
	glActiveTexture(GL_TEXTURE0);
};

void ParticleEmitter::Update(const GameTime &gameTime)
{
	u32 emittedThisFrame = GetRateOfEmission();
	f32 dt = gameTime.GetDeltaTime();

	if(applyForces)
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];

			p.pos += p.velocity * dt;

			// Apply forces
			for(u32 i=0;i<forceVectors.size();++i)
			{
				//p.pos += (p.velocity * forceVectors[i] * dt);
				p.pos += forceVectors[i] * dt;
			}

			p.energy -= gameTime.GetDeltaTime();

			// Check particle life
			if(p.energy < 0 && emittedThisFrame > 0 )
			{
				Emit(p, (void*)&dt);
				--emittedThisFrame;
				continue;
			}
		}
	}
	else // not applying forces
	{
		for(u32 i=0;i<GetLocalParticleMaximum();++i)
		{
			Particle &p = particles[i];

			// Apply velocity (No other forces)
			p.pos += p.velocity * dt;
			p.energy -= dt;

			// Check particle life
			if(p.energy < 0 && emittedThisFrame>0 ) // particle dead
			{
				Emit(p, (void*)&dt);
				--emittedThisFrame;
				continue;
			}
		}
	}
};

// Matrix billboard adjustments
void ParticleEmitter::CylindricalBillboardAdjust()
{
	f32 mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	mat[0] = mat[10] = mat[5] = 1.0f;
	mat[1] = mat[2] = mat[3] = mat[4] = 
		mat[6] = mat[7] = mat[8] = mat[9] =
		mat[11] = 0.0f;
	glLoadMatrixf(mat);
};

void ParticleEmitter::SphericalBillboardAdjust()
{
	f32 mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	mat[0] = mat[10] = 1.0f;
	mat[1] = mat[2] = mat[8] = mat[9] = 0.0f;
	glLoadMatrixf(mat);
};


#pragma region Accessors and Mutators
void ParticleEmitter::SetBillboardType(const BillboardType btype) { billboardType = btype; };
void ParticleEmitter::SetModel(const Model *m) { model = (Model*)m; };

void ParticleEmitter::SetEmitterOrigin(const float3& f) { emitterOrigin = f; };
const float3& ParticleEmitter::GetEmitterOrigin() const { return emitterOrigin; };

void ParticleEmitter::SetLocalParticleMaximum(const u32 localMax) { localParticleMaximum = localMax; };
const u32 ParticleEmitter::GetLocalParticleMaximum() const { return localParticleMaximum; };

void ParticleEmitter::DoUpdate(const bool shouldUpdate) { doUpdate = shouldUpdate; };
const bool ParticleEmitter::DoUpdate() const { return doUpdate; };
void ParticleEmitter::DoDraw(const bool shouldDraw) { doDraw = shouldDraw; };
const bool ParticleEmitter::DoDraw() const { return doDraw; };
void ParticleEmitter::DoEmit(const bool shouldEmit) { doEmit = shouldEmit; };
const bool ParticleEmitter::DoEmit() const { return doEmit; };

void ParticleEmitter::SetRateOfEmission(const u32 rate) { rateOfEmission = rate; };
const u32 ParticleEmitter::GetRateOfEmission() const { return rateOfEmission; };

void ParticleEmitter::ApplyForces(const bool shouldApplyForces) { applyForces = shouldApplyForces; };
const bool ParticleEmitter::ApplyForces() const { return applyForces; };
void ParticleEmitter::ClearForces() { forceVectors.clear(); };
void ParticleEmitter::AddForce(const float3 &f) { forceVectors.push_back(f); };
const std::vector<float3>& ParticleEmitter::GetForces() const { return forceVectors; };

void ParticleEmitter::SetShader(const Shader *shader) { emitterShader = (Shader*)shader; };
const Shader* ParticleEmitter::GetShader() const { return emitterShader; };

void ParticleEmitter::SetAlphaMap(const Texture t)
{
	alphaMap = t;
	alphaMap.SetTextureSlot(SLOT_GL_TEXTURE_0);
};
const Texture& ParticleEmitter::GetAlphaMap() const { return alphaMap; };

void ParticleEmitter::SetSourceAlphaBlendFunction(const GLenum blendfunc) { sourceAlphaBlendFunction = blendfunc; };
const GLenum ParticleEmitter::GetSourceAlphaBlendFunction() const { return sourceAlphaBlendFunction; };
#pragma endregion