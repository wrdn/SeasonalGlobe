#pragma once

#include "ctypes.h"
#include <iosfwd>

struct Vec2i
{
public:
	i32 x,y;
	Vec2i() : x(0), y(0) { };
	Vec2i(i32 _x, i32 _y): x(_x), y(_y) { };
};

class float2
{
public:
	f32 vec[2];

	static const float2 ZERO;
	static const float2 ONE;

	~float2();
	float2();
	float2(const f32 v);
	float2(const f32 _x, f32 _y);
	float2(const f32 * v);

	f32 x() const { return vec[0]; };
	f32 y() const { return vec[1]; };
	void x(const f32 v) { vec[0]=v; };
	void y(const f32 v) { vec[1]=v; };

	void set(const f32 _x, const f32 _y)
	{
		vec[0] = _x;
		vec[1] = _y;
	};
	void set(const f32 * const v)
	{
		vec[0] = v[0];
		vec[1] = v[1];
	};

	void zero(); // zero's the current vector
	void setall(const f32 v);

	float2 add(const float2 &v) const;
	float2 sub(const float2 &v) const;
	float2 mul(const float2 &v) const;
	float2 div(const float2 &v) const;
	
	float2 add(const f32 t) const;
	float2 sub(const f32 t) const;
	float2 mul(const f32 t) const;
	float2 div(const f32 t) const;

	f32 dot(const float2 &v) const;

	float2 negate() const;
};

// Auxiliary Functions (operator overloads)
float2 operator-(float2 &v);

float2 operator+(const float2 &a, const float2 &b);
float2 operator-(const float2 &a, const float2 &b);
float2 operator*(const float2 &a, const float2 &b);
float2 operator/(const float2 &a, const float2 &b);

float2 operator+(const float2 &a, const f32 b);
float2 operator-(const float2 &a, const f32 b);
float2 operator*(const float2 &a, const f32 b);
float2 operator/(const float2 &a, const f32 b);

float2 operator+(const f32 a, const float2 &b);
float2 operator-(const f32 a, const float2 &b);
float2 operator*(const f32 a, const float2 &b);
float2 operator/(const f32 a, const float2 &b);

void operator+=(float2 &a, const float2 &b);
void operator-=(float2 &a, const float2 &b);
void operator*=(float2 &a, const float2 &b);
void operator/=(float2 &a, const float2 &b);

void operator+=(float2 &a, const f32 &b);
void operator-=(float2 &a, const f32 &b);
void operator*=(float2 &a, const f32 &b);
void operator/=(float2 &a, const f32 &b);

std::ostream& operator<<(std::ostream &out, float2 &m);