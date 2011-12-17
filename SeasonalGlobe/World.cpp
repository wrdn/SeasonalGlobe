#include "World.h"
#include "strutils.h"
#include "Color.h"
#include "PerfTimer.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "CylindricalParticleEmitter.h"
#include "tutorialcodeheaders.h"
#include "SeasonManager.h"

World::World(void)
	: conf(), // Application configuration

	texMan(), shaderMan(), particleSystem(), seasonMan(), // Managers
	dtMultiplier(1),

	cam(), sceneRotationAxis(0,1,0), _cameraAngle(30.0f), _cameraPosition(-30.0f), // Camera
	_cameraRotation(-357.0f), AutoRotate(false), snowSlowing(false),

	terrain(0), terrainElevation(0.45f, 0.13f, 3.0f), tree(0), globeSphere(0), houseModel(0), // Geometry
	baseModel(0), boltModel(0), defaultBillboardModel(0), imposterModel(0), polygonMode(GL_FILL),lightSphere(0), spotCone(0),
	drawLightning(false),

	phongShaderID(0), particleSystemBaseShaderID(0), texturedParticleShaderID(0), globeShaderID(0), directionalLightShaderID(0), // Shaders
	multiTexturingSampleShaderID(0), spotlightShaderID(0), ambientLightShaderID(0), 
	normalMap_Ambient_ShaderID(0), normalMap_Directional_ShaderID(0), normalMap_Spotlights_ShaderID(0),
	treeShaders(), terrainShaders(), terrainTextureMergeFactor(0), mergingTerrainTextured(false),
	timeToMergeTextures(8), terrainTextureMergeRuntime(0), mergeDirection(1),

	grassTexture(0), houseTexture(0), barkTexture(0), particleTexture(0), leafTexture(0), baseTexture(0), // Textures
	displacementTexture(0), barkNormalMap(0), snowTexture(0), terrainNormalMapFull(0), grassParticleTexture(0),
	grassParticleColorMap(0), houseNormalMap(0),

	leafParticleEmitterID(0), snowEmitterID(0), smokeEmitterID(0), grassStaticEmitterID(0) , fireParticleEmitter(0), // Particle Emitters

	directionalLight(), directionalLightRotation(0), directionalLightSpeed(0.5f), lightMode(Directional) // Lights
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

Model* CreateImposterModel()
{
	Model *imposterModel = new Model();
	
	VERTEX *imposterVerts = new VERTEX[8];

	// first quad
	imposterVerts[0] = VERTEX(float3(-1,1,0),  float3(0,0,1), float2(0,1));
	imposterVerts[1] = VERTEX(float3(1,1,0),   float3(0,0,1), float2(1,1));
	imposterVerts[2] = VERTEX(float3(1,-1,0),  float3(0,0,1), float2(1,0));
	imposterVerts[3] = VERTEX(float3(-1,-1,0), float3(0,0,1), float2(0,0));

	// second quad
	imposterVerts[4] = VERTEX(float3(0,1,-1),  float3(1,0,0), float2(0,1));
	imposterVerts[5] = VERTEX(float3(0,1,1),   float3(1,0,0), float2(1,1));
	imposterVerts[6] = VERTEX(float3(0,-1,1),  float3(1,0,0), float2(1,0));
	imposterVerts[7] = VERTEX(float3(0,-1,-1), float3(1,0,0), float2(0,0));
	
	imposterModel->SetVertexArray(imposterVerts, 8);
	
	u32 *imposterIndices = new u32[16];
	imposterIndices[0] = 1;
	imposterIndices[1] = 0;
	imposterIndices[2] = 3;
	imposterIndices[3] = 1;
	imposterIndices[4] = 3;
	imposterIndices[5] = 2;
	imposterIndices[6] = 5;
	imposterIndices[7] = 4;
	imposterIndices[8] = 7;
	imposterIndices[9] = 5;
	imposterIndices[10] = 7;
	imposterIndices[11] = 6;

	imposterModel->SetIndicesArray(imposterIndices, 16);

	imposterModel->BuildVBO();
	return imposterModel;
};

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
	grassTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	grassParticleTexture = texMan.LoadTextureFromFile("Data/Textures/grass2.tga");
	grassParticleTexture->SetTextureSlot(SLOT_GL_TEXTURE_0);

	grassParticleColorMap = texMan.LoadTextureFromFile("Data/Textures/grassParticleTexture.bmp");
	grassParticleColorMap->SetTextureSlot(SLOT_GL_TEXTURE_1);

	snowTexture = texMan.LoadTextureFromFile("Data/Textures/snowTexture.jpg");
	snowTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR );
	snowTexture->SetMagFilter(GL_LINEAR_MIPMAP_LINEAR );
	snowTexture->SetWrapS(GL_REPEAT);
	snowTexture->SetWrapT(GL_REPEAT);
	snowTexture->SetTextureSlot(SLOT_GL_TEXTURE_1);

	terrainNormalMapFull = texMan.LoadTextureFromFile("Data/Textures/TerrainDisplacementNormalMap_Full.bmp");
	terrainNormalMapFull->SetTextureSlot(SLOT_GL_TEXTURE_2);

	houseNormalMap = texMan.LoadTextureFromFile("Data/House/Haus_020_unwrap_NRM.jpg");
	houseNormalMap->SetTextureSlot(SLOT_GL_TEXTURE_1);

	displacementTexture = texMan.LoadTextureFromFile("Data/Textures/TerrainDisplacementMap.bmp");
	displacementTexture->SetTextureSlot(SLOT_GL_TEXTURE_3);

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
	
	//texturedParticleShaderID
	if(LoadShader(texturedParticleShaderID, "Data/Shaders/texturedParticle.frag", "Data/Shaders/texturedParticle.vert"))
	{
		shaderMan.GetShader(texturedParticleShaderID)->Activate();
		shaderMan.GetShader(texturedParticleShaderID)->SetUniform("AlphaMap", 0);
		shaderMan.GetShader(texturedParticleShaderID)->SetUniform("ColorMap", 1);
		shaderMan.GetShader(texturedParticleShaderID)->Deactivate();
	}

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

	shaderMan.GetShader(normalMap_Spotlights_ShaderID)->Activate();
	shaderMan.GetShader(normalMap_Spotlights_ShaderID)->SetUniform("baseMap",0);
	shaderMan.GetShader(normalMap_Spotlights_ShaderID)->SetUniform("normalMap",1);
	shaderMan.GetShader(normalMap_Spotlights_ShaderID)->Deactivate();


	// Displacement mapping terrain shaders
	LoadShader(terrainShaders.Terrain_Displacement_Ambient_ShaderID, 
		"Data/Shaders/TerrainShaders/displacement_ambient.frag", "Data/Shaders/TerrainShaders/displacement_ambient.vert");
	LoadShader(terrainShaders.Terrain_Displacement_Directional_ShaderID, 
		"Data/Shaders/TerrainShaders/displacement_directional.frag", "Data/Shaders/TerrainShaders/displacement_directional.vert");
	LoadShader(terrainShaders.Terrain_Displacement_Spotlights_ShaderID, 
		"Data/Shaders/TerrainShaders/displacement_spotlights.frag", "Data/Shaders/TerrainShaders/displacement_spotlights.vert");

	Shader *displacementAmbientShader = shaderMan.GetShader(terrainShaders.Terrain_Displacement_Ambient_ShaderID);
	Shader *displacementDirectionalShader = shaderMan.GetShader(terrainShaders.Terrain_Displacement_Directional_ShaderID);
	Shader *displacementSpotlightsShader = shaderMan.GetShader(terrainShaders.Terrain_Displacement_Spotlights_ShaderID);

	displacementAmbientShader->Activate();
	displacementAmbientShader->SetUniform("colorMap", 0);
	displacementAmbientShader->SetUniform("snowMap", 1);
	displacementAmbientShader->SetUniform("normalMap", 2);
	displacementAmbientShader->SetUniform("displacementMap", 3);
	displacementAmbientShader->Deactivate();

	displacementDirectionalShader->Activate();
	displacementDirectionalShader->SetUniform("colorMap", 0);
	displacementDirectionalShader->SetUniform("snowMap", 1);
	displacementDirectionalShader->SetUniform("normalMap", 2);
	displacementDirectionalShader->SetUniform("displacementMap", 3);
	displacementDirectionalShader->Deactivate();

	displacementSpotlightsShader->Activate();
	displacementSpotlightsShader->SetUniform("colorMap", 0);
	displacementSpotlightsShader->SetUniform("snowMap", 1);
	displacementSpotlightsShader->SetUniform("normalMap", 2);
	displacementSpotlightsShader->SetUniform("displacementMap", 3);
	displacementSpotlightsShader->Deactivate();

	/*	terrain->GetShader()->SetUniform("colorMap", 0);
	terrain->GetShader()->SetUniform("snowMap", 1);
	terrain->GetShader()->SetUniform("normalMap", 2);
	terrain->GetShader()->SetUniform("displacementMap", 3);*/

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
	particleSystem.SetDefaultModel(CreateBillboardModel());
	defaultBillboardModel = particleSystem.GetDefaultModel();

	Shader *psysbase = shaderMan.GetShader(particleSystemBaseShaderID);

	//const u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;

	u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;
	conf.GetInt("LeavesPerLeafMatrix", (i32&)LEAF_PARTICLES_PER_LEAF_MATRIX);

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
	i32 smokeRateOfEmission = 250; conf.GetInt("SmokeRateOfEmission", smokeRateOfEmission);
	smokeParticleEmitter->SetRateOfEmission(smokeRateOfEmission);
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
	snowEmitter->SetHemiSphereRadius(globeSphere->GetRadius() - (globeSphere->GetRadius() * 0.048f));
	snowEmitter->SetEmitterOrigin(float3(0.2f,0,0));
	snowEmitter->SetShader(psysbase);
	snowEmitter->SetBillboardType(Spherical);
	snowEmitter->SetActive(false);
	snowEmitter->SetRateOfEmission(500);

	u32 fireParticleEmitterID = particleSystem.AddEmitter<FireParticleEmitter>();
	fireParticleEmitter = particleSystem.GetEmitter<FireParticleEmitter>(fireParticleEmitterID);
	fireParticleEmitter->SetLocalParticleMaximum(30000);
	fireParticleEmitter->SetAlphaMap(*particleTexture);
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
	imposterModel = (Model*)grassParticles->GetModel();
	grassParticles->SetAlphaMap(*grassParticleTexture);
	grassParticles->SetParticlesStaticState(true);
	grassParticles->SetShader(shaderMan.GetShader(texturedParticleShaderID));
	grassParticles->SetColorMap(grassParticleColorMap);
	grassParticles->SetBillboardType(NoBillboarding);
	grassParticles->SetEmitterOrigin(float3(0,0,0));
	grassParticles->SetSourceAlphaBlendFunction(GL_ONE);

	f32 radius = globeSphere->GetRadius()-0.3f;
	for(int i=0;i<150;++i)
	{
		f32 ZAxis = randflt(-radius, radius);
		f32 phi = randflt(0,PI);
		f32 theta = asin(ZAxis/radius);

		f32 xpos = radius * cos(theta) * cos(phi);
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
	houseModel = OBJFile::ParseFile("Data/House/Haus20.obj")[0];
	((Model&)houseModel->GetModel()).BuildVBO();
	houseModel->SetScale(float3(.05f,.05f,.05f));
	houseModel->SetPosition(float3(-5.5,0,0.5));
	houseModel->AddTexture(houseTexture);
	houseModel->AddTexture(houseNormalMap);

	//houseModel->SetTextureA(houseTexture);
	//Material mat(float4(0.25f, 0.25f, 0.25f, 1), float4(1,0,0,1), float4(1,1,1,1), 100);
	//houseModel->SetMaterial(mat);

	// Load globe base
	baseModel = OBJFile::ParseFile("Data/base2.obj")[0];
	((Model&)baseModel->GetModel()).BuildVBO();
	//baseModel->SetTextureA(baseTexture);
	baseModel->AddTexture(baseTexture);
	baseModel->SetPosition(float3(0, 0.2f, 0));
	baseModel->SetScale(float3(2.0f, 0.45f, 2.0f));

	boltModel = OBJFile::ParseFile("Data/bolt.obj")[0];
	((Model&)boltModel->GetModel()).BuildVBO();
	boltModel->SetMaterial(Material(float4(0,0,0), float4(1,0.8f,0,0), float4(1,1,1,1), 20));

	// Load globe
	globeSphere = new Sphere();
	f32 radius=11.3f; conf.GetFloat("GlobeRadius",radius);
	globeSphere->Create(radius, 40, 40);
	globeSphere->SetPosition(float3(0.3f,0,0));

	// Load terrain
	terrain = new TerrainLoader();
	terrain->Load("Data/Textures/ground_heightmap.bmp");
	terrain->AddTexture(grassTexture);
	terrain->AddTexture(displacementTexture);
	terrain->AddTexture(snowTexture);
	terrain->AddTexture(terrainNormalMapFull);
	terrain->SetXRotation(-90);
	terrain->SetZRotation(-90);
	terrain->SetScale(float3(5.48f,5.48f,5.48f));
	terrain->SetMaterial(Material(float4(0.5f), float4(0.95f), float4(0.3f), 50));

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
	//lightSphere->SetTextureA(barkTexture);
	lightSphere->AddTexture(barkTexture);

	spotCone = new Cylinder();
	spotCone->Create(0.1f, 0.3f, 0.45f, 10, 10);
	//spotCone->SetTextureA(baseTexture);
	spotCone->AddTexture(baseTexture);
	spotCone->SetPosition(spotlights[0].GetPosition().ToFloat3());

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
	float4 spotSpec(0.3f,0.3f,0.3f,1); float4 spotDir;

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

void StartTreeGrowth(const World *w)
{
	World *wt = (World*)w;
	wt->GetTree()->SetActive(true);
	wt->GetTree()->InitGrow();
};
void InitiateLeafGrowth(const World *w)
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->SetActive(true);
	wt->GetLeafParticleEmitter()->SetStartColor(Color4f(1,1,1,1));
	wt->GetLeafParticleEmitter()->InitiateParticleFadeIn();
};
void InitiateLeafColorChange(const World *w)
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->SetEndColor(Color4f(0.8f, 0.35f, 0.35f, 1));
	wt->GetLeafParticleEmitter()->InitiateMainColorChange();
};
void InitiateLeafFall(const World *w)
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->InitiateParticleFall();
};
void InitiateTreeIgnitionFire(const World *w)
{
	FireParticleEmitter *fp = (FireParticleEmitter*)w->GetFireParticleEmitter();
	fp->SetRuntime(0);
	fp->SetBurnLevel(0);
	fp->SetBurningState(Igniting);
	fp->SetActive(true);
};
void InitiateTreeDeath(const World *w)
{
	FireParticleEmitter *fp = (FireParticleEmitter*)w->GetFireParticleEmitter();
	fp->InitDeath();
};
void InitiateLeafVanish(const World *w)
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->InitiateParticleFadeOut();
}
void InitialiseHouseSmoke(const World *w)
{
	((World*)w)->ActivateSmokeParticleEffect();
};
void InitialiseSnow(const World *w)
{
	((World*)w)->ActivateSnow();
};
void InitialiseTerrainElevation(const World *w) { ((World*)w)->ActivateTerrainElevation(Up); }
void InitialiseTerrainMelt(const World *w) { ((World*)w)->ActivateTerrainElevation(Down); }
void InitialiseTerrainTextureMergeToSnow(const World *w) { ((World*)w)->ActiveTerrainTextureMerge(1); }
void InitialiseTerrainTextureMergeToGrass(const World *w) { ((World*)w)->ActiveTerrainTextureMerge(-1); }
void InitLightningAppear(const World *w) { ((World*)w)->SetLightningActive(true); }
void InitLightningVanish(const World *w) { ((World*)w)->SetLightningActive(false); }
void DeactivateWorldSnow(const World *w) { ((World*)w)->DeactivateSnow(); };
void ActivateSnowSlowing(const World *w) { ((World*)w)->SetSnowSlowing(true); }

void World::SetupSeasons()
{
	i32 timePerSeasons = 16;
	conf.GetInt("TimePerSeason", timePerSeasons);

	seasonMan.SetTimePerSeason((f32)timePerSeasons); // 8 seconds per season, 32 seconds per cycle
	seasonMan.SetWorldPointer(this);

	// Spring: Tree grows
	seasonMan.AddEvent(Spring, SeasonalEvent(0.1f, StartTreeGrowth)); // tree grows at start of spring

	// Summer: Leaves appear on tree
	seasonMan.AddEvent(Summer, SeasonalEvent(0.0f, InitiateLeafGrowth));

	// Autumn: Leaves change colour and fall from tree (then disappear over time), lightning bolt hits tree and it burns down
	seasonMan.AddEvent(Autumn, SeasonalEvent(0, InitiateLeafColorChange));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.2f, InitiateLeafFall));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.3f, InitiateLeafVanish));
	
	seasonMan.AddEvent(Autumn, SeasonalEvent(0.29f, InitLightningAppear));
	seasonMan.AddEvent(Autumn, SeasonalEvent(0.3f, InitLightningVanish));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.3f, InitiateTreeIgnitionFire));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.78f, InitiateTreeDeath));

	// Winter
	seasonMan.AddEvent(Winter, SeasonalEvent(0, InitialiseSnow ));
	seasonMan.AddEvent(Winter, SeasonalEvent(0.05f, InitialiseHouseSmoke));
	
	seasonMan.AddEvent(Winter, SeasonalEvent(0.3f, InitialiseTerrainTextureMergeToSnow));
	
	seasonMan.AddEvent(Winter, SeasonalEvent(0.45f, InitialiseTerrainElevation));
	
	seasonMan.AddEvent(Winter, SeasonalEvent(0.8f, InitialiseTerrainMelt));
	seasonMan.AddEvent(Winter, SeasonalEvent(0.85f, InitialiseTerrainTextureMergeToGrass));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.8f, ActivateSnowSlowing));
	
	seasonMan.AddEvent(Winter, SeasonalEvent(0.9f, DeactivateWorldSnow));

	UpdateSceneTimings();
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
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ColorT::black().GetVec());

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
	delete terrain;
	delete defaultBillboardModel;
	delete imposterModel;
	delete boltModel;
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
	glDisable(GL_TEXTURE_2D);
	glTranslatef(floorPos.x(), floorPos.y(), floorPos.z());
	glScalef(floorScale.x(), floorScale.y(), floorScale.z());
	floor(); // draw floor into stencil
	glEnable(GL_TEXTURE_2D);
};

void World::reflective_draw(const GameTime &gameTime)
{
	float3 floorScale(0.3f, 0.3f, 0.3f);
	float3 floorPos(2.5f, -0.45f, 3.2f);
	
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
	
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	fireParticleEmitter->Draw();
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->Draw();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	glDisable(GL_NORMALIZE);
	glCullFace(GL_BACK);

	glPopMatrix();

	glDisable(GL_STENCIL_TEST);

	glEnable(GL_LIGHTING);
	
	glColor4f(1,1,1,1);
	glDisable(GL_TEXTURE_2D);
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

	DrawTerrain(gameTime);
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

void World::Update(const GameTime &_gameTime)
{
	GameTime gameTime = _gameTime;
	gameTime.SetDeltaTime(gameTime.GetDeltaTime() * dtMultiplier);

	UpdateSceneTimings();

	if(mergingTerrainTextured)
	{
		terrainTextureMergeRuntime += gameTime.GetDeltaTime() * mergeDirection;
		if(terrainTextureMergeRuntime > timeToMergeTextures)
		{
			terrainTextureMergeRuntime = timeToMergeTextures - EPSILON;
		}
		else if(terrainTextureMergeRuntime <= 0)
		{
			terrainTextureMergeRuntime = 0;
		}
		terrainTextureMergeRuntime = fmod(terrainTextureMergeRuntime, timeToMergeTextures);
		terrainTextureMergeFactor = (1.0f / timeToMergeTextures) * terrainTextureMergeRuntime;
	}

	HemiSphericalParticleEmitter *snowEmitter = particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID);
	if(snowEmitter->IsActive())
	{
		if(snowEmitter->GetActivity() < 1.0f && !snowSlowing)
		{
			snowEmitter->SetActivity(snowEmitter->GetActivity() + 0.005f);
		}
		else if(snowSlowing && snowEmitter->GetActivity()>0)
		{
			snowEmitter->SetActivity(snowEmitter->GetActivity() - 0.005f);
		}
	}

	particleSystem.Update(gameTime);
	fireParticleEmitter->UpdateFireParticleEmitter(gameTime);
};

void World::DrawTerrain(const GameTime &gameTime)
{
	// Terrain (floor)
	terrainElevation.elevationRuntime += gameTime.GetDeltaTime() * terrainElevation.shiftDirection;
	if(terrainElevation.elevationRuntime <= 0 && terrainElevation.shiftDirection == Down) { terrainElevation.elevationRuntime=0; terrainElevation.shiftDirection = NoShift; }

	terrainElevation.elevationRuntime = min(terrainElevation.elevationRuntime, terrainElevation.timeToElevateFully); // or =fmod(CurrentTerrainTime, ShiftTime); to loop terrain
	f32 N = (1.0f / terrainElevation.timeToElevateFully) * terrainElevation.elevationRuntime;
	f32 vpos = lerp(0, terrainElevation.maxDisplacementScaleFactor, N);
	f32 CurrentShift = lerp(0, terrainElevation.maxGeometryShiftFactor, N);

	glDisable(GL_CULL_FACE);
	terrain->SetPosition(float3(0, terrain->GetPosition().y()-CurrentShift,0));
	terrain->GetShader()->Activate();
	terrain->GetShader()->SetUniform("t", terrainTextureMergeFactor);
	terrain->GetShader()->SetUniform("normalLerpFactor", N);
	terrain->GetShader()->SetUniform("vposmult", vpos);
	terrain->GetShader()->Deactivate();
	terrain->SetXRotation(-90);
	terrain->SetYRotation(0);
	terrain->SetZRotation(-90);
	terrain->Draw();
	terrain->SetPosition(float3(0, terrain->GetPosition().y()+CurrentShift,0));
	glEnable(GL_CULL_FACE);
};

void World::Draw(const GameTime &_gameTime)
{
	GameTime gameTime = _gameTime;
	gameTime.SetDeltaTime(gameTime.GetDeltaTime() * dtMultiplier);

	seasonMan.Update(gameTime.GetDeltaTime());

	glLoadIdentity();

	glPushMatrix();

	// This translates everything else in the scene, objects move, not the camera
	glTranslatef(0.0f, 0.0f,_cameraPosition);
	glRotatef(_cameraAngle, 1.0,0.0,0.0);
	glRotatef(_cameraRotation, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, -1.0f,0.0f);

	glRotatef(angle, 0, 1, 0);
	
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
		float3 newPos(pos.x()+radius*sin(directionalLightRotation), pos.y()+radius*cos(directionalLightRotation), pos.z());
		lightSphere->SetPosition(newPos);
		float4 lightSpherePos = lightSphere->GetPosition().ToFloat4();
		directionalLight.SetPositionAndUpdateOpenGL(lightSpherePos);
		directionalLightRotation += directionalLightSpeed * gameTime.GetDeltaTime();
		lightSphere->Draw();
	}

	if(drawLightning)
	{
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glTranslatef(tree->GetPosition().x(), tree->GetPosition().y(), tree->GetPosition().z());
		glTranslatef(-0.25,5,0);
		glScalef(2,2,2);
		glColor4f(0.9451f, 0.9178f, 0,1);
		boltModel->Draw();
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glColor4f(1,1,1,1);
	}

	//movableSphere->Draw();

	glPushMatrix();
	reflective_draw(gameTime);
	glPopMatrix();

	// House
	houseModel->Draw();

	// Base
	baseModel->Draw();

	tree->Draw(gameTime.GetDeltaTime());

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

	void World::ResetWorld()
	{
		seasonMan.Reset();

		// RESET TREE
		tree->SetRuntime(0);
		tree->SetTreeDieing(false);
		tree->SetTreeDeathDepth(0);
		tree->SetActive(false);

		snowSlowing = false;

		// RESET LEAVES
		const u32 LEAF_PARTICLES_PER_LEAF_MATRIX = 3;
		const Mat44 *LeafMatrices = tree->GetLeafMatrices();
		const u32 LeafCount = tree->GetLeafCount();
		StaticParticleEmitter *leafEmitter = particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID);
		leafEmitter->ResetParticleEmitter();
		for(u32 i=0;i<LeafCount;++i)
		{
			glMatrixMode(GL_MODELVIEW);
			const f32 *leafMat = LeafMatrices[i].GetMatrix();
			for(u32 j=0;j<LEAF_PARTICLES_PER_LEAF_MATRIX;++j)
			{
				leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].pos = float3(leafMat[12], leafMat[13], leafMat[14]);
				leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].size = float3(0.25f);
				leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].rotation_z = randflt(0, 360);
				leafEmitter->GetStaticParticles()[ (i*LEAF_PARTICLES_PER_LEAF_MATRIX)+j ].color = leafEmitter->GetStartColor();
			}
		}
		leafEmitter->SetParticlesStaticState(true);
		leafEmitter->SetStartColor(Color4f(1,1,1,0));
		leafEmitter->SetEndColor(Color4f(1,1,1,1));
		leafEmitter->SetParticlesDieing(false);
		leafEmitter->SetParticlesFadingIn(false);
		leafEmitter->SetParticlesFalling(false);
		leafEmitter->DoColorUpdate(true);
		leafEmitter->SetActive(false);

		fireParticleEmitter->SetRuntime(0);
		fireParticleEmitter->SetBurningState(Igniting);
		fireParticleEmitter->SetBurnLevel(0);
		fireParticleEmitter->SetActive(false);

		terrainElevation.shiftDirection = NoShift;

		particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID)->ResetParticleEmitter();
		particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID)->SetActive(false);

		particleSystem.GetEmitter<PointBasedParticleEmitter>(snowEmitterID)->ResetParticleEmitter();
		particleSystem.GetEmitter<PointBasedParticleEmitter>(snowEmitterID)->SetRateOfEmission(500);
		particleSystem.GetEmitter<PointBasedParticleEmitter>(snowEmitterID)->SetActive(false);
	};