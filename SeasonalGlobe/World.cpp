#include "World.h"
#include "strutils.h"
#include "Color.h"

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
	
	houseModel = new OBJFile();
	houseModel->ParseOBJFile("Data/House/Haus20.obj");
	houseModel->BuildModelVBOs();
	houseTexture = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");

	floor = new Floor();
	floor->CreateFloor(40,11);

	sphere = new Sphere();
	sphere->CreateSphere(11,40,40);

	terrain = new TerrainDisk();
	terrain->CreateTerrainDisk("Data/Textures/ground_heightmap.bmp");

	if(_phongShader.Init())
	{
		c8* phong_frag_src = read_src_fast("Data/Shaders/phong.frag");
		c8* phong_vert_src = read_src_fast("Data/Shaders/phong.vert");

		_phongShader.CompilerVertexShader(phong_vert_src);
		_phongShader.CompileFragmentShader(phong_frag_src);
		_phongShader.PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		_phongShader.PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);

		if(_phongShader.CreateProgram())
		{
			_phongShader.Activate();
			
			_phongShader.SetUniform("red", 3.0f);
			
			_phongShader.Deactivate();
		}

		delete [] phong_frag_src;
		delete [] phong_vert_src;
	}

	_cylinder = new Cylinder();
	_cylinder->Create(6, 5, 20, 20);

	return true;
};

void World::Shutdown()
{
	texMan.Cleanup();
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
	//_cylinder->Draw();
	_cylinder->GetModel().DrawVertexPoints();
	glPopMatrix();


	/*glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	grasstexture->Activate();
	glRotatef(90, 1,0,0);
	glScalef(5.48f,5.48f,5.48f);
	glPolygonMode(GL_FRONT_AND_BACK, terrainPolyMode);
	terrain->Draw(false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	grasstexture->Deactivate();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-5.5,0,0.5);
	glScalef(.05f,.05f,.05f);
	houseTexture->Activate();
	houseModel->Draw();
	houseTexture->Deactivate();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,2,0);
	glRotatef(angle,1,1,0);
	houseTexture->Activate();
	_phongShader.Activate();
	cubeModel->Draw();
	_phongShader.Deactivate();
	houseTexture->Deactivate();
	glPopMatrix();

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
	glPopMatrix();*/

	glPopMatrix();

	angle += rotationSpeed * gameTime.GetDeltaTime();
	if(angle >= 360) { angle-=360; }
}