﻿#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "CylindricalParticleEmitter.h"
#include "tutorialcodeheaders.h"

World::World(void)
	: conf(), // Application configuration

	texMan(), shaderMan(), particleSystem(), seasonMan(), // Managers

	cam(), sceneRotationAxis(0,1,0), _cameraAngle(30.0f), _cameraPosition(-30.0f), // Camera
	_cameraRotation(-357.0f), AutoRotate(false),

	terrain(0), tree(0), globeSphere(0), houseModel(0), baseModel(0), // Geometry
	defaultBillboardModel(0), polygonMode(GL_FILL),lightSphere(0), spotCone(0),

	phongShaderID(0), particleSystemBaseShaderID(0), globeShaderID(0), directionalLightShaderID(0), // Shaders
	multiTexturingSampleShaderID(0), spotlightShaderID(0), ambientLightShaderID(0), 
	normalMap_Ambient_ShaderID(0), normalMap_Directional_ShaderID(0), normalMap_Spotlights_ShaderID(0),
	treeShaders(), terrainShaders(),

	grassTexture(0), houseTexture(0), barkTexture(0), particleTexture(0), leafTexture(0), baseTexture(0), // Textures
	displacementTexture(0), barkNormalMap(0),

	leafParticleEmitterID(0), snowEmitterID(0), smokeEmitterID(0), fireParticleEmitter(0), // Particle Emitters

	directionalLight(), lightMode(Directional) // Lights
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
	grassTexture = texMan.LoadTextureFromFile("Data/Textures/snowTexture.jpg");
	grassTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR );
	grassTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR );
	grassTexture->SetWrapS(GL_REPEAT);
	grassTexture->SetWrapT(GL_REPEAT);

	barkTexture = texMan.LoadTextureFromFile("Data/Textures/bark.jpg");
	barkTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);
	barkNormalMap = texMan.LoadTextureFromFile("Data/Textures/barkNormalMap2.jpg");
	barkNormalMap->SetTextureSlot(SLOT_GL_TEXTURE_1);

	particleTexture = texMan.LoadTextureFromFile("Data/Textures/particleTexture.tga");
	particleTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	leafTexture = texMan.LoadTextureFromFile("Data/Textures/leaf.tga");
	leafTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	leafTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	leafTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	baseTexture = texMan.LoadTextureFromFile("Data/Textures/wood.jpg");

	houseTexture = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap.jpg");

	displacementTexture = texMan.LoadTextureFromFile("Data/Textures/TerrainDisplacementMap.bmp");
	displacementTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	return grassTexture && barkTexture && particleTexture && leafTexture && baseTexture && houseTexture && displacementTexture;
};

bool World::LoadShader(u32 &id, const c8* fragmentShaderFilename, const c8* vertexShaderFilename)
{
	id = shaderMan.AddShader(); 
	Shader *sh = shaderMan.GetShader(id);
	if(!sh->LoadShader(fragmentShaderFilename, vertexShaderFilename))
	{
		std::cout << vertexShaderFilename << " , " << fragmentShaderFilename << std::endl;
		sh->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		sh->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		sh->PrintProgramLog(std::cout);
		return false;
	}
	return true;
};

bool World::LoadShaders()
{
	LoadShader(multiTexturingSampleShaderID, "Data/Shaders/multitex.frag", "Data/Shaders/multitex.vert");

	LoadShader(particleSystemBaseShaderID, "Data/Shaders/particlesystembase.frag", "Data/Shaders/particlesystembase.vert");
	
	if(LoadShader(phongShaderID, "Data/Shaders/phong.frag", "Data/Shaders/phong.vert"))
	{
		Shader* phongShader = shaderMan.GetShader(phongShaderID);
		float3 lightPos(0, 10 , 0);
		phongShader->Activate();
		phongShader->SetUniform("lightPosition", lightPos);
		phongShader->SetUniform("fAmbient", Color::GREY);
		phongShader->SetUniform("fDiffuse", Color::WHITE);
		phongShader->SetUniform("baseMap", *houseTexture);
		phongShader->SetUniform("applyTexture", true);
		phongShader->Deactivate(); // initialise the shader uniforms
	}
	
	// Globe shader
	LoadShader(globeShaderID, "Data/Shaders/globe.frag", "Data/Shaders/globe.vert");

	// Lighting shaders
	LoadShader(directionalLightShaderID, "Data/Shaders/directionalLight.frag", "Data/Shaders/directionalLight.vert");
	LoadShader(spotlightShaderID, "Data/Shaders/spotlight.frag", "Data/Shaders/spotlight.vert");
	LoadShader(ambientLightShaderID, "Data/Shaders/ambientLight.frag", "Data/Shaders/ambientLight.vert");

	// Normal mapping shaders
	LoadShader(normalMap_Ambient_ShaderID, "Data/Shaders/NormalMapping/normalMapping_ambient_light.frag",
		"Data/Shaders/NormalMapping/normalMapping_ambient_light.vert");
	LoadShader(normalMap_Directional_ShaderID, "Data/Shaders/NormalMapping/normalMapping_directional_light.frag",
		"Data/Shaders/NormalMapping/normalMapping_directional_light.vert");
	LoadShader(normalMap_Spotlights_ShaderID, "Data/Shaders/NormalMapping/normalMapping_spotlights.frag",
		"Data/Shaders/NormalMapping/normalMapping_spotlights.vert");
	
	shaderMan.GetShader(normalMap_Directional_ShaderID)->Activate();
	shaderMan.GetShader(normalMap_Directional_ShaderID)->SetUniform("baseMap",0);
	shaderMan.GetShader(normalMap_Directional_ShaderID)->SetUniform("normalMap",1);
	shaderMan.GetShader(normalMap_Directional_ShaderID)->Deactivate();


	// Displacement mapping terrain shaders
	LoadShader(terrainShaders.Terrain_Displacement_Ambient_ShaderID, 
		"Data/Shaders/TerrainShaders/displacement_ambient.frag", "Data/Shaders/TerrainShaders/displacement_ambient.vert");
	LoadShader(terrainShaders.Terrain_Displacement_Directional_ShaderID, 
		"Data/Shaders/TerrainShaders/displacement_directional.frag", "Data/Shaders/TerrainShaders/displacement_directional.vert");
	LoadShader(terrainShaders.Terrain_Displacement_Spotlights_ShaderID, 
		"Data/Shaders/TerrainShaders/displacement_spotlights.frag", "Data/Shaders/TerrainShaders/displacement_spotlights.vert");

	// Load all the tree shaders
	LoadShader(treeShaders.Tree_Ambient_ShaderID, "Data/Shaders/TreeShaders/ambientLight.frag", "Data/Shaders/TreeShaders/ambientLight.vert");
	LoadShader(treeShaders.FlatNonTextured_Directional_ShaderID,
		"Data/Shaders/TreeShaders/FlatShading/directionalLight.frag", "Data/Shaders/TreeShaders/FlatShading/directionalLight.vert");
	LoadShader(treeShaders.FlatNonTextured_Spot_ShaderID,
		"Data/Shaders/TreeShaders/FlatShading/spotlight.frag", "Data/Shaders/TreeShaders/FlatShading/spotlight.vert");
	LoadShader(treeShaders.SmoothShaded_Directional_ShaderID,
		"Data/Shaders/TreeShaders/SmoothShading/directionalLight.frag", "Data/Shaders/TreeShaders/SmoothShading/directionalLight.vert");
	LoadShader(treeShaders.SmoothShaded_Spot_ShaderID,
		"Data/Shaders/TreeShaders/SmoothShading/spotlight.frag", "Data/Shaders/TreeShaders/SmoothShading/spotlight.vert");

	return true;
};

bool World::LoadParticles()
{
	defaultBillboardModel = CreateBillboardModel();
	particleSystem.SetDefaultModel(defaultBillboardModel);

	Shader *psysbase = shaderMan.GetShader(particleSystemBaseShaderID);

	const u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;
	leafParticleEmitterID = particleSystem.AddEmitter<StaticParticleEmitter>();
	StaticParticleEmitter *leafEmitter = particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID);
	leafEmitter->SetLocalParticleMaximum(tree->GetLeafCount() * LEAF_PARTICLES_PER_LEAF_MATRIX);
	leafEmitter->SetParticlesStaticState(true);
	leafEmitter->SetAlphaMap(*leafTexture);
	leafEmitter->SetShader(psysbase);
	leafEmitter->SetEmitterOrigin(tree->GetPosition());
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
	}
	leafEmitter->SetParticlesStaticState(true);
	leafEmitter->SetStartColor(Color4f(1,1,1,0));
	leafEmitter->SetEndColor(Color4f(1,1,1,1));
	//leafEmitter->SetStartColor(Color4f(1));
	//leafEmitter->SetEndColor(Color4f(0.8f, 0.35f, 0.35f, 1));
	leafEmitter->DoColorUpdate(true);
	leafEmitter->SetActive(false);
	//leafEmitter->InitiateParticleFade();
	//leafEmitter->SetBillboardType(NoBillboarding);*/

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
	smokeParticleEmitter->SetActive(false);

	snowEmitterID = particleSystem.AddEmitter<HemiSphericalParticleEmitter>();
	HemiSphericalParticleEmitter *snowEmitter = particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID);
	snowEmitter->SetAlphaMap(*particleTexture);
	i32 maxsnowparticles=150; conf.GetInt("MaxSnowParticles", maxsnowparticles);
	snowEmitter->SetLocalParticleMaximum(abs(maxsnowparticles));
	snowEmitter->SetHemiSphereRadius(globeSphere->GetRadius() - (globeSphere->GetRadius() * 0.075f));
	snowEmitter->SetEmitterOrigin(float3(0,0,0));
	snowEmitter->SetShader(psysbase);
	snowEmitter->SetBillboardType(Spherical);
	snowEmitter->SetActive(false);

	u32 fireParticleEmitterID = particleSystem.AddEmitter<FireParticleEmitter>();
	fireParticleEmitter = particleSystem.GetEmitter<FireParticleEmitter>(fireParticleEmitterID);
	fireParticleEmitter->SetLocalParticleMaximum(30000);
	fireParticleEmitter->SetAlphaMap(*particleTexture);
	fireParticleEmitter->SetShader(psysbase);
	fireParticleEmitter->SetBillboardType(Spherical);
	fireParticleEmitter->SetEmitterOrigin(tree->GetPosition());
	fireParticleEmitter->SetRateOfEmission(1000);
	fireParticleEmitter->SetTree(tree);
	std::vector<ParticleLine> fire_particle_lines;
	tree->CalculateParticleLines(fire_particle_lines);

	for(u32 i=0;i<fire_particle_lines.size();++i)
	{
		fireParticleEmitter->AddLine(fire_particle_lines[i]);
	}
	fireParticleEmitter->SetActive(false);

	return true;
};

bool World::LoadGeometry()
{
	// Load house model
	houseModel = OBJFile::ParseFile("Data/House/Haus20.obj")[0];
	houseModel->GetModel().BuildVBO();
	houseModel->SetScale(float3(.05f,.05f,.05f));
	houseModel->SetPosition(float3(-5.5,0,0.5));
	houseModel->SetTextureA(houseTexture);
	//Material mat(float4(0.25f, 0.25f, 0.25f, 1), float4(1,0,0,1), float4(1,1,1,1), 100);
	//houseModel->SetMaterial(mat);

	// Load globe base
	baseModel = OBJFile::ParseFile("Data/base2.obj")[0];
	((Model&)baseModel->GetModel()).BuildVBO();
	baseModel->SetTextureA(baseTexture);
	baseModel->SetPosition(float3(0, 0.2f, 0));
	baseModel->SetScale(float3(2.0f, 0.83f, 2.0f));

	// Load globe
	globeSphere = new Sphere();
	f32 radius=11.3f; conf.GetFloat("GlobeRadius",radius);
	globeSphere->Create(radius, 40, 40);
	globeSphere->SetPosition(float3(0.3f,0,0));

	// Load terrain
	terrain = new TerrainLoader();
	terrain->Load("Data/Textures/ground_heightmap.bmp");
	terrain->SetTextureA(grassTexture);
	terrain->SetTextureB(displacementTexture);
	terrain->SetXRotation(-90);
	terrain->SetZRotation(-90);
	terrain->SetScale(float3(5.48f,5.48f,5.48f));
	terrain->SetMaterial(Material(float4(1.0f), float4(1.0f), float4(1.0f), 0));

	// Load tree
	tree = new FractalTree();
	tree->SetPosition(float3(2.5f, 0, 0.2f));
	tree->SetTexture(barkTexture);
	tree->SetNormalMap(barkNormalMap);
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
	//tree->SetBuildTime(0);
	//tree->SetTreeDieing(true);

	// Set the shader on each object (for lighting)
	//Shader* directionalLightShader = shaderMan.GetShader(directionalLightShaderID);
	//Shader* directionalLightShader = shaderMan.GetShader(spotlightShaderID);
	//houseModel->SetShader(directionalLightShader);
	//baseModel->SetShader(directionalLightShader);
	//terrain->SetShader(directionalLightShader);

	SetLightingMode(Spotlights); // default to directional lights

	//terrain->SetShader(shaderMan.GetShader(displace));

	globeSphere->SetShader(shaderMan.GetShader(globeShaderID));

	lightSphere = new Sphere();
	lightSphere->Create(0.5,20,20);
	lightSphere->SetTextureA(barkTexture);

	spotCone = new Cylinder();
	spotCone->Create(0.1f, 0.3f, 0.45f, 10, 10);
	spotCone->SetTextureA(baseTexture);
	spotCone->SetPosition(spotlights[0].GetPosition().ToFloat3());

	SetLightingMode(Directional);
	SetTreeShadeMode(SmoothTextured);

	return true;
};

void World::LoadLights()
{
	// Directional Light
	float4 directionalLightAmb(0.35f,0.35f,0.35f,1), directionLightDiffuse(1,0.66666f,0.19607f,1), directionalLightSpecular(1,1,1,1), lightPosition;
	directionalLight = Light(lightPosition,directionalLightAmb, directionLightDiffuse, directionalLightSpecular);
	directionalLight.SetLightID(GL_LIGHT0);

	
	float4 spotAmb(0.35f,0.35f,0.35f,1);
	float4 spotDiff(1,0.66666f,0.19607f,1);
	float4 spotSpec(1,1,1,1); float4 spotDir;

	// Spotlight 1
	lightPosition = float4(0,7.5f,0,1); spotDir = float4(0,-1,0,1);
	spotlights[0] = Light(lightPosition, spotDir, 10, spotAmb, spotDiff, spotSpec);
	spotlights[0].SetLightID(GL_LIGHT0);

	// Spotlight 2
	lightPosition = float4(-5.3f, 2, 9.6f,1); spotDir = float4(0,0,-1,1); spotAmb = float4(0,0,0,1); spotDiff = float4(1,1,1,1);
	spotlights[1] = Light(lightPosition, spotDir, 10, spotAmb, spotDiff, spotSpec);
	spotlights[1].SetLightID(GL_LIGHT1);

	// Spotlight 3
	lightPosition = float4(4.4f, 2, 10, 1); spotDir = float4(0,0,-1,1); spotDiff = float4(0.6f,0.5f,0.4f,1);
	spotlights[2] = Light(lightPosition, spotDir, 10, spotAmb, spotDiff, spotSpec);
	spotlights[2].SetLightID(GL_LIGHT2);

	// Spotlight 4
	lightPosition = float4(0,0.6f,-11.2f,1); spotDir = float4(0,0,1,1); spotAmb = float4(0.35f,0.35f,0.35f,1);
	spotDiff = float4(1,0.66666f,0.19607f,1);
	spotlights[3] = Light(lightPosition, spotDir, 15, spotAmb, spotDiff, spotSpec);
	spotlights[3].SetLightID(GL_LIGHT3);
};

void StartTreeGrowth(World *w) { w->GetTree()->InitGrow(); }
void InitiateLeafGrowth(World *w)
{
	w->GetLeafParticleEmitter()->SetActive(true);
	w->GetLeafParticleEmitter()->SetStartColor(Color4f(1,1,1,1));
	w->GetLeafParticleEmitter()->InitiateParticleFadeIn();
};
void InitiateLeafColorChange(World *w)
{
	w->GetLeafParticleEmitter()->SetEndColor(Color4f(0.8f, 0.35f, 0.35f, 1));
	w->GetLeafParticleEmitter()->InitiateMainColorChange();
};
void InitiateLeafFall(World *w)
{
	w->GetLeafParticleEmitter()->InitiateParticleFall();
};
void InitiateTreeIgnitionFire(World *w)
{
	w->GetFireParticleEmitter()->SetRuntime(0);
	w->GetFireParticleEmitter()->SetBurningState(Igniting);
	w->GetFireParticleEmitter()->SetActive(true);
};

void InitiateLeafVanish(World *w) { w->GetLeafParticleEmitter()->InitiateParticleFadeOut(); }

void World::SetupSeasons()
{
	seasonMan.SetTimePerSeason(15); // 8 seconds per season, 32 seconds per cycle
	seasonMan.SetWorldPointer(this);

	// Spring: Tree grows
	tree->SetBuildTime(min(tree->GetBuildTime(), seasonMan.GetTimePerSeason()));
	seasonMan.AddEvent(Spring, SeasonalEvent(0, StartTreeGrowth)); // tree grows at start of spring
	
	// Summer: Leaves appear on tree
	seasonMan.AddEvent(Summer, SeasonalEvent(0.2f, InitiateLeafGrowth));
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToFadeIn(seasonMan.GetTimePerSeason() * 0.2f);

	// Autumn: Leaves change colour and fall from tree (then disappear over time), lightning bolt hits tree and it burns down
	seasonMan.AddEvent(Autumn, SeasonalEvent(0, InitiateLeafColorChange));
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToChangeColor(seasonMan.GetTimePerSeason() * 0.25f);

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.2f, InitiateLeafFall));
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToChangeColor(seasonMan.GetTimePerSeason() * 0.2f);

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.35f, InitiateLeafVanish));
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToChangeColor(seasonMan.GetTimePerSeason() * 0.2f);

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.4, InitiateTreeIgnitionFire));
	fireParticleEmitter->SetIgnitionTime(seasonMan.GetTimePerSeason() * 0.6f);
};

bool World::Load()
{
	conf.ParseConfigFile("Data/ConfigFile.txt"); // load configuration file
	
	//cam.Init(float3(0,6,25), float3(0,6,25), float3(0,1,0));

	LoadLights();
	LoadTextures();
	LoadShaders();
	LoadGeometry();
	LoadParticles();
	
	SetupSeasons();

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
	delete lightSphere;
	delete spotCone;
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
void texcube(const Texture &t)
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
	float3 floorPos(3.5f, -0.45f, 3.6f);
	
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_2D);

	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);

	glPushMatrix();
	DrawFloor(floorPos, floorScale);
	glPopMatrix();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();
	glScalef(1,-1,1);

	glCullFace(GL_FRONT);
	glEnable(GL_NORMALIZE);

	glDisable(GL_LIGHTING);
	tree->Draw(gameTime.GetDeltaTime());
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->Draw();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glDisable(GL_NORMALIZE);
	glCullFace(GL_BACK);

	glPopMatrix();

	glDisable(GL_STENCIL_TEST);

	glEnable(GL_LIGHTING);
	
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

	glDisable(GL_LIGHTING);
	tree->Draw(gameTime.GetDeltaTime());
};


void World::multi_texturing_test(/*const GameTime &gameTime*/)
{
	
	// Trying the gradient map shader
	glEnable(GL_TEXTURE_2D);

	Shader *shader = shaderMan.GetShader(multiTexturingSampleShaderID);
	glex g;
	g.Load();

	//particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);
	//gradientMapTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);
	grassTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);
	houseTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	shader->Activate();

	//g.glActiveTexture(GL_TEXTURE0_ARB);
	//particleTexture->Activate();

	//g.glActiveTexture(GL_TEXTURE1_ARB);
	//gradientMapTexture->Activate();

	g.glActiveTexture(grassTexture->GetTextureSlot());
	grassTexture->Activate();

	g.glActiveTexture(houseTexture->GetTextureSlot());
	houseTexture->Activate();
	
	shader->SetUniform("TextureA", *grassTexture);
	shader->SetUniform("TextureB",*houseTexture);
	//shader->SetUniform("Time", gameTime.GetRunningTime());

	cube();
	shader->Deactivate();
	
	glPopMatrix();
	
};

void World::Update(const GameTime &gameTime)
{
	particleSystem.Update(gameTime);
};

f32 xrot=0, yrot=0, radius=3.0f;
f32 ra=0;

//f32 vpos_mult=0, terrainShift=0;
//f32 terrainRuntime=0;

f32 MaxDisplacementScale = 0.45f;
f32 MaxShift = 1.45f;
f32 CurrentShift=0, CurrentScale=0;
f32 ShiftTime = 15, CurrentTerrainTime=0; // 5 seconds to make snow drifts
bool loopTerrainGrowth=false;

void World::Draw(const GameTime &gameTime)
{
	seasonMan.Update(gameTime.GetDeltaTime());

	glLoadIdentity();

	glPushMatrix();

	// This translates everything else in the scene, objects move, not the camera
	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);

	glRotatef(angle, 0, 1, 0);

	//_light1.setPosition(Vector4f(5.0,5.0, 10.0, 1.0));
	//_light2.setPosition(Vector4f(-5.0,5.0,5.0,1.0));
	//_light4.setPosition(Vector4f(0.0,5.0,-5.0,1.0));
	//_light5.setPosition(Vector4f(0.0,-1.0,-5.0,1.0));

	fireParticleEmitter->UpdateFireParticleEmitter(gameTime);

	if(lightMode == Spotlights)
	{
		spotlights[0].Activate();
		spotCone->SetXRotation(0);
		spotCone->SetPosition(spotlights[0].GetPosition().ToFloat3());
		spotCone->Draw();

		spotlights[1].Activate();
		spotCone->SetXRotation(90);
		spotCone->SetPosition(float3(-5.3f, 1.7f, 9.6f));
		spotCone->Draw();

		spotlights[2].Activate();
		spotCone->SetPosition(float3(4.4f, 1.7f, 10));
		spotCone->Draw();

		spotlights[3].Activate();
		spotCone->SetPosition(spotlights[3].GetPosition().ToFloat3());
		spotCone->SetZRotation(180);
		spotCone->Draw();
		spotCone->SetZRotation(0);
	}
	else if(lightMode == Directional)
	{
		// Directional Light
		directionalLight.Activate();
		f32 radius = 13; float3 pos(0,0,0);
		float3 newPos(pos.x()+radius*sin(ra), pos.y()+radius*cos(ra), pos.z());
		lightSphere->SetPosition(newPos);
		float4 lightSpherePos = lightSphere->GetPosition().ToFloat4();
		directionalLight.SetPositionAndUpdateOpenGL(lightSpherePos);
		ra += 0.5f * gameTime.GetDeltaTime();
		lightSphere->Draw();
	}

	glPushMatrix();
	reflective_draw(gameTime);
	glPopMatrix();

	// Terrain (floor)
	CurrentTerrainTime += gameTime.GetDeltaTime();

	if(loopTerrainGrowth)
		CurrentTerrainTime = fmod(CurrentTerrainTime, ShiftTime);
	else
		CurrentTerrainTime = min(CurrentTerrainTime, ShiftTime);

	f32 N = (1.0f / ShiftTime) * CurrentTerrainTime; // N in range 0 to 1
	f32 vpos = lerp(0, 0.45f, N);
	CurrentShift = lerp(0, MaxShift, N);

	glDisable(GL_CULL_FACE);
	terrain->SetPosition(float3(0, terrain->GetPosition().y()-CurrentShift,0));
	terrain->GetShader()->Activate();
	terrain->GetShader()->SetUniform("colorMap", 0);
	terrain->GetShader()->SetUniform("displacementMap", 1);
	terrain->GetShader()->SetUniform("vposmult", vpos);
	terrain->GetShader()->Deactivate();
	terrain->SetXRotation(-90);
	terrain->SetYRotation(0);
	terrain->SetZRotation(-90);
	terrain->Draw();
	terrain->SetPosition(float3(0, terrain->GetPosition().y()+CurrentShift,0));
	glEnable(GL_CULL_FACE);

	// House
	houseModel->Draw();

	// Base
	baseModel->Draw();

	// Particle system
	particleSystem.Draw();

	// Globe
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
