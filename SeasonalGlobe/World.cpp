#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "tutorialcodeheaders.h"

World::World(void)
	: grasstexture(0), houseTexture(0), barkTexture(0),
	  houseModel(0),
	  globeSphere(0),
	  terrain(0),
	  tree(0),
	  sceneRotationAxis(0,1,0),
	  _cameraPosition(-23.2f),
	  _cameraAngle(30.0f),
	  _cameraRotation(-357.0f),
	   AutoRotate(false),
	   polygonMode(GL_FILL)
{
	waterx=3;
	watery=0;
	waterz=0;

	treePos = float3(2.5f,0, 0.2f);
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

Model* CreateBillboardModel()
{
	// Billboard model (textured quad)
	Model *billboardModel = new Model();
	VERTEX billboardVertices[] = 
	{
		VERTEX(float3(-0.5f,1,0), float3(0,0,1), float2(0,1)),
		VERTEX(float3(0.5,1,0),   float3(0,0,1), float2(1,1)),
		VERTEX(float3(-0.5f,0,0), float3(0,0,1), float2(0,0)),
		VERTEX(float3(0.5f,0,0),  float3(0,0,1), float2(1,0))
	};
	billboardModel->SetVertexArray(billboardVertices, 4);
	u32 billboardIndices[] = { 0,2,3, 0,3,1 };
	billboardModel->SetIndicesArray(billboardIndices, 6);
	billboardModel->BuildVBO();
	return billboardModel;
};

Lights _light1, _light2, _light3, _light4, _light5;
Materials _material1, _material2, _material3;

bool World::LoadTextures()
{
	grasstexture = texMan.LoadTextureFromFile("Data/Textures/Grass2.jpg");
	grasstexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR );
	grasstexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR );
	grasstexture->SetWrapS(GL_REPEAT);
	grasstexture->SetWrapT(GL_REPEAT);

	barkTexture = texMan.LoadTextureFromFile("Data/Textures/bark.jpg");

	particleTexture = texMan.LoadTextureFromFile("Data/Textures/particleTexture.tga");
	particleTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	gradientMapTexture = texMan.LoadTextureFromFile("Data/Textures/gradientMap.tga");
	gradientMapTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	gradientMapTexture->SetMagFilter(GL_LINEAR);
	gradientMapTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	return grasstexture && barkTexture && particleTexture && gradientMapTexture;
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

	return true;
};

bool World::LoadParticles()
{
	Model *billboardModel = CreateBillboardModel();
	particleSystem.SetDefaultModel(billboardModel);

	Shader *psysbase = shaderMan.GetShader(particleSystemBaseShaderID);

	leafParticleEmitterID = particleSystem.AddEmitter<StaticParticleEmitter>();
	StaticParticleEmitter *leafEmitter = (StaticParticleEmitter*)particleSystem.GetEmitter(leafParticleEmitterID);
	leafEmitter->SetLocalParticleMaximum(tree->GetLeafCount());
	leafEmitter->SetParticlesStaticState(true);
	leafEmitter->SetAlphaMap(*particleTexture);
	leafEmitter->SetShader(psysbase);

	u32 pointBasedEmitter = particleSystem.AddEmitter<PointBasedParticleEmitter>();
	PointBasedParticleEmitter *emitter = (PointBasedParticleEmitter*)particleSystem.GetEmitter(pointBasedEmitter);
	emitter->SetParticleSpread(0.35f);
	emitter->SetRateOfEmission(2);
	emitter->SetAlphaMap(*particleTexture);
	i32 maxsmokeparticles=80; conf.GetInt("MaxSmokeParticles", maxsmokeparticles);
	emitter->SetLocalParticleMaximum(abs(maxsmokeparticles));
	emitter->SetStartingColor(Color4f(0.2f, 0.2f, 0.2f, 0.75f));
	emitter->SetEndingColor(Color4f(0.2f, 0.2f, 0.2f, 0.1f));
	emitter->SetEmitterOrigin(float3(-5.75f, 3.67f, 0.5f));
	emitter->SetShader(psysbase);
	emitter->SetBillboardType(Spherical);
	emitter->AddForce(float3(1.0f,0,0));
	emitter->AddForce(float3(-1.0f,0.2f,0.43f));

	u32 snowEmitterID = particleSystem.AddEmitter<HemiSphericalParticleEmitter>();
	HemiSphericalParticleEmitter *snowEmitter = (HemiSphericalParticleEmitter*)particleSystem.GetEmitter(snowEmitterID);
	snowEmitter->SetAlphaMap(*particleTexture);
	i32 maxsnowparticles=150; conf.GetInt("MaxSnowParticles", maxsnowparticles);
	snowEmitter->SetLocalParticleMaximum(abs(maxsnowparticles));
	snowEmitter->SetHemiSphereRadius(globeSphere->GetRadius() - (globeSphere->GetRadius() * 0.02f));
	snowEmitter->SetEmitterOrigin(float3(0,5,0));
	snowEmitter->SetShader(psysbase);
	snowEmitter->SetBillboardType(Spherical);

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
	globeSphere = AddModel<Sphere>();
	f32 radius=11.3f;
	conf.GetFloat("GlobeRadius",radius);
	globeSphere->CreateSphere(radius, 40, 40);

	// Load globe base
	baseModel = new OBJFile();
	baseModel->ParseOBJFile("Data/base2.obj");
	baseModel->BuildModelVBOs();
	scaleX = 2.0f; // scale for x and z on globe (height not scaled)
	scaleZ = 2.0f;

	// Load terrain
	terrain = AddModel<TerrainDisk>();
	terrain->CreateTerrainDisk("Data/Textures/ground_heightmap.bmp");

	// Load tree
	tree = new FractalTree();
	tree->SetBranchRadius(1.0f);
	tree->SetBranchRadiusReduction(0.1f);
	tree->SetBranchLength(0.6f);
	tree->SetBranchRotationAngles(30);
	tree->SetInitialString("FFF[A][^^^^^^A]");
	tree->SetInitialString("A");
	tree->AddProductionRule('A', "F[^B][^^^^^^^B]");
	tree->AddProductionRule('B', "F^[-BL]^B");

	i32 _gen = 8;
	conf.GetInt("LSystemGenerations", _gen);
	tree->SetGenerations(_gen);
	
	tree->BuildTree();

	return true;
};

bool World::Load()
{
	conf.ParseConfigFile("Data/ConfigFile.txt"); // load configuration file
	
	LoadTextures();
	LoadGeometry();
	LoadShaders();
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
	texMan.Cleanup();
	shaderMan.Clean();

	for(std::vector<Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	delete houseModel;
	delete tree;
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

	glPushMatrix();
	glTranslatef(treePos.x(), treePos.y(), treePos.z());
	barkTexture->Activate();
	tree->Draw(gameTime.GetDeltaTime()); // draw reflected tree
	tree->DrawLeaves();
	barkTexture->Deactivate();
	glPopMatrix();

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

	glPushMatrix();
	glTranslatef(treePos.x(), treePos.y(), treePos.z());
	_material1.apply();
	barkTexture->Activate();
	tree->Draw(gameTime.GetDeltaTime());
	tree->DrawLeaves();
	barkTexture->Deactivate();
	glPopMatrix();

	glPopMatrix();
};

void World::multi_texturing_test(const GameTime &gameTime)
{
	// Trying the gradient map shader
	glEnable(GL_TEXTURE_2D);

	Shader *shader = shaderMan.GetShader(gradientMapShaderID);
	FGLCaller caller;

	particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);
	gradientMapTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	shader->Activate();

	caller.glActiveTexture(GL_TEXTURE0_ARB);
	particleTexture->Activate();

	caller.glActiveTexture(GL_TEXTURE1_ARB);
	gradientMapTexture->Activate();

	shader->SetUniform("AlphaMap", 0);
	shader->SetUniform("GradientMap",1);
	shader->SetUniform("Time", gameTime.GetRunningTime());

	cube();
	shader->Deactivate();
	
	glPopMatrix();
};

void World::Draw(const GameTime &gameTime)
{
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);

	glRotatef(angle, 0, 1, 0);

	/*glEnable(GL_LIGHTING);
	glPushMatrix();
	reflective_draw(gameTime);
	glPopMatrix();
	glDisable(GL_LIGHTING);*/
	
	glPushMatrix();
	glTranslatef(treePos.x(), treePos.y(), treePos.z());
	barkTexture->Activate();
	tree->Draw(gameTime.GetDeltaTime());
	tree->DrawLeaves();
	barkTexture->Deactivate();
	glPopMatrix();

	// Terrain (floor)
	glPushMatrix();
	grasstexture->Activate();
	glRotatef(90, 1,0,0);
	glScalef(5.48f,5.48f,5.48f);
	terrain->Draw();
	grasstexture->Deactivate();
	glPopMatrix();

	// House
	glPushMatrix();
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
	glPopMatrix();

	// Base
	phongShader->Activate();
	{
		float3 lightPos(0, 5 , 5);
		phongShader->SetUniform("lightPosition", lightPos);
	}
	phongShader->SetUniform("fAmbient", Color::BLACK);
	phongShader->SetUniform("fDiffuse",Color::GREY);
	phongShader->SetUniform("baseMap", *barkTexture);
	phongShader->SetUniform("applyTexture", true);
	barkTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	barkTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	barkTexture->SetWrapS(GL_REPEAT);
	barkTexture->SetWrapT(GL_REPEAT);
	barkTexture->Activate();
	glPushMatrix();
	glScalef(scaleX,0.83f,scaleZ);
	glTranslatef(0, 0.2f, 0);
	baseModel->Draw();
	glPopMatrix();
	barkTexture->Deactivate();
	phongShader->Deactivate();

	// Particle system
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glPushMatrix();
	particleSystem.Update(gameTime);
	particleSystem.Draw(gameTime);
	glPopMatrix();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Globe
	Shader* globeShader = shaderMan.GetShader(globeShaderID);
	globeShader->Activate();
	globeShader->SetUniform("eyePos", cam.GetPosition());
	glPushMatrix();
	glEnable(GL_CLIP_PLANE0); // use clip plane to cut bottom half
	GLdouble eq[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, eq);
	glDisable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	globeSphere->Draw();
	glDisable(GL_BLEND);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
	globeShader->Deactivate();

	glPopMatrix();

	if(AutoRotate)
	{
		angle += rotationSpeed * gameTime.GetDeltaTime();
		if(angle >= 360) { angle-=360; }
	}
}