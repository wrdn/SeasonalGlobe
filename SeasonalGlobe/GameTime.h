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

	const f32 GetCurrentTime() const;
	const f32 GetDeltaTime() const;

	void Init();
	void Update();
};