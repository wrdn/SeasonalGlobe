#include "World.h"

GLuint textures[2];

World::World(void)
{
}

World::~World(void)
{
}

bool World::Load()
{
	cubeModel = new OBJFile();
	cubeModel->ParseOBJFile("Data/TexturedCube.obj");
	cubeModel->BuildModelVBOs();

	if(const Texture* t=texMan.LoadTextureFromFile("Data/tiles.jpg"))
	{
		testTextureID = t->GetID();
	}

	floor = new Floor();
	floor->CreateFloor(40,4.5f);

	return true;
};

void World::Shutdown()
{
	texMan.DeleteTexture(testTextureID);
};

float angle=0; const float rotationSpeed = 50.0f;
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

	gluLookAt(0,0.5f,8,0,0,-1,0,1,0);
	glRotatef(angle,1,0,0);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, testTextureID);

	//cubeModel->Draw();
	floor->Draw();

	angle += rotationSpeed * gameTime.GetDeltaTime();
	if(angle >= 360) { angle-=360; }
}