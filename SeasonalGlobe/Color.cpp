#include "Color.h"

/*const float4 Color::BLACK = fzero();
const float4 Color::WHITE   = finit(1.0f);
const float4 Color::GREY    = finit(0.5f);
const float4 Color::RED     = finit(1.0f,0,0,0);
const float4 Color::GREEN   = finit(0,1.0f,0,0);
const float4 Color::BLUE    = finit(0,0,1.0f,0);
const float4 Color::YELLOW  = finit(1.0f,1.0f,0,0);
const float4 Color::PINK    = finit(1.0f,0.75f,0.79f,0);
*/

const Color4f Color::BLACK;
const Color4f Color::WHITE  = Color4f(1.0f);
const Color4f Color::GREY   = Color4f(0.5f);
const Color4f Color::RED    = Color4f(1.0f,0,0,0);
const Color4f Color::GREEN  = Color4f(0,1.0f,0,0);
const Color4f Color::BLUE   = Color4f(0,0,1.0f,0);
const Color4f Color::YELLOW = Color4f(1.0f,1.0f,0,0);
const Color4f Color::PINK   = Color4f(1.0f,0.75f,0.79f,0);