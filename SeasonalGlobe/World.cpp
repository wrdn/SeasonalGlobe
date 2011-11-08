#include "World.h"


World::World(void)
{
}

World::~World(void)
{
}

bool World::Load()
{
	cubeModel.ParseOBJFile("Data/untitled.obj");
	cubeModel.GetModels()[0]->BuildVBO();
	return true;
};

float angle=0; const float rotationSpeed = 10.0f;
void World::Draw(const GameTime &gameTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	f32 ambLight[] = { 1,1,1,1 };
	f32 diffLight[] = { 1,1,1, 1.0f };
	f32 lightPos[] = { 0,0,-1.0f,1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	f32 const matAmbient[] = { 1,1,1,1 };
	f32 const matDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);

	gluLookAt(0,0,0,0,0,-1,0,1,0);

	glTranslatef(0,0,-3);
	glRotatef(angle,0,1,0);

	cubeModel.Draw();

	angle += rotationSpeed * gameTime.GetDeltaTime();
	if(angle >= 360) { angle-=360; }
}