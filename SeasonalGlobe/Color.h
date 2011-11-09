#pragma once

#include "ctypes.h"

struct Color4f
{
public:
	f32 r,g,b,a;

	Color4f();
	Color4f(f32 _r, f32 _g, f32 _b, f32 _a);
	Color4f(f32 v);
};

class Color
{
private:
	Color();
public:
	static const Color4f BLACK;
	static const Color4f WHITE;
	static const Color4f GREY;
	static const Color4f RED;
	static const Color4f GREEN;
	static const Color4f BLUE;
	static const Color4f YELLOW;
	static const Color4f PINK;

	static Color4f FromInt(const u32 i);
	static u32 ToInt(const f32 _r, const f32 _g, const f32 _b, const f32 _a);
	static u32 ToInt(const Color4f &c);
};