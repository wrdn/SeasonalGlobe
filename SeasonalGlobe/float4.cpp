#include "float4.h"

static const float4 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

float4 finit(const f32 v)
{
	return _mm_load1_ps(&v);
};

float4 finit(const f32 x, const f32 y, const f32 z, const f32 w)
{
	return _mm_set_ps(x,y,z,w);
};

float4 finit(const f32 *v)
{
	return _mm_load_ps(v);
};

float4 finitu(const f32 *v)
{
	return _mm_loadu_ps(v);
};

float4 fzero()
{
	return _mm_setzero_ps();
};

void fzero(float4 &t)
{
	t = _mm_setzero_ps();
};

float4 fadd(const float4 &a, const float4 &b)
{
	return _mm_add_ps(a,b);
};

void fadd(const float4 &a, const float4 &b, float4 &out)
{
	out = _mm_add_ps(a,b);
};

float4 fsub(const float4 &a, const float4 &b)
{
	return _mm_sub_ps(a,b);
};

void fsub(const float4 &a, const float4 &b, float4 &out)
{
	out = _mm_sub_ps(a,b);
};

float4 fmul(const float4 &a, const float4 &b)
{
	return _mm_mul_ps(a,b);
};

void fmul(const float4 &a, const float4 &b, float4 &out)
{
	out = _mm_mul_ps(a,b);
};

float4 fdiv(const float4 &a, const float4 &b)
{
	return _mm_div_ps(a,b);
};

void fdiv(const float4 &a, const float4 &b, float4 &out)
{
	out = _mm_div_ps(a,b);
};

f32 fdot(const float4 &a, const float4 &b)
{
	// http://software.intel.com/en-us/articles/implement-a-horizontal-addsubtract-with-sse3-instructions/
	float4 mres = _mm_mul_ps(a,b);
	float4 h = _mm_hadd_ps(mres,mres);
	float4 fin = _mm_hadd_ps(h,h);
	f32 ret; _mm_store_ss(&ret,fin);
	return ret;
};

float4 fcross(const float4 &a, const float4 &b)
{
	float4 ares = _mm_mul_ps(_mm_shuffle_ps(a,a,0xD8), _mm_shuffle_ps(b,b,0xE1));
	float4 bres = _mm_mul_ps(_mm_shuffle_ps(a,a,0xE1), _mm_shuffle_ps(b,b,0xD8));
	return _mm_mul_ps(ares,bres);
};

float4 fnormalize(const float4 &a)
{
	__m128 l = _mm_mul_ps(a, a);
	l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
	return _mm_div_ps(a, _mm_sqrt_ps(_mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11))));
};

void fcross(const float4 &a, const float4 &b, float4 &out)
{
	float4 ares = _mm_mul_ps(_mm_shuffle_ps(a,a,0xD8), _mm_shuffle_ps(b,b,0xE1));
	float4 bres = _mm_mul_ps(_mm_shuffle_ps(a,a,0xE1), _mm_shuffle_ps(b,b,0xD8));
	out = _mm_mul_ps(ares,bres);
};

void fnormalize(const float4 &a, float4 &out)
{
	__m128 l = _mm_mul_ps(a, a);
	l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
	out = _mm_div_ps(a, _mm_sqrt_ps(_mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11))));
};

void fnormalize(float4 *a)
{
	__m128 l = _mm_mul_ps(*a, *a);
	l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
	*a = _mm_div_ps(*a, _mm_sqrt_ps(_mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11))));
};

float4 negate(const float4 &v)
{
	return _mm_xor_ps(v, SIGNMASK);
};

float4 fabsolute(const float4 &v)
{
	return _mm_andnot_ps(SIGNMASK, v);
};

float4 operator-(const float4 &v) { return negate(v); };

float4 operator+(const float4 &a, const float4 &b) { return fadd(a,b); };
float4 operator-(const float4 &a, const float4 &b) { return fsub(a,b); };
float4 operator*(const float4 &a, const float4 &b) { return fmul(a,b); };
float4 operator/(const float4 &a, const float4 &b) { return fdiv(a,b); };

float4 operator+(const float4 &a, const f32 &b) { return fadd(a,finit(b)); };
float4 operator-(const float4 &a, const f32 &b) { return fsub(a,finit(b)); };
float4 operator*(const float4 &a, const f32 &b) { return fmul(a,finit(b)); };
float4 operator/(const float4 &a, const f32 &b) { return fdiv(a,finit(b)); };

void operator+=(float4 &a, const float4 &b) { a = fadd(a,b); };
void operator-=(float4 &a, const float4 &b) { a = fsub(a,b); };
void operator*=(float4 &a, const float4 &b) { a = fmul(a,b); };
void operator/=(float4 &a, const float4 &b) { a = fdiv(a,b); };

void operator+=(float4 &a, const f32 &b) { a = fadd(a, finit(b)); };
void operator-=(float4 &a, const f32 &b) { a = fsub(a, finit(b)); };
void operator*=(float4 &a, const f32 &b) { a = fmul(a, finit(b)); };
void operator/=(float4 &a, const f32 &b) { a = fdiv(a, finit(b)); };

float4 operator^(const float4 &a, const float4 &b) { return fcross(a,b); };
float4 operator^(const float4 &a, const f32 &b) { return fcross(a,finit(b)); };
void operator^=(float4 &a, const float4 &b) { a = fcross(a,b); };
void operator^=(float4 &a, const f32 &b) { a = fcross(a,finit(b)); };