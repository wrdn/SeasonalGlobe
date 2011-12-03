#include "GameTime.h"
#include "ctypes.h"

const f32 GameTime::GetCurrentTime() const { return currentTime; };
const f32 GameTime::GetDeltaTime() const { return deltaTime; };

void GameTime::Update()
{
	f32 temp_time = (f32)gxbase::App::GetTime();
	deltaTime = temp_time - currentTime;
	FrameRate = 1.0f / deltaTime;
	currentTime = temp_time;

	/*currentTime = (f32)gxbase::App::GetTime();
	deltaTime = currentTime - oldTime;
	FrameRate = 1.0f / deltaTime;
	oldTime = currentTime;*/
};

void GameTime::Init() { currentTime = (f32)gxbase::App::GetTime(); oldTime = currentTime; };