#pragma once

#include "Camera.h"
#include "OBJFile.h"
#include "GameTime.h"
#include "TextureManager.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "ShaderManager.h"
#include "FractalTree.h"
#include "AppConfig.h"
#include "ParticleSystem.h"
#include "CylindricalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "FireParticleEmitter.h"
#include "SphericalCamera.h"
#include "Camera2.h"
#include "Light.h"
#include "Disk.h"
#include "TerrainLoader.h"
#include "SeasonManager.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"

enum LightingMode
{
	Ambient,
	Directional,
	Spotlights,
};

struct TreeShaders // collects all the shaders for the tree (directional and spot lights)
{
public:
	u32 FlatNonTextured_Directional_ShaderID,
		FlatNonTextured_Spot_ShaderID,

		// Can use the same ambient shader for smooth and flat as normal not considered
		// Use the global ambient shader
		Tree_Ambient_ShaderID;

	// use a bool in the shader to determine if textures used or not
	u32 SmoothShaded_Directional_ShaderID,
		SmoothShaded_Spot_ShaderID;

	TreeShaders() : FlatNonTextured_Directional_ShaderID(0), FlatNonTextured_Spot_ShaderID(0),
		Tree_Ambient_ShaderID(0), SmoothShaded_Directional_ShaderID(0), SmoothShaded_Spot_ShaderID(0)
	{};
	~TreeShaders() { };
};

struct TerrainShaders
{
public:
	u32 Terrain_Displacement_Ambient_ShaderID;
	u32 Terrain_Displacement_Directional_ShaderID;
	u32 Terrain_Displacement_Spotlights_ShaderID;

	TerrainShaders() : Terrain_Displacement_Ambient_ShaderID(0), Terrain_Displacement_Directional_ShaderID(0),
		Terrain_Displacement_Spotlights_ShaderID(0) {};
	~TerrainShaders() {};
};

enum TerrainShiftDirection { NoShift=0, Up=1, Down=-1 };

struct TerrainShift
{
public:
	f32 maxDisplacementScaleFactor,
		maxGeometryShiftFactor; // translation on Y axis to put terrain back in base
	f32 timeToElevateFully;
	f32 elevationRuntime; // updated by dt each frame
	TerrainShiftDirection shiftDirection;

	TerrainShift() : maxDisplacementScaleFactor(0), maxGeometryShiftFactor(0),
		timeToElevateFully(0), elevationRuntime(0), shiftDirection(NoShift) {};
	TerrainShift(f32 _maxDisplacementScaleFactor, f32 _maxGeometryShiftFactor, f32 _timeToEvalateFully)
		: maxDisplacementScaleFactor(_maxDisplacementScaleFactor), maxGeometryShiftFactor(_maxGeometryShiftFactor),
		timeToElevateFully(_timeToEvalateFully), elevationRuntime(0), shiftDirection(NoShift) {};
	~TerrainShift() {};
};

class World
{
private:
	// Application configuration
	AppConfig conf;

	// Managers
	TextureManager texMan;
	ShaderManager shaderMan;
	ParticleSystem particleSystem;
	SeasonManager seasonMan;

	// Camera
	Camera2 cam;
	float3 sceneRotationAxis;
	f32 _cameraAngle, _cameraPosition, _cameraRotation;
	bool AutoRotate;

	// Geometry
	TerrainLoader *terrain; TerrainShift terrainElevation;
	FractalTree *tree;
	Sphere *globeSphere;
	GraphicsObject *houseModel, *baseModel, *boltModel;
	Model *defaultBillboardModel;
	Model *imposterModel;
	GLenum polygonMode;
	Sphere *lightSphere;
	Cylinder *spotCone;
	bool drawLightning;
	
	// Shaders
	u32 phongShaderID, particleSystemBaseShaderID, texturedParticleShaderID,
		globeShaderID, directionalLightShaderID,
		multiTexturingSampleShaderID, spotlightShaderID,
		ambientLightShaderID,
		normalMap_Ambient_ShaderID, normalMap_Directional_ShaderID, normalMap_Spotlights_ShaderID;
	TreeShaders treeShaders;
	TerrainShaders terrainShaders;

	f32 terrainTextureMergeFactor; // between 0 and 1, controls how much of grass/snow texture shown
	bool mergingTerrainTextured; // merging snow to grass (or vice versa)
	f32 timeToMergeTextures; // time in seconds to switch from grass to snow (and vice versa)
	f32 terrainTextureMergeRuntime; // keep track of how far through the merge process we are
	i32 mergeDirection; // should be 1 or -1

	// Textures
	Texture *grassTexture, *houseTexture, *barkTexture, *particleTexture,
		*leafTexture, *baseTexture, *displacementTexture, *barkNormalMap,
		*snowTexture, *terrainNormalMapFull,
		*grassParticleTexture, // alpha map
		*grassParticleColorMap;

	// Particle emitters
	u32 leafParticleEmitterID;
	u32 snowEmitterID;
	u32 smokeEmitterID;
	u32 grassStaticEmitterID;
	FireParticleEmitter *fireParticleEmitter;

	// Lights
	Light directionalLight; f32 directionalLightRotation; f32 directionalLightSpeed;
	Light spotlights[4];
	LightingMode lightMode; // ACW-switch between Ambient, Directional and 4 spotlights
	
	// Load functions
	bool LoadTextures();

	bool LoadShader(u32 &id, const c8* fragmentShaderFilename, const c8* vertexShaderFilename);
	bool LoadShaders();
	bool LoadParticles();
	bool LoadGeometry();
	void LoadLights();
	void SetupSeasons();

	// Prevent copying
	World(World const& w);
	World& operator= (World const& other);

	void reflective_draw(const GameTime &gameTime);
	void multi_texturing_test(/*const GameTime &gameTime*/);

	void DrawTerrain(const GameTime &gameTime);

	GraphicsObject *test_roof_triangles;

public:

	Sphere *movableSphere;

	const bool GetAutoRotate() const { return AutoRotate; };
	void SetAutoRotate(const bool b) { AutoRotate = b; };
	
	StaticParticleEmitter* GetLeafParticleEmitter() { return particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID); }
	const FireParticleEmitter* GetFireParticleEmitter() const { return fireParticleEmitter; };

	FractalTree * GetTree() const { return tree; };
	const GLenum GetPolygonMode() const { return polygonMode; };
	void SetPolygonMode(const GLenum polyMode)
	{
		polygonMode = polyMode;

		houseModel->SetDrawMode(polyMode);
		globeSphere->SetDrawMode(polyMode);
		terrain->SetDrawMode(polyMode);
		baseModel->SetDrawMode(polyMode);
		tree->SetDrawMode(polyMode);
	};
	
	const f32 GetCameraAngle() const { return _cameraAngle; };
	const f32 GetCameraPosition() const { return _cameraPosition; };
	const f32 GetCameraRotation() const { return _cameraRotation; };

	void SetCameraAngle(f32 v) { _cameraAngle = v; };
	void SetCameraPosition(f32 v) { _cameraPosition = v; };
	void SetCameraRotation(f32 v) { _cameraRotation = v; };

	//Camera2 &GetCamera() { return cam; };

	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Update(const GameTime &gameTime);
	void Draw(const GameTime &gameTime);

	const LightingMode GetLightingMode() const { return lightMode; }
	void SetLightingMode(LightingMode m) // need to switch shaders when we change how the scene is lit
	{
		lightMode = m;

		if(lightMode == Ambient) // AMBIENT LIGHTS
		{
			Shader* ambientShader = shaderMan.GetShader(ambientLightShaderID);
			houseModel->SetShader(ambientShader);
			baseModel->SetShader(ambientShader);
			tree->SetShader(ambientShader);
			terrain->SetShader(shaderMan.GetShader(terrainShaders.Terrain_Displacement_Ambient_ShaderID));
		}
		else if(lightMode == Directional) // DIRECTIONAL LIGHT
		{
			Shader* directionalLightShader = shaderMan.GetShader(directionalLightShaderID);
			houseModel->SetShader(directionalLightShader);
			baseModel->SetShader(directionalLightShader);
			tree->SetShader(directionalLightShader);
			terrain->SetShader(shaderMan.GetShader(terrainShaders.Terrain_Displacement_Directional_ShaderID));
		}
		else if(lightMode == Spotlights) // SPOT LIGHTS
		{
			Shader* spotShader = shaderMan.GetShader(spotlightShaderID);
			houseModel->SetShader(spotShader);
			baseModel->SetShader(spotShader);
			tree->SetShader(spotShader);
			
			terrain->SetShader(shaderMan.GetShader(terrainShaders.Terrain_Displacement_Spotlights_ShaderID));
		}

		SetTreeShadeMode(tree->GetTreeShadeMode());
	}

	// Controls what is selected as the next mode for shading the tree
	const TreeShadingMode GetNextTreeShadeMode() const
	{
		switch(tree->GetTreeShadeMode())
		{
		case NonTexturedNonLitWireframe:
			return FlatNonTextured;
		case FlatNonTextured:
			return SmoothNonTextured;
		case SmoothNonTextured:
			return SmoothTextured;
		case SmoothTextured:
			return NormalMappedTextured;
		case NormalMappedTextured:
			return NonTexturedNonLitWireframe;
		}

		return SmoothTextured;
	};

	// Used to move between all the tree shade modes
	void SetTreeShadeMode(TreeShadingMode m)
	{
		// Non Textured non lit wireframe
		if(m == NonTexturedNonLitWireframe)
		{
			tree->SetDrawMode(GL_LINE);
			tree->SetTreeShadeMode(0, 0, NonTexturedNonLitWireframe);
		}

		// Flat non textured
		else if(m == FlatNonTextured)
		{
			tree->SetDrawMode(GL_FILL);
			if(lightMode == Ambient)
			{
				Shader *ambShader = shaderMan.GetShader(treeShaders.Tree_Ambient_ShaderID);
				ambShader->Activate(); ambShader->SetUniform("useTextures", false); ambShader->Deactivate();
				tree->SetTreeShadeMode(ambShader,0, FlatNonTextured);
			}
			else if(lightMode == Directional)
				tree->SetTreeShadeMode(shaderMan.GetShader(treeShaders.FlatNonTextured_Directional_ShaderID),0, FlatNonTextured);
			else if(lightMode == Spotlights)
				tree->SetTreeShadeMode(shaderMan.GetShader(treeShaders.FlatNonTextured_Spot_ShaderID),0, FlatNonTextured);
		}

		// Smooth non textured
		else if(m == SmoothNonTextured)
		{
			tree->SetDrawMode(GL_FILL);
			if(lightMode == Ambient)
			{
				Shader *ambShader = shaderMan.GetShader(treeShaders.Tree_Ambient_ShaderID);
				ambShader->Activate(); ambShader->SetUniform("useTextures", false); ambShader->Deactivate();
				tree->SetTreeShadeMode(ambShader,0, SmoothNonTextured);
			}
			else if(lightMode == Directional)
			{
				Shader *directionalShader = shaderMan.GetShader(treeShaders.SmoothShaded_Directional_ShaderID);
				directionalShader->Activate(); directionalShader->SetUniform("useTextures", false); directionalShader->Deactivate();
				tree->SetTreeShadeMode(directionalShader,0, SmoothNonTextured);
			}
			else if(lightMode == Spotlights)
			{
				Shader *spotShader = shaderMan.GetShader(treeShaders.SmoothShaded_Spot_ShaderID);
				spotShader->Activate(); spotShader->SetUniform("useTextures", false); spotShader->Deactivate();
				tree->SetTreeShadeMode(spotShader,0, SmoothNonTextured);
			}
		}

		// Smooth textured
		else if(m == SmoothTextured)
		{
			tree->SetDrawMode(GL_FILL);
			if(lightMode == Ambient)
			{
				Shader *ambShader = shaderMan.GetShader(treeShaders.Tree_Ambient_ShaderID);
				ambShader->Activate(); ambShader->SetUniform("useTextures", true); ambShader->Deactivate();
				tree->SetTreeShadeMode(ambShader,barkTexture, SmoothTextured);
			}
			else if(lightMode == Directional)
			{
				Shader *directionalShader = shaderMan.GetShader(treeShaders.SmoothShaded_Directional_ShaderID);
				directionalShader->Activate(); directionalShader->SetUniform("useTextures", true); directionalShader->Deactivate();
				tree->SetTreeShadeMode(directionalShader,barkTexture, SmoothTextured);
			}
			else if(lightMode == Spotlights)
			{
				Shader *spotShader = shaderMan.GetShader(treeShaders.SmoothShaded_Spot_ShaderID);
				spotShader->Activate(); spotShader->SetUniform("useTextures", true); spotShader->Deactivate();
				tree->SetTreeShadeMode(spotShader,barkTexture, SmoothTextured);
			}
		}
		else if(m == NormalMappedTextured)
		{
			tree->SetTreeShadeMode(shaderMan.GetShader(normalMap_Directional_ShaderID), barkTexture, NormalMappedTextured);
		}
	};

	void ActivateSmokeParticleEffect() { particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID)->SetActive(true); }
	void ActivateSnow() { particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID)->SetActive(true); }
	void ActivateTerrainElevation(TerrainShiftDirection d)
	{
		if(d == Up)
			terrainElevation.elevationRuntime=0;
		else if(d == Down)
			terrainElevation.elevationRuntime = terrainElevation.timeToElevateFully;

		terrainElevation.shiftDirection = d;
	}
	void ActiveTerrainTextureMerge(i32 dir) { mergeDirection=dir; mergingTerrainTextured = true; }

	const Season GetCurrentSeason() const { return seasonMan.GetCurrentSeason(); }

	const c8* GetCurrentSeasonString() const
	{
		switch(seasonMan.GetCurrentSeason())
		{
		case Spring: return "Spring";
		case Summer: return "Summer";
		case Autumn: return "Autumn";
		case Winter: return "Winter";
		}
		return "";
	};

	void SetLightningActive(bool active) { drawLightning = active; }
};