#pragma once

#include "ctypes.h"
#include <GXBase.h>

class GameTime
{
private:
	f32 currentTime;
	f32 deltaTime;
public:
	GameTime() : currentTime(0), deltaTime(0) { };

	const float GetCurrentTime() const;
	const float GetDeltaTime() const;

	void Init();
	void Update();
};