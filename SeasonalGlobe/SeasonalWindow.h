#pragma once

#include <GXBase.h>
using gxbase::App;
using gxbase::GLWindowEx;

class SeasonalWindow : public GLWindowEx
{
public:
	void OnDisplay();
	void OnIdle();
	void OnKeyboard(int key, bool down);
	void OnCreate();
	void OnDestroy();

	const int* const GetWindowRes() const;
	void SetWindowResolution(const int width, const int height);
	void SwitchFullscreen();
	const bool IsFullScreen() const;

	SeasonalWindow();

private:
	int windowRes[2]; //index 0=width, 1=height
};