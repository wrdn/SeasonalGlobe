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

	Texture *t = texMan.LoadTextureFromFile("Data/Textures/Grass.jpg");

	if(t) { testTextureID = t->GetID(); }

	floor = new Floor();
	floor->CreateFloor(40,11);

	houseModel = new OBJFile();
	houseModel->ParseOBJFile("Data/House/Haus20.obj");
	houseModel->BuildModelVBOs();

	t = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");
	if(t) { houseTextureID = t->GetID(); }

	sphere = new Sphere();
	sphere->CreateSphere(11,40,40);

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

	gluLookAt(0,15,25,0,0,-1,0,1,0);
	//glRotatef(angle,1,0,0);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, testTextureID);

	floor->Draw();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, houseTextureID);
	glTranslatef(-5.5,0,0.5);
	glScalef(.05f,.05f,.05f);
	houseModel->Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,2,0);
	glRotatef(angle,1,1,0);
	cubeModel->Draw();
	glPopMatrix();

	glEnable(GL_CLIP_PLANE0); // use clip plane to cut bottom half
	GLdouble eq[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, eq);

	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f,1.0f,1.0f,0.1f);
	sphere->Draw();
	glDisable(GL_BLEND);
	glPopMatrix();

	glDisable(GL_CLIP_PLANE0);

	angle += rotationSpeed * gameTime.GetDeltaTime();
	if(angle >= 360) { angle-=360; }
}