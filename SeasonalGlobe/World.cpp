#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "CylindricalParticleEmitter.h"
#include "tutorialcodeheaders.h"

World::World(void)
	/*: grasstexture(0), houseTexture(0), barkTexture(0),
	  houseModel(0),
	  globeSphere(0),
	  terrain(0),
	  tree(0),
	  sceneRotationAxis(0,1,0),
	  _cameraPosition(-23.2f),
	  _cameraAngle(30.0f),
	  _cameraRotation(-357.0f),
	   AutoRotate(false),
	   polygonMode(GL_FILL),
	   phongShaderID(0), globeShaderID(0), multiTextureShaderID(0),
	   particleSystemBaseShaderID(0), leafParticleEmitterID(0),
	   cylindricalParticleEmitterID(0), snowEmitterID(0), smokeEmitter(0),
	   gradientMapShaderID(0), fireEmitter(0), particleTexture(0), gradientMapTexture(0),
	   leafTexture(0), baseModel(0), scaleX(0), scaleZ(0), mtt1(0), mtt2(0),
	   waterx(3), watery(0), waterz(0), */
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

Model* CreateBillboardModel()
{
	// Billboard model (textured quad)
	Model *billboardModel = new Model();
	VERTEX *billboardVertices = new VERTEX[4];
	billboardVertices[0] = VERTEX(float3(-0.5f,1,0), float3(0,0,1), float2(0,1));
	billboardVertices[1] = VERTEX(float3(0.5,1,0),   float3(0,0,1), float2(1,1));
	billboardVertices[2] = VERTEX(float3(-0.5f,0,0), float3(0,0,1), float2(0,0));
	billboardVertices[3] = VERTEX(float3(0.5f,0,0),  float3(0,0,1), float2(1,0));
	billboardModel->SetVertexArray(billboardVertices, 4);

	u32 *billboardIndices = new u32[6];
	billboardIndices[0] = 0;
	billboardIndices[1] = 2;
	billboardIndices[2] = 3;
	billboardIndices[3] = 0;
	billboardIndices[4] = 3;
	billboardIndices[5] = 1;
	billboardModel->SetIndicesArray(billboardIndices, 6);
	billboardModel->BuildVBO();
	return billboardModel;
};

Lights _light1, _light2, _light3, _light4, _light5;
Materials _material1, _material2, _material3;

bool World::LoadTextures()
{
	grassTexture = texMan.LoadTextureFromFile("Data/Textures/Grass2.jpg");
	grassTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR );
	grassTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR );
	grassTexture->SetWrapS(GL_REPEAT);
	grassTexture->SetWrapT(GL_REPEAT);

	barkTexture = texMan.LoadTextureFromFile("Data/Textures/bark.jpg");

	particleTexture = texMan.LoadTextureFromFile("Data/Textures/particleTexture.tga");
	particleTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	leafTexture = texMan.LoadTextureFromFile("Data/Textures/leaf.tga");
	leafTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	leafTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	leafTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	baseTexture = texMan.LoadTextureFromFile("Data/Textures/wood.jpg");

	return grassTexture && barkTexture && particleTexture && leafTexture && baseTexture;
};

bool World::LoadShaders()
{
	particleSystemBaseShaderID = shaderMan.AddShader();
	Shader *psysbase = shaderMan.GetShader(particleSystemBaseShaderID);
	if(!psysbase->LoadShader("Data/Shaders/particlesystembase.frag", "Data/Shaders/particlesystembase.vert"))
	{
		psysbase->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		psysbase->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		psysbase->PrintProgramLog(std::cout);
		return false;
	}

	phongShaderID = shaderMan.AddShader();
	Shader* phongShader = shaderMan.GetShader(phongShaderID);
	if(!phongShader->LoadShader("Data/Shaders/phong.frag", "Data/Shaders/phong.vert"))
	{
		phongShader->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		phongShader->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		phongShader->PrintProgramLog(std::cout);
		return false;
	}

	globeShaderID = shaderMan.AddShader();
	Shader* globeShader = shaderMan.GetShader(globeShaderID);
	if(!globeShader->LoadShader("Data/Shaders/globe.frag", "Data/Shaders/globe.vert"))
	{
		globeShader->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		globeShader->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		globeShader->PrintProgramLog(std::cout);
		return false;
	}

	directionalLightShaderID = shaderMan.AddShader();
	directionalLightShader = shaderMan.GetShader(directionalLightShaderID);
	if(!directionalLightShader->LoadShader("Data/Shaders/directionalLight.frag", "Data/Shaders/directionalLight.vert"))
	{
		directionalLightShader->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		directionalLightShader->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		directionalLightShader->PrintProgramLog(std::cout);
		return false;
	}

	return true;
};

bool World::LoadParticles()
{
	defaultBillboardModel = CreateBillboardModel();
	particleSystem.SetDefaultModel(defaultBillboardModel);

	Shader *psysbase = shaderMan.GetShader(particleSystemBaseShaderID);

	/*const u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;
	leafParticleEmitterID = particleSystem.AddEmitter<StaticParticleEmitter>();
	StaticParticleEmitter *leafEmitter = particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID);
	leafEmitter->SetLocalParticleMaximum(tree->GetLeafCount() * LEAF_PARTICLES_PER_LEAF_MATRIX);
	leafEmitter->SetParticlesStaticState(true);
	leafEmitter->SetAlphaMap(*leafTexture);
	leafEmitter->SetShader(psysbase);
	leafEmitter->SetEmitterOrigin(treePos);
	leafEmitter->SetBillboardType(Cylindrical);
	leafEmitter->SetSourceAlphaBlendFunction(GL_ONE_MINUS_SRC_ALPHA);
	const Mat44 *LeafMatrices = tree->GetLeafMatrices();
	const u32 LeafCount = tree->GetLeafCount();
	for(u32 i=0;i<LeafCount;++i)
	{
		glMatrixMode(GL_MODELVIEW);

		Particle p;
		const f32 *leafMat = LeafMatrices[i].GetMatrix();
		p.pos = float3(leafMat[12], leafMat[13], leafMat[14]);

		for(u32 j=0;j<LEAF_PARTICLES_PER_LEAF_MATRIX;++j)
		{
			p.rotation_z = randflt(0, 360);

			p.size = float3(0.25f);
			leafEmitter->AddParticle(p);
		}
	}*/


	smokeEmitterID = particleSystem.AddEmitter<PointBasedParticleEmitter>();
	PointBasedParticleEmitter *smokeParticleEmitter = particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID);
	smokeParticleEmitter->SetParticleSpread(0.35f);
	smokeParticleEmitter->SetRateOfEmission(2);
	smokeParticleEmitter->SetAlphaMap(*particleTexture);
	i32 maxsmokeparticles=80; conf.GetInt("MaxSmokeParticles", maxsmokeparticles);
	smokeParticleEmitter->SetLocalParticleMaximum(abs(maxsmokeparticles));
	smokeParticleEmitter->SetStartingColor(Color4f(0.2f, 0.2f, 0.2f, 0.75f));
	smokeParticleEmitter->SetEndingColor(Color4f(0.2f, 0.2f, 0.2f, 0.1f));
	smokeParticleEmitter->SetEmitterOrigin(float3(-2.8f, 1.98f, 0.2f));
	smokeParticleEmitter->SetShader(psysbase);
	smokeParticleEmitter->SetBillboardType(Spherical);
	smokeParticleEmitter->AddForce(float3(1.0f,0,0));
	smokeParticleEmitter->AddForce(float3(-1.0f,0.2f,0.43f));

	snowEmitterID = particleSystem.AddEmitter<HemiSphericalParticleEmitter>();
	HemiSphericalParticleEmitter *snowEmitter = particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID);
	snowEmitter->SetAlphaMap(*particleTexture);
	i32 maxsnowparticles=150; conf.GetInt("MaxSnowParticles", maxsnowparticles);
	snowEmitter->SetLocalParticleMaximum(abs(maxsnowparticles));
	snowEmitter->SetHemiSphereRadius(globeSphere->GetRadius() - (globeSphere->GetRadius() * 0.02f));
	snowEmitter->SetEmitterOrigin(float3(0,0,0));
	snowEmitter->SetShader(psysbase);
	snowEmitter->SetBillboardType(Spherical);

	u32 fireParticleEmitterID = particleSystem.AddEmitter<FireParticleEmitter>();
	fireParticleEmitter = particleSystem.GetEmitter<FireParticleEmitter>(fireParticleEmitterID);
	fireParticleEmitter->SetLocalParticleMaximum(30000);
	fireParticleEmitter->SetAlphaMap(*particleTexture);
	fireParticleEmitter->SetShader(psysbase);
	fireParticleEmitter->SetBillboardType(Spherical);
	fireParticleEmitter->SetEmitterOrigin(tree->GetPosition());
	fireParticleEmitter->SetRateOfEmission(1000);

	std::vector<ParticleLine> fire_particle_lines;
	tree->CalculateParticleLines(fire_particle_lines);

	for(int i=0;i<fire_particle_lines.size();++i)
	{
		fireParticleEmitter->AddLine(fire_particle_lines[i]);
	}
	
	return true;
};

bool World::LoadGeometry()
{
	// Load house model
	houseModel = new OBJFile();
	houseModel->ParseOBJFile("Data/House/Haus20.obj");
	houseModel->BuildModelVBOs();
	houseTexture = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");

	// Load globe
	globeSphere = new Sphere();
	f32 radius=11.3f; conf.GetFloat("GlobeRadius",radius);
	globeSphere->CreateSphere(radius, 40, 40);
	globeSphere->SetPosition(float3(0.3f,0,0));
	globeSphere->SetShader(shaderMan.GetShader(globeShaderID));

	// Load globe base
	baseModel = new OBJFile();
	baseModel->ParseOBJFile("Data/base2.obj");
	baseModel->BuildModelVBOs();

	// Load terrain
	terrain = new TerrainDisk();
	terrain->CreateTerrainDisk("Data/Textures/ground_heightmap.bmp");
	terrain->SetTexture(grassTexture);
	terrain->SetXRotation(90);
	terrain->SetScale(float3(5.48f,5.48f,5.48f));

	// Load tree
	tree = new FractalTree();
	tree->SetPosition(float3(2.5f, 0, 0.2f));
	tree->SetTexture(barkTexture);
	tree->SetBranchRadius(1.0f);
	tree->SetBranchRadiusReduction(0.1f);
	tree->SetBranchLength(0.6f);
	tree->SetBranchRotationAngles(30);
	//tree->SetInitialString("FFF[A][^^^^^^A]");
	tree->SetInitialString("A");
	tree->AddProductionRule('A', "F[^B][^^^^^^^B]");
	tree->AddProductionRule('B', "F^[L-BL]^B");
	i32 gen = 8; conf.GetInt("LSystemGenerations", gen);
	tree->SetGenerations(gen);
	tree->BuildTree();

	return true;
};

bool World::Load()
{
	conf.ParseConfigFile("Data/ConfigFile.txt"); // load configuration file
	
	//cam.SetRadius(28.0f);
	//cam.SetAzimuth(20);
	//cam.Update();

	//cam.Init(45, 1.333f, 0.3f, 100, float3(0,1, -28), float3(0,1,1), float3(0,1,0));

	cam.Init(float3(0,6,25), float3(0,6,25), float3(0,1,0));

	directionalLight = DirectionalLight(float3(), Color4f(0.3f,0.3f,0.3f,1), Color4f(0.75f,0.75f,0.75f,1), Color4f(1,1,1,1));

	LoadTextures();
	LoadShaders();
	LoadGeometry();
	LoadParticles();
	
	// NOTE: THIS IS ALL CODE FROM TUTORIALS THAT SHOULD LATER BE REMOVED
	// Load materials
	_material1.create(ColorT::black(), ColorT(0.9f,0.9f,0.9f,1.0f));
	_material2.create(ColorT::black(), ColorT(0.7f,0.7f,0.7f,0.5f));
	_material3.create(ColorT::black(), ColorT::black(), ColorT::yellow());

	_light1.create(0, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light2.create(1, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light3.create(2, ColorT::black(), ColorT(0.5f,0.5f,0.0f,1.0f));
	_light4.create(3, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light5.create(4, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ColorT::black().rgba());

	_light3.setPosition(Vector4f(0.0,5.0,-3.0,1.0));
	_light1.apply();
	_light2.apply();
	_light3.apply();
	_light4.apply();
	_light5.apply();

	return true;
};

void World::Shutdown()
{
	shaderMan.Clean();
	texMan.Cleanup();

	delete houseModel;
	delete baseModel;
	delete globeSphere;
	delete tree;
	delete terrain; // crash
	delete defaultBillboardModel; // crash
};

f32 angle=0; const f32 rotationSpeed = 50.0f;
f32 inc=0;

void cube()
{
		// Front Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);
	glTexCoord2d(0.0, 0.0);		glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d(-1.0,  1.0,  1.0);
	glEnd();
	// Back Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,-1.0f);
	glTexCoord2d(1.0, 0.0);		glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d(-1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d( 1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d( 1.0, -1.0, -1.0);
	glEnd();
	// Top Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,1.0f,0.0f);
	glTexCoord2d(0.0, 1.0);		glVertex3d(-1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d(-1.0,  1.0,  1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d( 1.0,  1.0, -1.0);
	glEnd();
	// Bottom Face
	glBegin(GL_QUADS);
	glNormal3f(0.0f,-1.0f,0.0f);
	glTexCoord2d(1.0, 1.0);		glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d( 1.0, -1.0, -1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d(-1.0, -1.0,  1.0);
	glEnd();
	// Right face
	glBegin(GL_QUADS);
	glNormal3f(1.0f,0.0f,0.0f);
	glTexCoord2d(1.0, 0.0);		glVertex3d( 1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d( 1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 0.0);		glVertex3d( 1.0, -1.0,  1.0);
	glEnd();
	// Left Face
	glBegin(GL_QUADS);
	glNormal3f(-1.0f,0.0f,0.0f);
	glTexCoord2d(0.0, 0.0);		glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 0.0);		glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0);		glVertex3d(-1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0);		glVertex3d(-1.0,  1.0, -1.0);
	glEnd();
};
void texcube(Texture &t)
{
	glEnable(GL_TEXTURE_2D);
	t.Activate();
	cube();
	t.Deactivate();
	glDisable(GL_TEXTURE_2D);
}
void floor()
{
	glPushMatrix();
	//glScalef(0.5,0.5,0.5);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);

	glTexCoord2f(0,0);
	glVertex3f(-10.0f,0.0f,10.0f);

	glTexCoord2f(1,0);
	glVertex3f(10.0f,0.0f,10.0f);

	glTexCoord2f(1,1);
	glVertex3f(10.0f,0.0f,-10.0f);

	glTexCoord2f(0,1);
	glVertex3f(-10.0f,0.0f,-10.0f);
	glEnd();

	glPopMatrix();
}

void DrawFloor(const float3 &floorPos, const float3 &floorScale)
{
	glTranslatef(floorPos.x(), floorPos.y(), floorPos.z());
	glScalef(floorScale.x(), floorScale.y(), floorScale.z());
	floor(); // draw floor into stencil
};

void World::reflective_draw(const GameTime &gameTime)
{
	float3 floorScale(0.3f, 0.3f, 0.3f);
	float3 floorPos(3.5f, -0.35f, 3.6f);

	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);
	glEnable(GL_STENCIL_TEST);
	
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);

	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();
	glScalef(1,-1,1);

	glCullFace(GL_FRONT);
	glEnable(GL_NORMALIZE);

	_light1.setPosition(Vector4f(5.0,5.0,5.0,1.0));
	_light2.setPosition(Vector4f(-5.0,5.0,5.0,1.0));
	_light4.setPosition(Vector4f(0.0,5.0,-5.0,1.0));
	_light5.setPosition(Vector4f(0.0,-1.0,-5.0,1.0));

	tree->Draw(gameTime.GetDeltaTime());

	glDisable(GL_NORMALIZE);
	glCullFace(GL_BACK);

	glPopMatrix();

	glDisable(GL_STENCIL_TEST);

	_light1.setPosition(Vector4f(5.0,5.0,5.0,1.0));
	_light2.setPosition(Vector4f(-5.0,5.0,5.0,1.0));
	_light4.setPosition(Vector4f(0.0,5.0,-5.0,1.0));
	_light5.setPosition(Vector4f(0.0,-1.0,-5.0,1.0));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_material2.apply();
	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);
	glFrontFace(GL_CW);
	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);

	tree->Draw(gameTime.GetDeltaTime());

	glDisable(GL_LIGHTING);
};

void World::multi_texturing_test(const GameTime &gameTime)
{
	/*
	// Trying the gradient map shader
	glEnable(GL_TEXTURE_2D);

	Shader *shader = shaderMan.GetShader(gradientMapShaderID);
	FGLCaller caller;

	particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);
	//gradientMapTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	shader->Activate();

	caller.glActiveTexture(GL_TEXTURE0_ARB);
	particleTexture->Activate();

	caller.glActiveTexture(GL_TEXTURE1_ARB);
	//gradientMapTexture->Activate();

	shader->SetUniform("AlphaMap", 0);
	shader->SetUniform("GradientMap",1);
	shader->SetUniform("Time", gameTime.GetRunningTime());

	cube();
	shader->Deactivate();
	
	glPopMatrix();
	*/
};

void World::Update(GameTime &gameTime)
{
	particleSystem.Update(gameTime);
};

float xrot=0, yrot=0, radius=3.0f;
float ra=0;
void World::Draw(const GameTime &gameTime)
{
	glLoadIdentity();

	glPushMatrix();

	// This translates everything else in the scene, objects move, not the camera
	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);

	glRotatef(angle, 0, 1, 0);

	glPushMatrix();
	reflective_draw(gameTime);
	glPopMatrix();

	_light1.setPosition(Vector4f(5.0,5.0, 10.0, 1.0));
	_light2.setPosition(Vector4f(-5.0,5.0,5.0,1.0));
	_light4.setPosition(Vector4f(0.0,5.0,-5.0,1.0));
	_light5.setPosition(Vector4f(0.0,-1.0,-5.0,1.0));
	
	/*
	directionalLightShader->Activate();
	directionalLightShader->SetUniform("lightAmbient", directionalLight.ambient);
	directionalLightShader->SetUniform("lightDiffuse", directionalLight.diffuse);
	directionalLightShader->Deactivate();
	*/

	// Terrain (floor)
	glEnable(GL_NORMALIZE);
	terrain->Draw();
	glDisable(GL_NORMALIZE);

	// House
	Shader *phongShader = shaderMan.GetShader(phongShaderID);
	phongShader->Activate();
	float3 lightPos(0, 5 , 5);
	phongShader->SetUniform("lightPosition", lightPos);
	phongShader->SetUniform("fAmbient", Color::BLACK);
	phongShader->SetUniform("fDiffuse", Color::WHITE);
	phongShader->SetUniform("baseMap", *houseTexture);
	phongShader->SetUniform("applyTexture", true);
	houseTexture->Activate();
	glPushMatrix();
	glTranslatef(-5.5,0,0.5);
	glScalef(.05f,.05f,.05f);
	houseModel->Draw();
	glPopMatrix();
	houseTexture->Deactivate();
	phongShader->Deactivate();

	// Base
	phongShader->Activate();
	lightPos = float3(0, 0 , 10);
	phongShader->SetUniform("lightPosition", lightPos);
	phongShader->SetUniform("fAmbient", Color::BLACK);
	phongShader->SetUniform("fDiffuse",Color::GREY);
	phongShader->SetUniform("baseMap", *baseTexture);
	phongShader->SetUniform("applyTexture", true);
	glPushMatrix();
	baseTexture->Activate();
	baseTexture->SetWrapS(GL_REPEAT);
	baseTexture->SetWrapT(GL_REPEAT);
	glScalef(2.0f,0.83f,2.0f);
	glTranslatef(0, 0.2f, 0);
	baseModel->Draw();
	baseTexture->Deactivate();
	glPopMatrix();
	//barkTexture->Deactivate();
	phongShader->Deactivate();

	// Particle system
	particleSystem.Draw();

	// Globe
	//Shader* globeShader = shaderMan.GetShader(globeShaderID);
	//globeShader->Activate();
	//globeShader->SetUniform("eyePos", cam.GetPosition());
	
	glEnable(GL_CLIP_PLANE0); // use clip plane to cut bottom half
	GLdouble eq[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, eq);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	//glPushMatrix();
	//glTranslatef(0.3f,0,0);
	globeSphere->Draw();
	//glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_CLIP_PLANE0);
	//globeShader->Deactivate();

	glPopMatrix();

	if(AutoRotate)
	{
		angle += rotationSpeed * gameTime.GetDeltaTime();
		if(angle >= 360) { angle-=360; }
	}
}
