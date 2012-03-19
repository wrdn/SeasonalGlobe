#pragma once

#include "ctypes.h"
#include <iosfwd>

// simple 2D integer vector
struct vec2i
{
public:
	i32 x,y;
	vec2i() : x(0), y(0) { };
	vec2i(const i32 _x, const i32 _y): x(_x), y(_y) { };

	void set(i32 _x, i32 _y) { x = _x; y = _y; }
};

// 2D floating point vector with common operations including: add, subtract, multiply, dot, negate and vector lerp
class float2
{
private:
	f32 vec[2]; // x=vec[0], y=vec[1]
public:
	static const float2 ZERO;
	static const float2 ONE;

	~float2();
	float2(); // sets entire vector to 0
	explicit float2(const f32 v);

	// set x and y
	explicit float2(const f32 _x, const f32 _y);

	// set vec[0] and vec[1] to v[0] and v[1]
	explicit float2(const f32 * v);

	// returns the vector pointer - this is useful in OpenGL calls which take a vector
	f32 * GetVec() const { return (f32*)vec; };

	// Accessors to x and y elements (also accessible through r() and g() functions,
	// which are more clear when dealing with colors)
	f32 x() const { return vec[0]; };
	f32 y() const { return vec[1]; };

	f32 r() const { return x(); };
	f32 g() const { return y(); };

	// set x and y individually
	void x(const f32 v) { vec[0]=v; };
	void y(const f32 v) { vec[1]=v; };

	// set x and y
	void set(const f32 _x, const f32 _y)
	{
		vec[0] = _x;
		vec[1] = _y;
	};
	
	// set x and y to values in "v" array
	void set(const f32 * const v)
	{
		vec[0] = v[0];
		vec[1] = v[1];
	};

	void zero(); // zero's the current vector

	// set everything in the vector to value v
	void setall(const f32 v);

	// Add, Subtract, Multiply and Divide by another vector
	float2 add(const float2 &v) const;
	float2 sub(const float2 &v) const;
	float2 mul(const float2 &v) const;
	float2 div(const float2 &v) const;
	
	// Add, Subtract, Multiply and Divide by a scalar (same as operations on float2 where everything set to scalar value)
	float2 add(const f32 t) const;
	float2 sub(const f32 t) const;
	float2 mul(const f32 t) const;
	float2 div(const f32 t) const;

	// scalar dot product between 2 vectors
	f32 dot(const float2 &v) const;

	// negate vector and return a new one
	float2 negate() const;

	// lerp between 2 vectors by 0 < lerpFactor < 1. Values outside 0 to 1 range of lerpFactor are undefined
	float2 vec_lerp(const float2 &target, f32 lerpFactor);

	// Addition, Subtraction, Division, Multiplication between another vector
	inline const float2& operator+=(const float2 &rhs) { *this = add(rhs); return *this; }
	inline const float2& operator-=(const float2 &rhs) { *this = sub(rhs); return *this; }
	inline const float2& operator*=(const float2 &rhs) { *this = mul(rhs); return *this; }
	inline const float2& operator/=(const float2 &rhs) { *this = div(rhs); return *this; }
};

// Auxiliary Functions (operator overloads)

// Add, Subtract, Multiply and Divide by another vector
inline float2 operator+(const float2 &a, const float2 &b) { return float2(a)+=b; }
inline float2 operator-(const float2 &a, const float2 &b) { return float2(a)-=b; };
inline float2 operator*(const float2 &a, const float2 &b) { return float2(a)*=b; };
inline float2 operator/(const float2 &a, const float2 &b) { return float2(a)/=b; };

// Add, Subtract, Multiply and Divide by scalar, vector first e.g. float2(1,2) + 3 = float2(4,5)
inline float2 operator+(const float2 &a, const f32 b) { return float2(a)+=float2(b); };
inline float2 operator-(const float2 &a, const f32 b) { return float2(a)-=float2(b); };
inline float2 operator*(const float2 &a, const f32 b) { return float2(a)*=float2(b); };
inline float2 operator/(const float2 &a, const f32 b) { return float2(a)/=float2(b); };

// Add, Subtract, Multiply and Divide by another vector, scalar first e.g. 3 + float3(1,2) = float2(4,5)
inline float2 operator+(const f32 a, const float2 &b) { return float2(a)+=b; };
inline float2 operator-(const f32 a, const float2 &b) { return float2(a)+=b; };
inline float2 operator*(const f32 a, const float2 &b) { return float2(a)+=b; };
inline float2 operator/(const f32 a, const float2 &b) { return float2(a)+=b; };

// write out vector to stream in format "1,2"
std::ostream& operator<<(std::ostream &out, float2 &m);

// read into the vector through streaming operations, format "1 2"
std::istream& operator>>(std::istream &in, float2& out);