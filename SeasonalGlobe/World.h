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
#include "FireParticleEmitter.h"
#include "SphericalCamera.h"
#include "Camera2.h"
#include "Light.h"
#include "Disk.h"
#include "TerrainLoader.h"

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

	// Managers
	TextureManager texMan;
	ShaderManager shaderMan;
	ParticleSystem particleSystem;

	// Camera
	Camera2 cam;
	float3 sceneRotationAxis;
	f32 _cameraAngle, _cameraPosition, _cameraRotation;
	bool AutoRotate;

	// Geometry
	TerrainLoader *terrain;
	FractalTree *tree;
	Sphere *globeSphere;
	GraphicsObject *houseModel, *baseModel;
	Model *defaultBillboardModel;
	GLenum polygonMode;

	// Shaders
	u32 phongShaderID, particleSystemBaseShaderID,
		globeShaderID, directionalLightShaderID,
		multiTexturingSampleShaderID, spotlightShaderID;

	// Textures
	Texture *grassTexture, *houseTexture, *barkTexture, *particleTexture,
		*leafTexture, *baseTexture;

	// Particle emitters
	u32 leafParticleEmitterID;
	u32 snowEmitterID;
	u32 smokeEmitterID;
	FireParticleEmitter *fireParticleEmitter;

	// Lights
	Light directionalLight, spotlight;
	LightingMode lightMode; // ACW-switch between Ambient, Directional and 4 spotlights
	Sphere *lightSphere;
	Cylinder *spotCone;

	// Load functions
	bool LoadTextures();
	bool LoadShaders();
	bool LoadParticles();
	bool LoadGeometry();

	// Prevent copying
	World(World const& w);
	World& operator= (World const& other);

	void reflective_draw(const GameTime &gameTime);
	void multi_texturing_test(/*const GameTime &gameTime*/);
public:
	const bool GetAutoRotate() const { return AutoRotate; };
	void SetAutoRotate(const bool b) { AutoRotate = b; };

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

	void Update(GameTime &gameTime);
	void Draw(const GameTime &gameTime);
};

