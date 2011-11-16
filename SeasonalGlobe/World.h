#pragma once

#include "Camera.h"
#include "OBJFile.h"
#include "GameTime.h"
#include "TextureManager.h"
#include "Floor.h"
#include "Sphere.h"
#include "TerrainDisk.h"

class World
{
private:
	static const u32 CAMERA_COUNT = 1;
	Camera cams[World::CAMERA_COUNT];

	TextureManager texMan;
	int testTextureID;
	int houseTextureID;

	OBJFile *cubeModel;
	Floor *floor;
	OBJFile *houseModel;
	Sphere *sphere;
	TerrainDisk *terrain;

public:

	// Temporary Camera Variables (Code from RealTime Graphics module tutorials)
	f32 _cameraAngle;
	f32 _cameraPosition;
	f32 _cameraRotation;


	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Draw(const GameTime &gameTime);
};

