#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"
#include "GradientParticleEmitter.h"
#include "tutorialcodeheaders.h"

World::World(void)
	: grasstexture(0), houseTexture(0), barkTexture(0),
	  houseModel(0),
	  sphere(0),
	  terrain(0),
	  tree2(0),
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

bool World::Load()
{
	oglcall.Load();

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
	
	Texture *particleTexture = texMan.LoadTextureFromFile("Data/Textures/particleTexture.tga");
	particleTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetMagFilter(GL_LINEAR);

	Texture *gradientMap = texMan.LoadTextureFromFile("Data/Textures/gradientMap.tga");
	gradientMap->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	gradientMap->SetMagFilter(GL_LINEAR);

	phongShaderID = shaderMan.AddShader();
	Shader* phongShader = shaderMan.GetShader(phongShaderID);
	phongShader->LoadShader("Data/Shaders/phong.frag", "Data/Shaders/phong.vert");

	globeShaderID = shaderMan.AddShader();
	Shader* globeShader = shaderMan.GetShader(globeShaderID);
	globeShader->LoadShader("Data/Shaders/globe.frag", "Data/Shaders/globe.vert");


	//cam.Init(45, 1.333f, 0.3f, 100, float3(0,1, -3), float3(0,0,1), float3(0,1,0));

	Model *billboardModel = CreateBillboardModel();

	u32 gradientEmitter = particleSystem.AddEmitter<GradientParticleEmitter>();
	GradientParticleEmitter *emitter = (GradientParticleEmitter*)particleSystem.GetEmitter(gradientEmitter);
	emitter->SetParticleSpread(0.4f);
	emitter->SetRateOfEmission(1);
	emitter->SetModel(billboardModel);
	emitter->SetAlphaMap(*particleTexture);
	emitter->SetGradientMap(*gradientMap);
	emitter->SetLocalParticleMaximum(80);
	emitter->SetEmitterOrigin(float3(-5.56f, 3.67f, 0.5f));

	gradientMapShaderID = shaderMan.AddShader();
	Shader* gradientMapShader = shaderMan.GetShader(gradientMapShaderID);
	bool res = gradientMapShader->LoadShader("Data/Shaders/gradientMap.frag", "Data/Shaders/gradientMap.vert");
	if(!res)
	{
		gradientMapShader->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		gradientMapShader->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		gradientMapShader->PrintProgramLog(std::cout);
	}
	//emitter->SetShader(gradientMapShader);
	
	waterPlane = AddModel<Floor>();
	waterPlane->CreateFloor(20, 2.25);

	_material1.create(ColorT::black(), ColorT(0.9f,0.9f,0.9f,1.0f));
	_material2.create(ColorT::black(), ColorT(0.7f,0.7f,0.7f,0.5f));
	_material3.create(ColorT::black(), ColorT::black(), ColorT::yellow());

	// setup some lights (ambient is turned off on all lights)
	_light1.create(0, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light2.create(1, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light3.create(2, ColorT::black(), ColorT(0.5f,0.5f,0.0f,1.0f));
	_light4.create(3, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light5.create(4, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ColorT::black().rgba());

	_light3.setPosition(Vector4f(0.0,5.0,-3.0,1.0));
	// Turn on the lights
	_light1.apply();
	_light2.apply();
	_light3.apply();
	_light4.apply();
	_light5.apply();

	mtt1 = texMan.LoadTextureFromFile("Data/Textures/bark.jpg");
	mtt2 = texMan.LoadTextureFromFile("Data/Textures/checkerboard.jpg");

	multiTextureShaderID = shaderMan.AddShader();
	Shader *multitexshader = shaderMan.GetShader(multiTextureShaderID);
	res = multitexshader->LoadShader("Data/Shaders/multitex.frag", "Data/Shaders/multitex.vert");
	if(!res)
	{
		multitexshader->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		multitexshader->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		multitexshader->PrintProgramLog(std::cout);
	}

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
	delete tree2;
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

void World::reflective_draw(const GameTime &gameTime)
{
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);

	glPushMatrix(); floor(); glPopMatrix(); //floor, pushpop
	
	glEnable(GL_LIGHTING);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

	glPushMatrix(); // scale push
	glScalef(1.0, -1.0, 1.0);

	glEnable(GL_NORMALIZE);
	glCullFace(GL_FRONT);
	_light1.setPosition(Vector4f(5.0,5.0,5.0,1.0));
	_light2.setPosition(Vector4f(-5.0,5.0,5.0,1.0));
	_light4.setPosition(Vector4f(0.0,5.0,-5.0,1.0));
	_light5.setPosition(Vector4f(0.0,-1.0,-5.0,1.0));

	glPushMatrix(); // tree push
	((Model*)tree2->GetBranchModel())->SetDrawMode(polygonMode);
	barkTexture->Activate();
	glTranslatef(2.5f,0,0.2f);
	tree2->Draw(gameTime.GetDeltaTime());
	barkTexture->Deactivate();
	glPopMatrix(); // tree pop

	glPushMatrix(); // floor push
	_material1.apply();
	floor();
	glPopMatrix(); // floor pop

	glDisable(GL_NORMALIZE);
	glCullFace(GL_BACK);

	glPopMatrix(); // scale pop

	glDisable(GL_STENCIL_TEST);

	// put the lights back
	_light1.setPosition(Vector4f(5.0,5.0,5.0,1.0));
	_light2.setPosition(Vector4f(-5.0,5.0,5.0,1.0));
	_light4.setPosition(Vector4f(0.0,5.0,-5.0,1.0));
	_light5.setPosition(Vector4f(0.0,-1.0,-5.0,1.0));

	// make the floor semi-transparent
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_material2.apply();
	floor();
	glDisable(GL_BLEND);

	// now draw the floor again from below but with no blending
	// switch meaning of front face from anti-clockwise to clockwise
	glFrontFace(GL_CW);
	floor();
	// undo the meaning of front face
	glFrontFace(GL_CCW);

	glPushMatrix(); // tree push
	_material1.apply();
	((Model*)tree2->GetBranchModel())->SetDrawMode(polygonMode);
	barkTexture->Activate();
	glTranslatef(2.5f,0,0.2f);
	tree2->Draw(gameTime.GetDeltaTime());
	barkTexture->Deactivate();
	glPopMatrix(); // tree pop
	
	glPopMatrix();
};

void World::multi_texturing_test()
{
	FGLCaller caller; // instance REQUIRED!

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	Shader* mtshader = shaderMan.GetShader(multiTextureShaderID);
	mtshader->Activate();

	oglcall.glActiveTexture(GL_TEXTURE0_ARB);
	oglcall.glBindTextureEXT(GL_TEXTURE_2D, mtt1->GetID());

	oglcall.glActiveTexture(GL_TEXTURE1_ARB);
	oglcall.glBindTextureEXT(GL_TEXTURE_2D, mtt2->GetID());

	mtshader->SetUniform("TextureA", 0);
	mtshader->SetUniform("TextureB", 1);

	cube();

	mtshader->Deactivate();
	/*mtt1->SetTextureSlot(SLOT_GL_TEXTURE_0);
	mtt2->SetTextureSlot(SLOT_GL_TEXTURE_1);

	Shader* mtshader = shaderMan.GetShader(multiTextureShaderID);
	oglcall.glActiveTexture(GL_TEXTURE0);
	mtshader->Activate();
	mtt1->Activate();
	mtshader->SetUniform("TextureA", (i32)mtt1->GetTextureSlotIndex());

	mtt2->Activate();
	oglcall.glActiveTexture(GL_TEXTURE1);
	mtshader->SetUniform("TextureB", (i32)mtt2->GetTextureSlotIndex());

	cube();
	mtshader->Deactivate();*/
	glPopMatrix();
};

void World::Draw(const GameTime &gameTime)
{
	//glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
	glLoadIdentity();

	glPushMatrix();

	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);

	multi_texturing_test();
	glPopMatrix();
	return;
	
	//// Uncomment this code to draw the reflective scene test
	//glEnable(GL_LIGHTING);
	//reflective_draw(gameTime);
	//glPopMatrix();
	//return;
	

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
	
	glPushMatrix();
	glRotatef(90,1,0,0);
	glTranslatef(3.8, 4.2, 0.6);
	waterPlane->Draw();
	glPopMatrix();

	// Base
	Shader *phongShader = shaderMan.GetShader(phongShaderID);
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
	glScalef(scaleX,1,scaleZ);;
	baseModel->Draw();
	glPopMatrix();
	barkTexture->Deactivate();
	phongShader->Deactivate();

	// Tree
	glPushMatrix();
	((Model*)tree2->GetBranchModel())->SetDrawMode(polygonMode);
	barkTexture->Activate();
	glTranslatef(2.5f,0,0.2f);
	tree2->Draw(gameTime.GetDeltaTime());
	barkTexture->Deactivate();
	glPopMatrix();

	// House
	glPushMatrix();

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
	((Model*)houseModel->GetModels())->SetDrawMode(polygonMode);
	houseModel->Draw();
	glPopMatrix();
	houseTexture->Deactivate();
	phongShader->Deactivate();

	glPopMatrix();

	// Particle system
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	particleSystem.Update(gameTime.GetDeltaTime());
	particleSystem.Draw();
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
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);//GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glColor4f(1.0f,1.0f,1.0f,0.25f);
	//sphere->SetDrawMode(polygonMode);
	sphere->Draw();
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