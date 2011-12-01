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

	// get rid of unreferenced formal parameter warning
	// (remove when Update(f32 dt) implemented)
	if(dt) { }
};

void ParticleEmitter::Draw()
{
	if(!isActive) return;

	//glScalef(10,10,10);

	/*tex.Activate();
	BillboardModel.Draw();
	tex.Deactivate();*/

	tex.Activate();
	//glScalef(0.1f, 0.1f, 0.1f);
	for(u32 i=0;i<MAX_PARTICLES;++i)
	{
		Particle &p = particles[i];
		if(p.alive)
		{
			glPushMatrix();

			glTranslatef(p.pos.x(), p.pos.y(), p.pos.z());

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
		}
	}
	tex.Deactivate();
};