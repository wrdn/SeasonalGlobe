#pragma once

#include "ctypes.h"
#include <iostream>

class float4;
class float3
{
public:
	ALIGN(16) f32 vec[3];

	static const float3 ZERO;
	static const float3 ONE;

	~float3();
	float3(); // sets all to 0
	float3(const f32 v);
	float3(const f32 _x, f32 _y, f32 _z);
	float3(const f32 * v);
	float3(const float4 &f);

	float4 ToFloat4() const;

	void zero(); // zero's the current vector
	void setall(const f32 v);

	float3 add(const float3 &v) const;
	float3 sub(const float3 &v) const;
	float3 mul(const float3 &v) const;
	float3 div(const float3 &v) const;
	
	float3 add(const f32 t) const;
	float3 sub(const f32 t) const;
	float3 mul(const f32 t) const;
	float3 div(const f32 t) const;

	f32 dot(const float3 &v) const;
	float3 cross(const float3 &v) const;
	void cross(const float3 &v, f32 * const output3d) const;

	float3 normalize() const;
	void normalize();

	f32 magnitude() const;
	f32 LengthSquared() const;
	f32 distance(const float3 &v) const;
	f32 DistanceSquared(const float3 &v) const;

	float3 negate() const;
	float3 absolute() const;
};

// Auxiliary Functions (operator overloads)
float3 operator-(float3 &v);

float3 operator+(const float3 &a, const float3 &b);
float3 operator-(const float3 &a, const float3 &b);
float3 operator*(const float3 &a, const float3 &b);
float3 operator/(const float3 &a, const float3 &b);

float3 operator+(const float3 &a, const f32 b);
float3 operator-(const float3 &a, const f32 b);
float3 operator*(const float3 &a, const f32 b);
float3 operator/(const float3 &a, const f32 b);

float3 operator+(const f32 a, const float3 &b);
float3 operator-(const f32 a, const float3 &b);
float3 operator*(const f32 a, const float3 &b);
float3 operator/(const f32 a, const float3 &b);

void operator+=(float3 &a, const float3 &b);
void operator-=(float3 &a, const float3 &b);
void operator*=(float3 &a, const float3 &b);
void operator/=(float3 &a, const float3 &b);

void operator+=(float3 &a, const f32 &b);
void operator-=(float3 &a, const f32 &b);
void operator*=(float3 &a, const f32 &b);
void operator/=(float3 &a, const f32 &b);

float3 operator^(const float3 &a, const float3 &b);
void operator^=(float3 &a, const float3 &b);

std::ostream& operator<<(std::ostream &out, float3 &m);
