#pragma once

#include <GXBase.h>
using gxbase::App;
using gxbase::GLWindowEx;

#include "Color.h"

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

	const Color4f& GetClearColor() const;
	void SetClearColor(const Color4f c);

	SeasonalWindow();

private:
	int windowRes[2]; //index 0=width, 1=height
	Color4f clearColor; // opengl window clear color
};