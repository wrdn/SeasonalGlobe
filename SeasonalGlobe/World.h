#pragma once

#include "Camera.h"
#include "OBJFile.h"
#include "GameTime.h"
#include "TextureManager.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "TerrainDisk.h"
#include "ShaderManager.h"
#include "FractalTree.h"
#include "AppConfig.h"
#include "ParticleSystem.h"
#include "CylindricalParticleEmitter.h"
#include "FireParticleEmitter.h"

class FGLCaller : public glex
{
public:
	FGLCaller()
	{
	};
};

class World
{
private:
	AppConfig conf;

	Camera cam;

	TextureManager texMan;
	ShaderManager shaderMan;

	u32 phongShaderID;
	u32 globeShaderID;
	u32 gradientMapShaderID;
	u32 multiTextureShaderID;

	u32 particleSystemBaseShaderID;
	u32 leafParticleEmitterID;
	u32 cylindricalParticleEmitterID;
	u32 snowEmitterID;
	CylindricalParticleEmitter *fireEmitter;

	FireParticleEmitter *fireParticleEmitter;


	Texture *grasstexture, *houseTexture, *barkTexture, *particleTexture,
		*gradientMapTexture, *leafTexture;

	OBJFile *houseModel;
	Sphere *globeSphere;

	OBJFile *baseModel;

	TerrainDisk *terrain;
	FractalTree *tree;
	float3 sceneRotationAxis;
	f32 _cameraAngle;
	f32 _cameraPosition;
	f32 _cameraRotation;
	bool AutoRotate;
	GLenum polygonMode;

	std::vector<Model*> models;

	template<class T>
	T* AddModel();

	// Prevent copying
	World(World const& w);
	World& operator= (World const& other);

	float3 treePos;

	bool LoadTextures();
	bool LoadShaders();
	bool LoadParticles();
	bool LoadGeometry();

	Cylinder testFireCylinder;

	FGLCaller oglcall;
	f32 waterx, watery, waterz;
	f32 scaleX, scaleZ;
	u32 smokeEmitter;
	ParticleSystem particleSystem;
	Texture *mtt1, *mtt2; // multi texturing test texture 1 and 2


public:
	
	ParticleSystem &GetParticleSystem() { return particleSystem; };
	const u32 GetSmokeEmitterID() const { return smokeEmitter; };

	void reflective_draw(const GameTime &gameTime);

	void multi_texturing_test(const GameTime &gameTime);

	const bool GetAutoRotate() const { return AutoRotate; };
	void SetAutoRotate(const bool b) { AutoRotate = b; };

	FractalTree * GetTree() const { return tree; };
	const GLenum GetPolygonMode() const { return polygonMode; };
	void SetPolygonMode(const GLenum polyMode)
	{
		polygonMode = polyMode;

		((Model*)houseModel->GetModels())->SetDrawMode(polyMode);
		globeSphere->SetDrawMode(polyMode);
		terrain->SetDrawMode(polyMode);
		((Model*)baseModel->GetModels())->SetDrawMode(polyMode);
		((Model*)tree->GetBranchModel())->SetDrawMode(polyMode);
	};
	
	const f32 GetCameraAngle() const { return _cameraAngle; };
	const f32 GetCameraPosition() const { return _cameraPosition; };
	const f32 GetCameraRotation() const { return _cameraRotation; };

	void SetCameraAngle(f32 v) { _cameraAngle = v; };
	void SetCameraPosition(f32 v) { _cameraPosition = v; };
	void SetCameraRotation(f32 v) { _cameraRotation = v; };

	const Camera &GetCamera() { return cam; };

	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Draw(const GameTime &gameTime);
};

