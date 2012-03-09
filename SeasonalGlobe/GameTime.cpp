#include "GameTime.h"
#include "ctypes.h"

const f32 GameTime::GetCurrentTime() const { return currentTime; };
const f32 GameTime::GetDeltaTime() const { return deltaTime; };

void GameTime::Update()
{
	f32 temp_time = (f32)gxbase::App::GetTime();
	deltaTime = temp_time - currentTime;
	if(deltaTime >= 1) deltaTime=0;

	currentTime = temp_time;

	runningTime += deltaTime;
};

void GameTime::Init() { currentTime = (f32)gxbase::App::GetTime(); runningTime = 0; deltaTime=0; };