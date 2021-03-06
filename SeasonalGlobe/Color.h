#pragma once

#include "ctypes.h"
#include "float4.h"

typedef float4 Color4f;
typedef float4 color;

// Static class to hold some colors (in Color4f r,g,b,a form)
// All colors have values in the range 0 to 1
// You can convert a color in the range 0 to 255 into a color in
// the range 0 to 1 by calling Normalize(const Color4f &c)
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
	static u32 ToInt(f32 _r, const f32 _g, const f32 _b, const f32 _a);
	static u32 ToInt(const Color4f &c);

	// Maps each value in Color4f c into the range 0 to 1 (for GLSL)
	static Color4f Normalize(const Color4f &c);
};

// color represented as int
class ColorU32
{
private:
	u32 color;
public:
	ColorU32();

	void SetColor(const u32 i);
	void SetColor(const f32 r, const f32 g, const f32 b, const f32 a);

	const u32 GetColor() const;
};