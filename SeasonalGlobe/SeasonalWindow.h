#pragma once

#include <GXBase.h>
using gxbase::App;
using gxbase::GLWindowEx;

#include "Color.h"

class SeasonalWindow : public GLWindowEx
{
private:
	int windowRes[2]; //index 0=width, 1=height
	Color4f clearColor; // opengl window clear color
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

	// Parasoft warns that you shouldn't return a member variable by reference. However, in this case, it is more efficient.
	// Further, if it is altered to return by value, Parasoft throws a different warning that that may be inefficient.
	const Color4f& GetClearColor() const;

	void SetClearColor(const Color4f &c);

	SeasonalWindow();
};