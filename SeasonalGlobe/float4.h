#pragma once

#include "ctypes.h"

bool NearZero(const f32 v);

class float4
{
public:
	f32 vec[4];

	static const float4 ZERO;
	static const float4 ONE;

	float4(); // sets all to 0
	float4(const f32 v);
	float4(const f32 _x, f32 _y, f32 _z);
	float4(const f32 _x, f32 _y, f32 _z, f32 _w);
	float4(const f32 * v); // copy from 4 element array

	static float4 FromXYZ(const f32 * v); 

	void zero(); // zero's the current vector

	float4 add(const float4 &v) const;
	float4 sub(const float4 &v) const;
	float4 mul(const float4 &v) const;
	float4 div(const float4 &v) const;
	
	float4 add(const f32 t) const;
	float4 sub(const f32 t) const;
	float4 mul(const f32 t) const;
	float4 div(const f32 t) const;

	f32 dot(const float4 &v) const;
	float4 cross(const float4 &v) const;
	void cross(const float4 &v, f32 * const output3d) const;

	float4 normalize() const;
	void normalize();

	f32 magnitude() const;
	f32 LengthSquared() const;
	f32 distance(const float4 &v) const;
	f32 DistanceSquared(const float4 &v) const;

	float4 negate() const;
	float4 absolute() const;
};

// Auxiliary Functions (operator overloads)
float4 operator-(float4 &v);

float4 operator+(const float4 &a, const float4 &b);
float4 operator-(const float4 &a, const float4 &b);
float4 operator*(const float4 &a, const float4 &b);
float4 operator/(const float4 &a, const float4 &b);

float4 operator+(const float4 &a, const f32 b);
float4 operator-(const float4 &a, const f32 b);
float4 operator*(const float4 &a, const f32 b);
float4 operator/(const float4 &a, const f32 b);

float4 operator+(const f32 a, const float4 &b);
float4 operator-(const f32 a, const float4 &b);
float4 operator*(const f32 a, const float4 &b);
float4 operator/(const f32 a, const float4 &b);

void operator+=(float4 &a, const float4 &b);
void operator-=(float4 &a, const float4 &b);
void operator*=(float4 &a, const float4 &b);
void operator/=(float4 &a, const float4 &b);

void operator+=(float4 &a, const f32 &b);
void operator-=(float4 &a, const f32 &b);
void operator*=(float4 &a, const f32 &b);
void operator/=(float4 &a, const f32 &b);

float4 operator^(const float4 &a, const float4 &b);
void operator^=(float4 &a, const float4 &b);

/*
#include <emmintrin.h> // SSE2    http://softpixel.com/~cwright/programming/simd/sse2.php
#include <pmmintrin.h> // SSE3    http://softpixel.com/~cwright/programming/simd/sse3.php
#include "ctypes.h"

// Aligned initialisation
float4 finit(const f32 v);
float4 finit(const f32 x, const f32 y, const f32 z, const f32 w);
float4 finit(const f32 x, const f32 y, const f32 z);
float4 finit(const f32 *v);

// Unaligned initialisation
float4 finitu(const f32 *v);

float4 fzero();
void fzero(float4 &t);

float4 fadd(const float4 &a, const float4 &b);
void fadd(const float4 &a, const float4 &b, float4 &out);

float4 fsub(const float4 &a, const float4 &b);
void fsub(const float4 &a, const float4 &b, float4 &out);

float4 fmul(const float4 &a, const float4 &b);
void fmul(const float4 &a, const float4 &b, float4 &out);

float4 fdiv(const float4 &a, const float4 &b);
void fdiv(const float4 &a, const float4 &b, float4 &out);

f32 fdot(const float4 &a, const float4 &b);
float4 fcross(const float4 &a, const float4 &b);
float4 fnormalize(const float4 &a);

void fcross(const float4 &a, const float4 &b, float4 &out);
void fnormalize(const float4 &a, float4 &out);
void fnormalize(float4 *a);

// pulls the data out of the sse vector, and into
// a 4 element array (out)
void fpull(const float4 &f, f32 *out);
void fpullu(const float4 &f, f32 *out); // pull data out into address unaligned array

// pulls data into 3 elements array (out)
void fpull3(const float4 &f, f32 *out);

float4 negate(const float4 &v);

float4 fabsolute(const float4 &v);

float4 operator-(float4 &v);

float4 operator+(const float4 &a, const float4 &b);
float4 operator-(const float4 &a, const float4 &b);
float4 operator*(const float4 &a, const float4 &b);
float4 operator/(const float4 &a, const float4 &b);

float4 operator+(const float4 &a, const f32 &b);
float4 operator-(const float4 &a, const f32 &b);
float4 operator*(const float4 &a, const f32 &b);
float4 operator/(const float4 &a, const f32 &b);

void operator+=(float4 &a, const float4 &b);
void operator-=(float4 &a, const float4 &b);
void operator*=(float4 &a, const float4 &b);
void operator/=(float4 &a, const float4 &b);

void operator+=(float4 &a, const f32 &b);
void operator-=(float4 &a, const f32 &b);
void operator*=(float4 &a, const f32 &b);
void operator/=(float4 &a, const f32 &b);

//friend vfloat4 operator^(const vfloat4 &a, const vfloat4 &b) { vfloat4 v; v.vec = fcross(a.vec, b.vec); return v; };
float4 operator^(const float4 &a, const float4 &b);
float4 operator^(const float4 &a, const f32 &b);
void operator^=(float4 &a, const float4 &b);
void operator^=(float4 &a, const f32 &b);

void fcross3(f32 *a, f32 *b, f32 *out);
void fmin3(f32 *a, f32 *b, f32 *out);
*/