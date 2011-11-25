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
#include "MyCylinder.h"

class World
{
private:
	static const u32 CAMERA_COUNT = 1;
	Camera cams[World::CAMERA_COUNT];

	TextureManager texMan;
	
	Shader _phongShader;
	
	Texture *grasstexture, *houseTexture, *barkTexture;
	
	OBJFile *cubeModel;
	OBJFile *houseModel;
	
	Floor *floor;
	Cylinder *_cylinder;
	Sphere *sphere;
	TerrainDisk *terrain;

	MyCylinder *mycylinder;


	std::vector<CustomModel*> models;

	template<class T>
	T* AddModel();

public:

	// Temporary Camera Variables (Code from RealTime Graphics module tutorials)
	f32 _cameraAngle;
	f32 _cameraPosition;
	f32 _cameraRotation;

	GLenum terrainPolyMode;

	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Draw(const GameTime &gameTime);
};

