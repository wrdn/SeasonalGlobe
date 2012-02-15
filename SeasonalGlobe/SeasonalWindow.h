#pragma once

#include <GXBase.h>
using gxbase::App;
using gxbase::GLWindowEx;

#include "Color.h"
#include "ctypes.h"
#include "World.h"
#include "GameTime.h"

class SeasonalWindow : public GLWindowEx
{
private:
	static const c8 TREE_DRAW_MODES_CONTROL_KEY = 'm';
	static const c8 LIGHT_SWITCH_CONTROL_KEY = 's';

	static const u32 DEFAULT_WIDTH = 800;
	static const u32 DEFAULT_HEIGHT = 600;
	static const c8* DEFAULT_WINDOW_TITLE;

	u32 windowRes[2]; //index 0=width, 1=height
	Color4f clearColor; // opengl window clear color

	World scn;

	GameTime gameTime;

	bool _leftDown, _rightDown;

	bool displayHelpMenu;

public:

	void OnCreate();
	void OnDestroy();

	void OnDisplay();
	void OnIdle();

	void OnKeyboard(i32 key, bool down);
	void OnMouseButton(MouseButton button, bool down);
	void OnMouseMove(i32 x, i32 y);

	const u32* const GetWindowRes() const;
	void SetWindowResolution(const u32 width, const u32 height);
	void ResetPerspective() const;
	void SwitchFullscreen();
	const bool IsFullScreen() const;

	// Parasoft warns that you shouldn't return a member variable by reference. However, in this case, it is more efficient.
	// Further, if it is altered to return by value, Parasoft throws a different warning that that may be inefficient.
	const Color4f& GetClearColor() const;

	void SetClearColor(const Color4f &c);

	SeasonalWindow();
};