#pragma once

#include "Camera.h"
#include "OBJFile.h"
#include "GameTime.h"
#include "TextureManager.h"
#include "Floor.h"

class World
{
private:
	static const u32 CAMERA_COUNT = 1;
	Camera cams[World::CAMERA_COUNT];

	TextureManager texMan;
	int testTextureID;

	OBJFile *cubeModel;
	Floor *floor;
public:
	World(void);
	~World(void);

	bool Load();
	void Shutdown();

	void Draw(const GameTime &gameTime);
};

