#pragma once

#include <GXBase.h>
using gxbase::App;
using gxbase::GLWindowEx;

#include "Color.h"
#include "ctypes.h"
#include "World.h"
#include "GameTime.h"

// Used so GXBase can open a window for us
class SeasonalWindow : public GLWindowEx
{
private:
	static const c8 TREE_DRAW_MODES_CONTROL_KEY = 'm';
	static const c8 LIGHT_SWITCH_CONTROL_KEY = 's';

	static const u32 DEFAULT_WIDTH = 800;
	static const u32 DEFAULT_HEIGHT = 600;
	static const c8* DEFAULT_WINDOW_TITLE;

	// window resolution
	u32 windowRes[2]; //index 0=width, 1=height

	World scn; // holds state of application world, including all models, etc

	GameTime gameTime; // single game time instance updated per frame and passed through to World

	bool _leftDown, _rightDown; // is the left/right mouse button down?

	bool displayHelpMenu; // display help menu?

public:
	SeasonalWindow();

	void OnCreate(); // loads world

	void OnDisplay(); // displays world (calling World::Draw)
	void OnIdle(); // requests redraw

	void OnKeyboard(i32 key, bool down); // keyboard processing
	void OnMouseButton(MouseButton button, bool down); // mouse processing
	void OnMouseMove(i32 x, i32 y);

	// Accessors and Mutators
	const u32* const GetWindowRes() const;
	void SetWindowResolution(const u32 width, const u32 height);
	void ResetPerspective() const;
};