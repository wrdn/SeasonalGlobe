#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() : isActive(true), emitterOrigin(), tex()
{
};

ParticleEmitter::~ParticleEmitter()
{
};

void ParticleEmitter::Update(const f32 dt)
{
	if(!isActive) return;

	if(dt) { }; // get rid of unreferenced formal parameter
};

void ParticleEmitter::Draw(f32 dt)
{
	if(!isActive) return;
	
	tex.Activate();
	for(u32 i=0;i<MAX_PARTICLES;++i)
	{
		particles[i].pos += particles[i].velocity;
		//particles[i].timeToLive -= dt;
		particles[i].timeToLive -= 1;

		Color4f col = Color::FromInt(particles[i].color.GetColor());
		glColor3fv(col.GetVec());

		glPushMatrix();

		glScalef(0.001f, 0.001f, 0.001f);
		glTranslatef(particles[i].pos.x(), particles[i].pos.y(), particles[i].pos.z());

		f32 mat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);

		/*mat[0] = mat[10] = 1.0f;
		mat[1] = mat[2] = mat[8] = mat[9] = 0.0f;*/

		mat[0] 	= mat[10] = mat[5] 	= 1;
		mat[1] 	= mat[2] 	= mat[3] 	= mat[4] = 0;
		mat[6] 	= mat[7] 	= mat[8] 	= mat[9] = 0;
		mat[11] 	= 0;
		glLoadMatrixf(mat);

		BillboardModel.Draw();

		glPopMatrix();

		if(particles[i].timeToLive < 0)
			particles[i].SetDefaultValues();
	}
	tex.Deactivate();
};

void ParticleEmitter::InitParticles()
{
	for(u32 i=0;i<MAX_PARTICLES;++i)
	{
		particles[i].SetDefaultValues();
	}
};