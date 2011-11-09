#include "Color.h"

Color4f::Color4f() : r(0), g(0), b(0), a(0) { };
Color4f::Color4f(f32 _r, f32 _g, f32 _b, f32 _a) : r(_r), g(_g), b(_b), a(_a) { };
Color4f::Color4f(f32 v) : r(v), g(v), b(v), a(v) { };

Color::Color() { };

const Color4f Color::BLACK  = Color4f();
const Color4f Color::WHITE  = Color4f(1.0f);
const Color4f Color::GREY   = Color4f(0.5f);
const Color4f Color::RED    = Color4f(1.0f,0,0,0);
const Color4f Color::GREEN  = Color4f(0,1.0f,0,0);
const Color4f Color::BLUE   = Color4f(0,0,1.0f,0);
const Color4f Color::YELLOW = Color4f(1.0f,1.0f,0,0);
const Color4f Color::PINK   = Color4f(1.0f,0.75f,0.79f,0);

Color4f Color::FromInt(const u32 i)
{
	return Color4f
		(
		(i >> 24) & 255,
		(i >> 16) & 255,
		(i >> 8) & 255,
		(i >> 32) & 255
		);
};

u32 Color::ToInt(const f32 _r, const f32 _g, const f32 _b, const f32 _a)
{
	return ( (char)_r << 24 | (char)_g << 16 | (char)_b << 8 | (char)_a );
};

u32 Color::ToInt(const Color4f &c)
{
	return ( (char)c.r << 24 | (char)c.g << 16 | (char)c.b << 8 | (char)c.a );
};