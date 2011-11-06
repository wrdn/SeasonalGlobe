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