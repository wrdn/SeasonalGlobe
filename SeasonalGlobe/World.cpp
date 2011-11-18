#include "World.h"

GLuint textures[2];

World::World(void)
{
	_cameraAngle = 30.0f;
	_cameraPosition = -15.0f;
	_cameraRotation = 0.0f;

	terrainPolyMode = GL_FILL;
}

World::~World(void)
{
}

bool World::Load()
{
	cubeModel = new OBJFile();
	cubeModel->ParseOBJFile("Data/TexturedCube.obj");
	cubeModel->BuildModelVBOs();

	grasstexture = texMan.LoadTextureFromFile("Data/Textures/Grass2.jpg");
	grasstexture->SetWrapS(GL_REPEAT);
	grasstexture->SetWrapT(GL_REPEAT);
	if(grasstexture) { testTextureID = grasstexture->GetID(); }
	
	houseModel = new OBJFile();
	houseModel->ParseOBJFile("Data/House/Haus20.obj");
	houseModel->BuildModelVBOs();
	Texture *t = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");
	if(t) { houseTextureID = t->GetID(); }

	floor = new Floor();
	floor->CreateFloor(40,11);

	sphere = new Sphere();
	sphere->CreateSphere(11,40,40);

	terrain = new TerrainDisk();
	terrain->CreateTerrainDisk("Data/Textures/ground_heightmap.bmp");

	return true;
};

void World::Shutdown()
{
	texMan.DeleteTexture(testTextureID);
};

float angle=0; const float rotationSpeed = 50.0f;
void World::Draw(const GameTime &gameTime)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	grasstexture->Activate();

	//glBindTexture(GL_TEXTURE_2D, testTextureID);
	glRotatef(90, 1,0,0);
	glScalef(5.48f,5.48f,5.48f);
	glPolygonMode(GL_FRONT_AND_BACK, terrainPolyMode);
	terrain->Draw(false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	grasstexture->Deactivate();

	glPopMatrix();
	
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

	//sphere->GetModel().SetDrawMode(terrainPolyMode);
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_CULL_FACE);
	//glPushMatrix();
	//sphere->Draw();
	//glPopMatrix();

	glPushMatrix();
	glEnable(GL_CLIP_PLANE0); // use clip plane to cut bottom half
	GLdouble eq[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, eq);
	glDisable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f,1.0f,1.0f,0.25f);
	sphere->GetModel().SetDrawMode(terrainPolyMode);
	sphere->Draw();
	glDisable(GL_BLEND);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	glPopMatrix();

	angle += rotationSpeed * gameTime.GetDeltaTime();
	if(angle >= 360) { angle-=360; }
}