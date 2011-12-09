#pragma once

#include "Color.h"
#include "float3.h"

struct DirectionalLight
{
public:
	float3 position;
	Color4f ambient, diffuse, specular;

	DirectionalLight() { };
	DirectionalLight(float3 &pos, Color4f &amb, Color4f &diff, Color4f &spec)
		: position(pos), ambient(amb), diffuse(diff), specular(spec) { };
	~DirectionalLight() { };
};

struct SpotLight
{
	float3 position, direction;
	f32 innerCutoffAngle, outerCutoffAngle;
	Color4f ambient, diffuse, specular;

	SpotLight() { };
	SpotLight(float3 &pos, float3 &dir, f32 _innerCutoffAngle, f32 _outerCutoffAngle,
		Color4f &amb, Color4f &diff, Color4f &spec)
		: position(pos), direction(dir), innerCutoffAngle(_innerCutoffAngle),
		outerCutoffAngle(_outerCutoffAngle), ambient(amb), diffuse(diff), specular(spec)
	{ };
	~SpotLight() { };
};