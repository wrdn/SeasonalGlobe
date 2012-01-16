#include "Color.h"

Color::Color() { };

const Color4f Color::BLACK  = Color4f();
const Color4f Color::WHITE  = Color4f(1.0f);
const Color4f Color::GREY   = Color4f(0.5f);
const Color4f Color::RED    = Color4f(1.0f,0,0,1);
const Color4f Color::GREEN  = Color4f(0,1.0f,0,1);
const Color4f Color::BLUE   = Color4f(0,0,1.0f,1);
const Color4f Color::YELLOW = Color4f(1.0f,1.0f,0,1);
const Color4f Color::PINK   = Color4f(1.0f,0.75f,0.79f,1);

Color4f Color::FromInt(const u32 i)
{
	return Color4f
		(
		(f32)(((u32)i >> 24) & 255),
		(f32)(((u32)i >> 16) & 255),
		(f32)(((u32)i >> 8) & 255),
		(f32)(((u32)i & 255))
		);
};

u32 Color::ToInt(f32 _r, const f32 _g, const f32 _b, const f32 _a)
{
	return ((u32)_r << 24 | (u32)_g << 16 | (u32)_b << 8 | (u32)_a);
};

u32 Color::ToInt(const Color4f &c)
{
	return ToInt(c.r(), c.g(), c.b(), c.a());
};

ColorU32::ColorU32() : color(0) { };

void ColorU32::SetColor(const u32 i) { color = i; };

void ColorU32::SetColor(const f32 r, const f32 g, const f32 b, const f32 a)
{
	color = Color::ToInt(r,g,b,a);
};

const u32 ColorU32::GetColor() const
{
	return color;
};

Color4f Color::Normalize(const Color4f &c)
{
	const f32 mul = 1.0f / 255.0f;
	return c*mul;
};