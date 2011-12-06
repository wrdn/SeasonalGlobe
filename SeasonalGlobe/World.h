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

	ParticleSystem particleSystem;

	u32 phongShaderID;
	u32 globeShaderID;
	u32 gradientMapShaderID;
	u32 multiTextureShaderID;

	Texture *grasstexture, *houseTexture, *barkTexture, *particleTexture,
		*gradientMapTexture;

	OBJFile *houseModel;
	Sphere *sphere;
	Cylinder *globeBase;

	OBJFile *baseModel;

	TerrainDisk *terrain;
	FractalTree *tree2;
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

public:
	FGLCaller oglcall;

	f32 waterx, watery, waterz;
	
	f32 scaleX, scaleZ;

	void reflective_draw(const GameTime &gameTime);

	void multi_texturing_test(const GameTime &gameTime);
	Texture *mtt1, *mtt2; // multi texturing test texture 1 and 2

	const bool GetAutoRotate() const { return AutoRotate; };
	void SetAutoRotate(const bool b) { AutoRotate = b; };

	FractalTree * GetTree() const { return tree2; };
	const GLenum GetPolygonMode() const { return polygonMode; };
	void SetPolygonMode(const GLenum polyMode) { polygonMode = polyMode; };
	
	const f32 GetCameraAngle() const { return _cameraAngle; };
	const f32 GetCameraPosition() const { return _cameraPosition; };
	const f32 GetCameraRotation() const { return _cameraRotation; };

	void SetCameraAngle(f32 v) { _cameraAngle = v; };
	void SetCameraPosition(f32 v) { _cameraPosition = v; };
	void SetCameraRotation(f32 v) { _cameraRotation = v; };

	Camera &GetCamera() { return cam; };

	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Draw(const GameTime &gameTime);
};

