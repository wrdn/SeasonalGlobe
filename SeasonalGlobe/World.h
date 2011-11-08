#pragma once

#include "Camera.h"
#include "OBJFile.h"
#include "GameTime.h"

class World
{
private:
	static const u32 CAMERA_COUNT = 1;
	Camera cams[World::CAMERA_COUNT];

	OBJFile cubeModel;
public:
	World(void);
	~World(void);

	bool Load();

	void Draw(const GameTime &gameTime);
};

