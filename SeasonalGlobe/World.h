#pragma once

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
#include "Light.h"
#include "TerrainLoader.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "SeasonManager.h"
#include "tutorialcodeheaders.h"

enum LightingMode
{
	Ambient,
	Directional,
	Spotlights,
};

class World
{
private:
	// Application configuration
	AppConfig conf;

	Lights _light1, _light2, _light3, _light4, _light5;
	Materials _material1, _material2, _material3;

	// Managers
	TextureManager texMan;
	ShaderManager shaderMan;
	ParticleSystem particleSystem;
	SeasonManager seasonMan;
	f64 dtMultiplier; // used to speed up/slow down, every frame dt = dt*dtMultiplier

	float3 sceneRotationAxis;
	f32 _cameraAngle, _cameraPosition, _cameraRotation;
	bool AutoRotate;
	bool snowSlowing; // used to reduce rate of emission to snow slow down towards the end

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
		*grassParticleColorMap,
		*houseNormalMap;

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

	bool LoadShaders();
	bool LoadParticles();
	bool LoadGeometry();
	void LoadLights();
	void SetupSeasons();

	// Prevent copying
	World(World const& w);
	World& operator= (World const& other);

	void reflective_draw(const GameTime &gameTime);

	void DrawTerrain(const GameTime &gameTime);

public:

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
	GLenum GetNextPolygonMode(const GLenum polyMode)
	{
		return polyMode == GL_FILL ? GL_LINE : GL_FILL;
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
	void SetLightingMode(LightingMode m);

	// Controls what is selected as the next mode for shading the tree
	const TreeShadingMode GetNextTreeShadeMode() const;

	// Used to move between all the tree shade modes
	void SetTreeShadeMode(TreeShadingMode m);

	void ActivateSmokeParticleEffect() { particleSystem.GetEmitter<PointBasedParticleEmitter>(smokeEmitterID)->SetActive(true); }
	void ActivateSnow() { particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID)->SetActive(true); }
	void DeactivateSnow() { particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID)->SetActive(false); }
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

	const SeasonManager* GetSeasonManager() const { return &seasonMan; }

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

	void ResetWorld();
	
	void UpdateSceneTimings();

	void SetDtMultiplier(f64 multiplier) { dtMultiplier = max(0,multiplier); };
	const f64 GetMultiplier() const { return dtMultiplier; };

	void SetSnowSlowing(bool _slowing) { snowSlowing = _slowing; }
};