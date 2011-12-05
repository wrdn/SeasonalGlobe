#include "KParticleEmitter.h"

KParticleEmitter::KParticleEmitter() : emitterTexture(0), gravityVector(GRAVITY_VECTOR), windDirection(0.0f),
	sourceAlphaBlendFunction(GL_ONE), rateOfEmission(20), billboardType(Spherical), maxEmitterParticles(GLOBAL_MAX_PARTICLES)
{
};

KParticleEmitter::~KParticleEmitter()
{
};

void KParticleEmitter::Draw()
{
	((Texture*)emitterTexture)->Activate();

	for(u32 i=0;i<maxEmitterParticles;++i)
	{
		KParticle &p = particles[i];
		
		glPushMatrix();
		glColor4fv(p.color.GetVec());
		//glColor4f(p.color.x(), p.color.y(), p.color.z(), 1);
		//glColor3f(0.1,0.1,0.1);

		glScalef(p.size.x(), p.size.y(), p.size.z());
		glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());

		f32 mat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		if(billboardType == Spherical)
		{
			mat[0] = mat[10] = 1.0f;
			mat[1] = mat[2] = mat[8] = mat[9] = 0.0f;
		}
		else // cylindrical
		{
			mat[0] = mat[10] = mat[5] = 1.0f;
			mat[1] = mat[2] = mat[3] = mat[4] = 
			mat[6] = mat[7] = mat[8] = mat[9] =
			mat[11] = 0.0f;
		}
		glLoadMatrixf(mat);
		billboardModel.Draw();
		glPopMatrix();
	}

	emitterTexture->Deactivate();

	glColor3f(1,1,1);
};

void KParticleEmitter::Update(const f32 dt)
{
	u32 emittedThisFrame = rateOfEmission;

	for(u32 i=0;i<maxEmitterParticles;++i)
	{
		KParticle &p = particles[i];
		
		p.pos += p.velocity * dt; //(p.velocity * gravityVector) + (p.velocity * windDirection);
		p.energy -= dt; //1.0f;

		if(p.energy < 0 && emittedThisFrame>0 ) // particle dead
		{
			Emit(p, (void*)&dt);
			--emittedThisFrame;
			continue;
		}
	}
};