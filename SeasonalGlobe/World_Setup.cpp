#pragma once

#include "World.h"
#include "tutorialcodeheaders.h"
#include "graphics_utils.h"

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
	//_material2.create(ColorT::black(), ColorT(0.7f,0.7f,0.7f,0.5f));
	_material2.create(ColorT::black(), ColorT(0.24f,0.24f,1.0f,0.8f));
	_material3.create(ColorT::black(), ColorT::black(), ColorT::yellow());

	_light1.create(0, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light2.create(1, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light3.create(2, ColorT::black(), ColorT(0.5f,0.5f,0.0f,1.0f));
	_light4.create(3, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f)); 
	_light5.create(4, ColorT::black(), ColorT(0.5f,0.5f,0.5f,1.0f));
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ColorT::black().GetVec());

	_light3.setPosition(Vector4f(0.0,5.0,-3.0,1.0));
	_light1.apply();
	_light2.apply();
	_light3.apply();
	_light4.apply();
	_light5.apply();

	SetLightingMode(Spotlights);

	return true;
};

bool World::LoadTextures()
{
	grassTexture = LoadTexture("Data/Textures/Grass2.jpg");
	grassTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR );
	grassTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR );
	grassTexture->SetWrapS(GL_REPEAT);
	grassTexture->SetWrapT(GL_REPEAT);
	grassTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	grassParticleTexture = LoadTexture("Data/Textures/grass2.tga");
	grassParticleTexture->SetWrapS(GL_CLAMP);
	grassParticleTexture->SetWrapT(GL_CLAMP);
	grassParticleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	grassParticleColorMap = LoadTexture("Data/Textures/grassParticleTexture.bmp");
	grassParticleColorMap->SetTextureSlot(SLOT_GL_TEXTURE_1);

	snowTexture = LoadTexture("Data/Textures/snowTexture.jpg");
	snowTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR );
	snowTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR );
	snowTexture->SetWrapS(GL_REPEAT);
	snowTexture->SetWrapT(GL_REPEAT);
	snowTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	terrainNormalMapFull = LoadTexture("Data/Textures/TerrainDisplacementNormalMap_Full.bmp");
	terrainNormalMapFull->SetTextureSlot(SLOT_GL_TEXTURE_2);

	houseNormalMap = LoadTexture("Data/House/Haus_020_unwrap_NRM.jpg");
	houseNormalMap->SetTextureSlot(SLOT_GL_TEXTURE_1);

	displacementTexture = LoadTexture("Data/Textures/TerrainDisplacementMap.bmp");
	displacementTexture->SetTextureSlot(SLOT_GL_TEXTURE_3);

	barkTexture = LoadTexture("Data/Textures/bark.jpg");
	barkTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	barkNormalMap = LoadTexture("Data/Textures/barkNormalMap2.jpg");
	barkNormalMap->SetTextureSlot(SLOT_GL_TEXTURE_1);

	particleTexture = LoadTexture("Data/Textures/particleTexture.tga");
	particleTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	particleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	leafTexture = LoadTexture("Data/Textures/leaf.tga");
	leafTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	leafTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR);
	leafTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	baseTexture = LoadTexture("Data/Textures/wood.jpg");

	houseTexture = LoadTexture("Data/House/Haus_020_unwrap.jpg");

	return grassTexture && barkTexture && particleTexture && leafTexture && baseTexture && houseTexture && displacementTexture;
};

bool World::LoadShaders()
{
	multiTexturingSampleShader = LoadShader("Data/Shaders/multitex.vert", "Data/Shaders/multitex.frag");
	particleSystemBaseShader = LoadShader("Data/Shaders/particlesystembase.vert", "Data/Shaders/particlesystembase.frag");

	texturedParticleShader = LoadShader("Data/Shaders/texturedParticle.vert", "Data/Shaders/texturedParticle.frag");
	if(texturedParticleShader)
	{
		texturedParticleShader->SetUniform("AlphaMap",0);
		texturedParticleShader->SetUniform("ColorMap",1);
	}

	phongShader = LoadShader("Data/Shaders/phong.vert", "Data/Shaders/phong.frag");
	if(phongShader)
	{
		phongShader->SetUniform("lightPosition", float3(0,10,0));
		phongShader->SetUniform("fAmbient", Color::GREY);
		phongShader->SetUniform("fDiffuse", Color::WHITE);
		phongShader->SetUniform("baseMap", houseTexture);
		phongShader->SetUniform("applyTexture", true);
	}
	
	// Globe shader
	globeShader = LoadShader("Data/Shaders/globe.vert", "Data/Shaders/globe.frag");

	// Lighting shaders
	directionalLightShader = LoadShader("Data/Shaders/directionalLight.vert", "Data/Shaders/directionalLight.frag");
	spotlightShader = LoadShader("Data/Shaders/spotlight.vert", "Data/Shaders/spotlight.frag");
	ambientLightShader = LoadShader("Data/Shaders/ambientLight.vert", "Data/Shaders/ambientLight.frag");

	// Normal mapping shaders
	normalMap_AmbientShader = LoadShader("Data/Shaders/NormalMapping/normalMapping_ambient_light.vert", "Data/Shaders/NormalMapping/normalMapping_ambient_light.frag");
	normalMap_DirectionalShader = LoadShader("Data/Shaders/NormalMapping/normalMapping_directional_light.vert", "Data/Shaders/NormalMapping/normalMapping_directional_light.frag");
	normalMap_SpotlightsShader = LoadShader("Data/Shaders/NormalMapping/normalMapping_spotlights.vert", "Data/Shaders/NormalMapping/normalMapping_spotlights.frag");
	
	normalMap_DirectionalShader->SetUniform("baseMap",0);
	normalMap_DirectionalShader->SetUniform("normalMap",1);

	normalMap_SpotlightsShader->SetUniform("baseMap",0);
	normalMap_SpotlightsShader->SetUniform("normalMap",1);

	// Displacement mapping terrain shaders
	terrainShaders.Terrain_Displacement_Ambient_Shader = LoadShader("Data/Shaders/TerrainShaders/displacement_ambient.vert", "Data/Shaders/TerrainShaders/displacement_ambient.frag");
	terrainShaders.Terrain_Displacement_Directional_Shader = LoadShader("Data/Shaders/TerrainShaders/displacement_directional.vert", "Data/Shaders/TerrainShaders/displacement_directional.frag");
	terrainShaders.Terrain_Displacement_Spotlights_Shader = LoadShader("Data/Shaders/TerrainShaders/displacement_spotlights.vert", "Data/Shaders/TerrainShaders/displacement_spotlights.frag");

	ShaderHandle displacementAmbientShader = terrainShaders.Terrain_Displacement_Ambient_Shader,
		displacementDirectionalShader = terrainShaders.Terrain_Displacement_Directional_Shader,
		displacementSpotlightsShader = terrainShaders.Terrain_Displacement_Spotlights_Shader;

	displacementAmbientShader->SetUniform("colorMap", 0); displacementAmbientShader->SetUniform("snowMap", 1);
	displacementAmbientShader->SetUniform("normalMap", 2); displacementAmbientShader->SetUniform("displacementMap", 3);

	displacementDirectionalShader->SetUniform("colorMap", 0); displacementDirectionalShader->SetUniform("snowMap", 1);
	displacementDirectionalShader->SetUniform("normalMap", 2); displacementDirectionalShader->SetUniform("displacementMap", 3);
	
	displacementSpotlightsShader->SetUniform("colorMap", 0); displacementSpotlightsShader->SetUniform("snowMap", 1);
	displacementSpotlightsShader->SetUniform("normalMap", 2); displacementSpotlightsShader->SetUniform("displacementMap", 3);
	displacementSpotlightsShader->SetUniform("vposmult", 0);

	// Load all the tree shaders
	treeShaders.Tree_Ambient_Shader = LoadShader("Data/Shaders/TreeShaders/ambientLight.vert", "Data/Shaders/TreeShaders/ambientLight.frag");
	treeShaders.FlatNonTextured_Directional_Shader = LoadShader("Data/Shaders/TreeShaders/FlatShading/directionalLight.vert", "Data/Shaders/TreeShaders/FlatShading/directionalLight.frag");
	treeShaders.SmoothShaded_Spot_Shader = LoadShader("Data/Shaders/TreeShaders/FlatShading/spotlight.vert", "Data/Shaders/TreeShaders/FlatShading/spotlight.frag");
	treeShaders.SmoothShaded_Directional_Shader = LoadShader("Data/Shaders/TreeShaders/SmoothShading/directionalLight.vert", "Data/Shaders/TreeShaders/SmoothShading/directionalLight.frag");
	treeShaders.SmoothShaded_Spot_Shader = LoadShader("Data/Shaders/TreeShaders/SmoothShading/spotlight.vert", "Data/Shaders/TreeShaders/SmoothShading/spotlight.frag");

	return true;
};

bool World::LoadParticles()
{
	particleSystem.SetDefaultModel(CreateBillboardModel());
	defaultBillboardModel = particleSystem.GetDefaultModel();

	ShaderHandle psysbase = particleSystemBaseShader;

	//const u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;

	u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;
	conf.GetInt("LeavesPerLeafMatrix", (i32&)LEAF_PARTICLES_PER_LEAF_MATRIX);

	leafParticleEmitterID = particleSystem.AddEmitter<StaticParticleEmitter>();
	StaticParticleEmitter *leafEmitter = particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID);
	leafEmitter->SetLocalParticleMaximum(tree->GetLeafCount() * LEAF_PARTICLES_PER_LEAF_MATRIX);
	leafEmitter->SetParticlesStaticState(true);
	leafEmitter->SetAlphaMap(leafTexture);
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
	i32 smokeRateOfEmission = 250; conf.GetInt("SmokeRateOfEmission", smokeRateOfEmission);
	smokeParticleEmitter->SetRateOfEmission(smokeRateOfEmission);
	smokeParticleEmitter->SetAlphaMap(particleTexture);
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
	snowEmitter->SetAlphaMap(particleTexture);
	i32 maxsnowparticles=150; conf.GetInt("MaxSnowParticles", maxsnowparticles);
	snowEmitter->SetLocalParticleMaximum(abs(maxsnowparticles));
	snowEmitter->SetHemiSphereRadius(globeSphere.GetRadius() - (globeSphere.GetRadius() * 0.048f));
	snowEmitter->SetEmitterOrigin(float3(0.2f,0,0));
	snowEmitter->SetShader(psysbase);
	snowEmitter->SetBillboardType(Spherical);
	snowEmitter->SetActive(false);
	snowEmitter->SetRateOfEmission(500);

	u32 fireParticleEmitterID = particleSystem.AddEmitter<FireParticleEmitter>();
	fireParticleEmitter = particleSystem.GetEmitter<FireParticleEmitter>(fireParticleEmitterID);
	fireParticleEmitter->SetLocalParticleMaximum(30000);
	fireParticleEmitter->SetAlphaMap(particleTexture);
	fireParticleEmitter->SetShader(psysbase);
	fireParticleEmitter->SetBillboardType(Spherical);
	fireParticleEmitter->SetEmitterOrigin(tree->GetPosition());
	fireParticleEmitter->SetSourceAlphaBlendFunction(GL_ONE);
	fireParticleEmitter->SetTree(tree);
	std::vector<ParticleLine> fire_particle_lines;
	tree->CalculateParticleLines(fire_particle_lines);

	for(u32 i=0;i<fire_particle_lines.size();++i)
	{
		fireParticleEmitter->AddLine(fire_particle_lines[i]);
	}
	fireParticleEmitter->SetActive(false);
	fireParticleEmitter->SetRateOfEmission(-1);

	grassStaticEmitterID = particleSystem.AddEmitter<StaticParticleEmitter>();
	StaticParticleEmitter *grassParticles = particleSystem.GetEmitter<StaticParticleEmitter>(grassStaticEmitterID);

	i32 grassLocalParticleMax = 150;
	conf.GetInt("GrassParticleCount", grassLocalParticleMax);
	grassParticles->SetLocalParticleMaximum(grassLocalParticleMax);
	
	grassParticles->SetModel(CreateImposterModel());
	imposterModel = grassParticles->GetModel();
	grassParticles->SetAlphaMap(grassParticleTexture);
	grassParticles->SetColorMap(grassParticleColorMap);
	grassParticles->SetParticlesStaticState(true);
	grassParticles->SetShader(texturedParticleShader);
	grassParticles->SetBillboardType(NoBillboarding);
	grassParticles->SetEmitterOrigin(float3(0,0.15f,0));
	grassParticles->SetSourceAlphaBlendFunction(GL_ONE_MINUS_SRC_ALPHA);

	f32 radius = globeSphere.GetRadius()-0.95f;

	i32 grass_particles_count=150;
	conf.GetInt("GrassParticleCount", grass_particles_count);
	if(grass_particles_count < 0) { grass_particles_count = 150; }

	const float X_FUDGE = 0.35f;

	for(int i=0;i<grass_particles_count;++i)
	{
		f32 ZAxis = randflt(-radius, radius);
		f32 phi = randflt(0,PI);
		f32 theta = asin(ZAxis/radius);

		f32 xpos = (radius * cos(theta) * cos(phi)) + X_FUDGE;
		f32 zpos = radius * sin(theta);

		if( (xpos>(-0.3) && xpos<5.1) && (zpos>1.15 && zpos<5.3) ) // dont generate particles on the pond
		{
			i--;
			continue;
		}

		Particle p;
		p.color = float4(1,1,1,1);
		p.pos = float3(xpos, 0, zpos);
		p.size = float3(0.4f);
		
		grassParticles->AddParticle(p);
	}

	return true;
};

bool World::LoadGeometry()
{
	// Load house model
	{
		vector<GraphicsObject> objVec;
		OBJLoader::LoadOBJFile("Data/House/Haus20.obj", objVec);
		houseModel = objVec[0];
		houseModel.SetScale(float3(.05f,.05f,.05f));
		houseModel.SetPosition(float3(-5.5,0,0.5));
		houseModel.AddTexture(houseTexture);
		houseModel.AddTexture(houseNormalMap);
	}

	// Load globe base
	{
		vector<GraphicsObject> objVec;
		OBJLoader::LoadOBJFile("Data/base2.obj", objVec);
		baseModel = objVec[0];
		baseModel.AddTexture(baseTexture);
		baseModel.SetPosition(float3(0, 0.2f, 0));
		baseModel.SetScale(float3(2.0f, 0.45f, 2.0f));
	}

	// Load bolt model
	{
		vector<GraphicsObject> objVec;
		OBJLoader::LoadOBJFile("Data/bolt.obj", objVec);
		boltModel = objVec[0];
		
		boltModel.GetMaterial().SetAmbientDiffuseSpecularEmissive(color(0.0f), color(1.0f, 0.8f, 0.0f, 0.0f), color(1.0f));
		boltModel.GetMaterial().SetShininess(20);
	}

	// Load globe
	f32 radius=11.3f; conf.GetFloat("GlobeRadius",radius);
	globeSphere.Create(radius, 40, 40);
	globeSphere.SetPosition(float3(0.3f,0,0));

	// Load terrain
	terrain.Load("Data/Textures/ground_heightmap.bmp");
	terrain.AddTexture(grassTexture);
	terrain.AddTexture(displacementTexture);
	terrain.AddTexture(snowTexture);
	terrain.AddTexture(terrainNormalMapFull);
	terrain.SetXRotation(-90);
	terrain.SetZRotation(-90);
	terrain.SetScale(float3(5.48f,5.48f,5.48f));

	terrain.GetMaterial().SetAmbientDiffuseSpecularEmissive(color(0.5f), color(0.95f), color(0.3f));
	terrain.GetMaterial().SetShininess(50);

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
	tree->SetActive(false);

	SetLightingMode(Spotlights); // default to directional lights

	globeSphere.SetShader(globeShader);

	lightSphere.Create(0.5,20,20);
	lightSphere.AddTexture(barkTexture);

	spotCone.Create(0.1f, 0.3f, 0.45f, 10, 10);
	spotCone.AddTexture(baseTexture);
	spotCone.SetPosition(spotlights[0].GetPosition().ToFloat3());

	SetTreeShadeMode(SmoothTextured);

	return true;
};

void World::LoadLights()
{
	// Directional Light
	float4 directionalLightAmb(0.35f,0.35f,0.35f,1), directionLightDiffuse(1,0.66666f,0.19607f,1), directionalLightSpecular(1,1,1,1), lightPosition;
	directionalLight = Light(lightPosition,directionalLightAmb, directionLightDiffuse, directionalLightSpecular);
	directionalLight.SetLightID(GL_LIGHT0);
	
	float4 spotDir;
	float4 spotDiff(1,0.66666f,0.19607f,1);
	//float4 spotSpec(0.3f,0.3f,0.3f,1);

	float4 spotAmb = Color::WHITE * 0.2f;
	float4 spotSpec = Color::WHITE * 0.2f; 

	// Spotlight 1 (top) 
	lightPosition = float4(0,5,0,1); spotDir = float4(0,-1,0,1);  
	spotDiff = Color::GREY*0.55f;
	spotlights[0] = Light(lightPosition, spotDir, 10, spotAmb, spotDiff, spotSpec);
	spotlights[0].SetLightID(GL_LIGHT0); 
	 
	// Spotlight 2 (left)
	lightPosition = float4(-5.3f, 0.6f, 9.6f,1);  spotDir = float4(0,-0.5f,-1,1);
	spotDiff = float4(0.5f,0.6f,0.2f,1);
	spotlights[1] = Light(lightPosition, spotDir, 32, spotAmb, spotDiff, spotSpec);
	spotlights[1].SetLightID(GL_LIGHT1);
	 
	// Spotlight 3 (right)
	lightPosition = float4(4.4f, 0.6f, 10, 1); spotDir = float4(0,-0.5,-1,1);
	spotDiff = float4(0.8f,0.45f,0.3f,1);
	spotlights[2] = Light(lightPosition, spotDir, 32, spotAmb, spotDiff, spotSpec);
	spotlights[2].SetLightID(GL_LIGHT2);
	 
	// Spotlight 4 (back) 
	lightPosition = float4(0,0.6f,-11.2f,1); spotDir = float4(0,0,1,1);
	spotAmb = float4(0.35f,0.35f,0.35f,1);
	spotDiff = float4(1,0.66666f,0.19607f,1); 
	spotlights[3] = Light(lightPosition, spotDir, 33, spotAmb, spotDiff, spotSpec);
	spotlights[3].SetLightID(GL_LIGHT3);
};

void World::Shutdown()
{
	delete tree;
	ResourceManager::get().Cleanup();
};