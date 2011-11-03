#ifndef __FLOAT4_H__
#define __FLOAT4_H__

#include <emmintrin.h> // SSE2    http://softpixel.com/~cwright/programming/simd/sse2.php
#include <pmmintrin.h> // SSE3    http://softpixel.com/~cwright/programming/simd/sse3.php
#include "ctypes.h"

// Aligned initialisation
float4 finit(const f32 v);
float4 finit(const f32 x, const f32 y, const f32 z, const f32 w);
float4 finit(const f32 *v);

// Unaligned initialisation
float4 finitu(const f32 *v);

float4 fzero();
void fzero(float4 &t);

float4 fadd(const float4 a, const float4 b);
void fadd(const float4 a, const float4 b, float4 &out);

float4 fsub(const float4 a, const float4 b);
void fsub(const float4 a, const float4 b, float4 &out);

float4 fmul(const float4 a, const float4 b);
void fmul(const float4 a, const float4 b, float4 &out);

float4 fdiv(const float4 a, const float4 b);
void fdiv(const float4 a, const float4 b, float4 &out);

f32 fdot(const float4 a, const float4 b);
float4 fcross(const float4 a, const float4 b);
float4 fnormalize(const float4 a);

void fcross(const float4 a, const float4 b, float4 &out);
void fnormalize(const float4 a, float4 &out);
void fnormalize(float4 *a);

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

#endif