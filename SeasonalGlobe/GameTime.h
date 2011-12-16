#pragma once

#include "ctypes.h"
#include <GXBase.h>

class GameTime
{
private:
	f32 currentTime;
	f32 oldTime;
	f32 deltaTime;
	f32 FrameRate;
	f32 runningTime;

public:
	GameTime() : currentTime(0), oldTime(0), deltaTime(0), FrameRate(0), runningTime(0) { };

	const f32 GetCurrentTime() const;
	const f32 GetDeltaTime() const;

	void Init();
	void Update();

	void SetDeltaTime(f32 _dt) { deltaTime = _dt; }

	const f32 GetFrameRate() const { return FrameRate; };

	const f32 GetRunningTime() const { return runningTime; };
};