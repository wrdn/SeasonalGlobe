#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"

World::World(void)
{
	_cameraAngle = 30.0f;
	_cameraPosition = -3.8500025f;
	_cameraRotation = -232.0f;

	terrainPolyMode = GL_FILL;

	sceneRotationAxis.set(0, 1, 0);
	AutoRotate = false;
}

World::~World(void)
{
}

// adds the model to models vector, templated to ensure constructor gets called properly
// example: Sphere *s = AddModel<Sphere>();
template<class T>
T* World::AddModel()
{
	T *n = new T();
	models.push_back(n);
	return n;
};

bool World::Load()
{
	grasstexture = texMan.LoadTextureFromFile("Data/Textures/Grass2.jpg");
	
	//barkTexture = texMan.LoadTextureFromFile("Data/Textures/checkerboard_Test.jpg");
	barkTexture = texMan.LoadTextureFromFile("Data/Textures/bark.jpg");
	barkTexture->SetWrapS(GL_REPEAT);
	barkTexture->SetWrapT(GL_REPEAT);

	houseModel = new OBJFile();
	houseModel->ParseOBJFile("Data/House/Haus20.obj");
	houseModel->BuildModelVBOs();
	houseTexture = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");

	sphere = AddModel<Sphere>();
	sphere->CreateSphere(11, 40, 40);

	terrain = AddModel<TerrainDisk>();
	terrain->CreateTerrainDisk("Data/Textures/ground_heightmap.bmp");

	/*
	// http://www.geekyblogger.com/2008/04/tree-and-l-system.html
	tree = new FractalTree();
	tree->Init(float3(25), 1.0f, 0.1f, 0.6f);
	//tree->SetInitialString("FFFF[A][^^^^^^A]");
	tree->SetInitialString("FF+F[+F]");
	tree->AddProductionRule('A', "F[++A][--A]>>>A");
	tree->SetGenerations(2);
	tree->EvaluateTreeLSystem();
	tree->Draw(false);*/
	
	tree2 = new FractalTree2();
	tree2->SetBranchRadius(1.0f);
	tree2->SetBranchRadiusReduction(0.1f);
	tree2->SetBranchLength(0.6f);
	tree2->SetBranchRotationAngles(30);
	tree2->SetInitialString("FFF[A][^^^^^^A]");
	//tree2->SetInitialString("FFF[A]");
	tree2->SetInitialString("A");
	tree2->AddProductionRule('A', "F[^B][^^^^^^^B]");
	tree2->AddProductionRule('B', "F^[-B]^B");

	tree2->SetGenerations(8);

	//tree2->SetInitialString("FF[+FF]");

	tree2->BuildTree(false);
	tree2->SetDrawLevel(0);

	/*if(_phongShader.Init())
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
	}*/

	return true;
};

void World::Shutdown()
{
	texMan.Cleanup();

	for(std::vector<Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
};

f32 angle=0; const f32 rotationSpeed = 50.0f;
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
	
	glRotatef(angle, 0, 1, 0);

	glPushMatrix();
	tree2->GetBranchModel().SetDrawMode(terrainPolyMode);
	barkTexture->Activate();
	tree2->Draw(gameTime.GetDeltaTime() * 0.2f);
	barkTexture->Deactivate();
	glPopMatrix();


	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	grasstexture->Activate();
	glRotatef(90, 1,0,0);
	glScalef(5.48f,5.48f,5.48f);
	terrain->SetDrawMode(terrainPolyMode);
	terrain->Draw();
	grasstexture->Deactivate();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-5.5,0,0.5);
	glScalef(.05f,.05f,.05f);
	houseTexture->Activate();
	((Model*)houseModel->GetModels())->SetDrawMode(terrainPolyMode);
	houseModel->Draw();
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
	sphere->SetDrawMode(terrainPolyMode);
	//sphere->Draw();
	glDisable(GL_BLEND);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	glPopMatrix();

	if(AutoRotate)
	{
		angle += rotationSpeed * gameTime.GetDeltaTime();
		if(angle >= 360) { angle-=360; }
	}
}