#include "GameTime.h"
#include "ctypes.h"

const f32 GameTime::GetCurrentTime() const { return currentTime; };
const f32 GameTime::GetDeltaTime() const { return deltaTime; };

void GameTime::Update()
{
	f32 temp_time = (f32)gxbase::App::GetTime();
	deltaTime = temp_time - currentTime;
	currentTime = temp_time;
};

void GameTime::Init() { currentTime = (f32)gxbase::App::GetTime(); };