#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"

World::World(void)
	: grasstexture(0), houseTexture(0), barkTexture(0),
	  houseModel(0),
	  sphere(0),
	  terrain(0),
	  tree2(0),
	  sceneRotationAxis(0,1,0),
	  _cameraAngle(30.0f),
	  _cameraPosition(-3.8500025f),
	  _cameraRotation(-232.0f),
	   AutoRotate(false),
	   polygonMode(GL_FILL)
{
}

World::~World(void)
{
	try
	{
		Shutdown();
	}
	catch(...) { };
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
	conf.ParseConfigFile("Data/ConfigFile.txt");

	grasstexture = texMan.LoadTextureFromFile("Data/Textures/Grass2.jpg");
	
	//barkTexture = texMan.LoadTextureFromFile("Data/Textures/checkerboard_Test.jpg");
	barkTexture = texMan.LoadTextureFromFile("Data/Textures/bark.jpg");
	//barkTexture->SetWrapS(GL_REPEAT);
	//barkTexture->SetWrapT(GL_REPEAT);

	houseModel = new OBJFile();
	houseModel->ParseOBJFile("Data/House/Haus20.obj");
	houseModel->BuildModelVBOs();
	houseTexture = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");

	sphere = AddModel<Sphere>();
	f32 radius=11;
	conf.GetFloat("GlobeRadius",radius);
	sphere->CreateSphere(radius, 40, 40);

	baseModel = new OBJFile();
	baseModel->ParseOBJFile("Data/base2.obj");
	baseModel->BuildModelVBOs();
	scaleX = 2.1f; // scale for x and z on globe (height not scaled)
	scaleZ = 2.1f;

	terrain = AddModel<TerrainDisk>();
	terrain->CreateTerrainDisk("Data/Textures/ground_heightmap.bmp");

	// http://www.geekyblogger.com/2008/04/tree-and-l-system.html
	tree2 = new FractalTree();
	tree2->SetBranchRadius(1.0f);
	tree2->SetBranchRadiusReduction(0.1f);
	tree2->SetBranchLength(0.6f);
	tree2->SetBranchRotationAngles(30);
	tree2->SetInitialString("FFF[A][^^^^^^A]");
	tree2->SetInitialString("A");
	tree2->AddProductionRule('A', "F[^B][^^^^^^^B]");
	tree2->AddProductionRule('B', "F^[-B]^B");

	i32 _gen = 8;
	conf.GetInt("LSystemGenerations", _gen);
	tree2->SetGenerations(_gen);
	
	tree2->BuildTree();
	
	Texture *particleTexture = texMan.LoadTextureFromFile
		("Data/Textures/particleTexture.tga");
	particleTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetMagFilter(GL_LINEAR);

	psys = new ParticleSystem();
	psys->Init();
	testEmitter = psys->GetEmitter();
	testEmitter.Get()->Init();
	testEmitter.Get()->SetTexture(*particleTexture);
	testEmitter.Get()->InitParticles();
	//testEmitter.Get()->particles

	_phongShader = new Shader();
	if(_phongShader->Init())
	{
		//c8* phong_frag_src = read_src_fast("Data/Shaders/phong.frag");
		//c8* phong_vert_src = read_src_fast("Data/Shaders/phong.vert");
		c8* phong_frag_src = read_src("Data/Shaders/phong.frag");
		c8* phong_vert_src = read_src("Data/Shaders/phong.vert");

		_phongShader->CompilerVertexShader(phong_vert_src);
		_phongShader->CompileFragmentShader(phong_frag_src);
		_phongShader->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		_phongShader->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);

		_phongShader->CreateProgram();

		delete [] phong_frag_src;
		delete [] phong_vert_src;
	}

	cam.Init(45, 1.333f, 0.3f, 100, float3(0,1, -3), float3(0,0,1), float3(0,1,0));

	return true;
};

void World::Shutdown()
{
	texMan.Cleanup();

	_phongShader->Deactivate();
	delete _phongShader;

	for(std::vector<Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	delete houseModel;
	delete tree2;
};

f32 angle=0; const f32 rotationSpeed = 50.0f;
float inc=0;
void World::Draw(const GameTime &gameTime)
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glDisable(GL_LIGHTING);
	//glEnable(GL_LIGHTING);

	//_light1.create(0, Color::black(), Color(0.5f,0.5f,0.5f,1.0f)); 

	glEnable(GL_TEXTURE_2D);
	
	glLoadIdentity();

	glPushMatrix();

	/*glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);*/
	cam.Update();

	glRotatef(angle, 0, 1, 0);

	inc += 0.0001f;
	if(inc > 20) inc=0;

	// Terrain (floor)
	glPushMatrix();
	grasstexture->Activate();
	glRotatef(90, 1,0,0);
	glScalef(5.48f,5.48f,5.48f);
	terrain->SetDrawMode(polygonMode);
	terrain->Draw();
	grasstexture->Deactivate();
	glPopMatrix();
	
	// Base
	barkTexture->Activate();
	glPushMatrix();
	glScalef(scaleX,1,scaleZ);;
	baseModel->Draw();
	glPopMatrix();
	barkTexture->Deactivate();


	// Tree
	glPushMatrix();
	((Model*)tree2->GetBranchModel())->SetDrawMode(polygonMode);
	barkTexture->Activate();
	tree2->Draw(gameTime.GetDeltaTime());
	barkTexture->Deactivate();
	glPopMatrix();

	// House
	glPushMatrix();

	_phongShader->Activate();
	float3 mv(cam.GetPosition());
	_phongShader->PrintActiveUniforms();
	_phongShader->SetUniform("timeVariable", mv);
	_phongShader->SetUniform("cameraPosition2", mv);

	glTranslatef(-5.5,0,0.5);
	glScalef(.05f,.05f,.05f);
	houseTexture->Activate();
	((Model*)houseModel->GetModels())->SetDrawMode(polygonMode);
	houseModel->Draw();
	houseTexture->Deactivate();

	_phongShader->Deactivate();

	glPopMatrix();

	// Particle system
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	psys->Draw(gameTime.GetDeltaTime());
	glPopMatrix();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Globe
	glPushMatrix();
	glEnable(GL_CLIP_PLANE0); // use clip plane to cut bottom half
	GLdouble eq[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, eq);
	glDisable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);//GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f,1.0f,1.0f,0.25f);
	sphere->SetDrawMode(polygonMode);
	sphere->Draw();
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