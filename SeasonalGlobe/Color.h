#pragma once

struct Color4f
{
public:
	float r,g,b,a;

	Color4f() : r(0), g(0), b(0), a(0) { };
	Color4f(float _r, float _g, float _b, float _a)
		: r(_r), g(_g), b(_b), a(_a) { };
	Color4f(float v) : r(v), g(v), b(v), a(v) { };
};

class Color
{
public:
	static const Color4f BLACK;
	static const Color4f WHITE;
	static const Color4f GREY;
	static const Color4f RED;
	static const Color4f GREEN;
	static const Color4f BLUE;
	static const Color4f YELLOW;
	static const Color4f PINK;
};