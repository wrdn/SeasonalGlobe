#pragma once

#include "OBJLoader.h"
#include "GameTime.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "FractalTree.h"
#include "AppConfig.h"
#include "ParticleSystem.h"
#include "CylindricalParticleEmitter.h"
#include "StaticParticleEmitter.h"
#include "FireParticleEmitter.h"
#include "Light.h"
#include "Terrain.h"
#include "PointBasedParticleEmitter.h"
#include "HemiSphericalParticleEmitter.h"
#include "SeasonManager.h"
#include "ResourceManager.h"
#include "Mesh.h"

// Current lighting mode: ambient, directional or spotlights
enum LightingMode
{
	Ambient,
	Directional,
	Spotlights,
};

// The World class contains the entire world state, including the season manager, models, particle systems, camera info,
// shaders and textures, and the applications primary update and draw functions
class World
{
private:
	// Application configuration
	AppConfig conf;

	// Managers
	ParticleSystem particleSystem; // particle manager
	SeasonManager seasonMan; // season manager
	f64 dtMultiplier; // used to speed up/slow down, every frame dt = dt*dtMultiplier

	float3 sceneRotationAxis; // axis about which the scene rotates
	f32 _cameraAngle, _cameraPosition, _cameraRotation; // camera angle, position and rotation
	bool AutoRotate; // autorotating?
	bool snowSlowing; // used to reduce rate of emission to snow slow down towards the end

	// Geometry
	Terrain terrain;
	TerrainShift terrainElevation;
	
	// Objects in the scene we will draw
	GraphicsObject houseModel, baseModel, boltModel;
	Sphere globeSphere;
	Sphere lightSphere;
	Cylinder spotCone;

	// mesh handles for the billboard and imposter models (also stored in the particle system and particle emitters)
	MeshHandle defaultBillboardModel;
	MeshHandle imposterModel;

	// Fractal L-System tree
	FractalTree *tree;

	// polygon mode
	GLenum polygonMode;
	
	// should be draw the lightning now?
	bool drawLightning;
	
	// Shaders
	ShaderHandle phongShader, particleSystemBaseShader, texturedParticleShader, globeShader, directionalLightShader,
		multiTexturingSampleShader, spotlightShader, ambientLightShader,
		normalMap_AmbientShader, normalMap_DirectionalShader, normalMap_SpotlightsShader;

	// Tree shaders
	TreeShaders treeShaders;

	// Terrain shaders
	TerrainShaders terrainShaders;

	// These variables are used when merging textures (diffuse and normal map) on the terrain
	// to create a better effect of snow drifts
	f32 terrainTextureMergeFactor; // between 0 and 1, controls how much of grass/snow texture shown
	bool mergingTerrainTextured; // merging snow to grass (or vice versa)
	f32 timeToMergeTextures; // time in seconds to switch from grass to snow (and vice versa)
	f32 terrainTextureMergeRuntime; // keep track of how far through the merge process we are
	i32 mergeDirection; // should be 1 or -1

	// Textures
	TextureHandle grassTexture, houseTexture, barkTexture, particleTexture,
		leafTexture, baseTexture, displacementTexture, barkNormalMap,
		snowTexture, terrainNormalMapFull, grassParticleTexture, grassParticleColorMap,
		houseNormalMap;

	// Particle emitters
	u32 leafParticleEmitterID;
	u32 snowEmitterID;
	u32 smokeEmitterID;
	u32 grassStaticEmitterID;
	FireParticleEmitter *fireParticleEmitter;

	// Lights
	Light directionalLight; f32 directionalLightRotation; f32 directionalLightSpeed;
	Light spotlights[4];
	Light ambientLight;
	LightingMode lightMode; // ACW-switch between Ambient, Directional and 4 spotlights
	
	// Load functions (see World_Setup.cpp)
	bool LoadTextures();
	bool LoadShaders();
	bool LoadParticles();
	bool LoadGeometry();
	void LoadLights();
	void SetupSeasons();

	// Prevent copying
	World(World const& w);
	World& operator= (World const& other);

	// draw the reflective pond surface and reflect tree, leaves and fire in it)
	void reflective_draw(const GameTime &gameTime);

	// draw the terrain (and normal map/displace in winter)
	void DrawTerrain(const GameTime &gameTime);

public:
	World(void);
	~World(void);

	// Accessors and Mutators
	const bool GetAutoRotate() const { return AutoRotate; };
	void SetAutoRotate(const bool b) { AutoRotate = b; };
	
	StaticParticleEmitter* GetLeafParticleEmitter() { return particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID); }
	const FireParticleEmitter* GetFireParticleEmitter() const { return fireParticleEmitter; };

	FractalTree * GetTree() const { return tree; };
	const GLenum GetPolygonMode() const { return polygonMode; };
	void SetPolygonMode(const GLenum polyMode)
	{
		polygonMode = polyMode;

		houseModel.SetPolygonFillMode(polyMode);
		globeSphere.SetPolygonFillMode(polyMode);
		terrain.SetPolygonFillMode(polyMode);
		baseModel.SetPolygonFillMode(polyMode);
		tree->SetPolygonFillMode(polyMode);
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

	// Load world
	bool Load();

	// Shutdown world (unload)
	void Shutdown();

	// update world (note most updates are done in Draw)
	void Update(const GameTime &gameTime);

	// draw world (and update some stuff)
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
	
	// used when we speed up/slow down the app (through dtMultiplier) 
	void UpdateSceneTimings();

	void SetDtMultiplier(f64 multiplier) { dtMultiplier = max(0,multiplier); };
	const f64 GetMultiplier() const { return dtMultiplier; };

	void SetSnowSlowing(bool _slowing) { snowSlowing = _slowing; };
};