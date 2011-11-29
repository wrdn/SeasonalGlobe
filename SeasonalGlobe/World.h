#pragma once

#include "Camera.h"
#include "OBJFile.h"
#include "GameTime.h"
#include "TextureManager.h"
#include "Floor.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "TerrainDisk.h"
#include "Shader.h"
#include "FractalTree.h"

class World
{
private:
	static const u32 CAMERA_COUNT = 1;
	Camera cams[World::CAMERA_COUNT];
	TextureManager texMan;
	Shader _phongShader;
	Texture *grasstexture, *houseTexture, *barkTexture;
	OBJFile *houseModel;
	Sphere *sphere;
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

	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Draw(const GameTime &gameTime);
};

