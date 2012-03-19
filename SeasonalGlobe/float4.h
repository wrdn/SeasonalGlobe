#pragma once

#include "ctypes.h"
#include <iostream>

class float3;

// 3D vector supporting common operations including addition, subtraction, division, multiplication,
// dot product, cross product (vector w ignored), magnitude and normalization, negation and lerp
class float4
{
private:
	f32 vec[4]; // x=vec[0], y=vec[1], z=vec[2], w=vec[3]

public:
	f32 * GetVec() const { return (f32*)vec; }; // returns vector (useful for opengl operations requiring vector array)

	static const float4 ZERO;
	static const float4 ONE;

	// Constructors
	float4(); // sets all to 0
	explicit float4(const f32 v);
	float4(const f32 _x, const f32 _y, const f32 _z);
	float4(const f32 _x, const f32 _y, const f32 _z, const f32 _w);
	explicit float4(const f32 * v); // copy from 4 element array

	// Accessors
	f32 x() const { return vec[0]; };
	f32 y() const { return vec[1]; };
	f32 z() const { return vec[2]; };
	f32 w() const { return vec[3]; };

	f32 r() const { return x(); };
	f32 g() const { return y(); };
	f32 b() const { return z(); };
	f32 a() const { return w(); };

	// Mutators
	void x(const f32 v) { vec[0]=v; };
	void y(const f32 v) { vec[1]=v; };
	void z(const f32 v) { vec[2]=v; };
	void w(const f32 v) { vec[3]=v; };

	void set(f32 _x, f32 _y, f32 _z, f32 _w)
	{
		x(_x); y(_y);
		z(_z), w(_w);
	}

	float3 ToFloat3() const;

	void zero(); // zero's the current vector

	// set everything to v
	void setall(const f32 v);

	// vector-vector operations
	float4 add(const float4 &v) const;
	float4 sub(const float4 &v) const;
	float4 mul(const float4 &v) const;
	float4 div(const float4 &v) const;
	
	// vector-scalar operations, equivalent to vector operations with a vector full of the scalar
	float4 add(const f32 t) const;
	float4 sub(const f32 t) const;
	float4 mul(const f32 t) const;
	float4 div(const f32 t) const;

	// dot product
	f32 dot(const float4 &v) const;

	// 3D cross product (w set to 0)
	float4 cross(const float4 &v) const;
	void cross(const float4 &v, f32 * const output3d) const;

	// normalize 4D vector
	float4 normalize() const;
	void normalize();

	// length and length squared
	f32 magnitude() const;
	f32 LengthSquared() const;

	// distance and distance squared
	f32 distance(const float4 &v) const;
	f32 DistanceSquared(const float4 &v) const;

	// negate vector (inverts sign of each element)
	float4 negate() const;

	// absolute vector (all positive)
	float4 absolute() const;

	// lerp between "this" and "target" based on lerpFactor between 0 and 1
	float4 vec_lerp(const float4 &target, f32 lerpFactor);

	// Addition, Subtraction, Division, Multiplication and Cross Product between another vector and self
	inline const float4& operator+=(const float4 &rhs) { *this = add(rhs); return *this; }
	inline const float4& operator-=(const float4 &rhs) { *this = sub(rhs); return *this; }
	inline const float4& operator*=(const float4 &rhs) { *this = mul(rhs); return *this; }
	inline const float4& operator/=(const float4 &rhs) { *this = div(rhs); return *this; }
	inline const float4& operator^=(const float4 &rhs) { *this = cross(rhs); return *this; }
};

// Auxiliary Functions (operator overloads)

// vector-vector operations
inline float4 operator+(const float4 &a, const float4 &b) { return float4(a)+=b; }
inline float4 operator-(const float4 &a, const float4 &b) { return float4(a)-=b; }
inline float4 operator*(const float4 &a, const float4 &b) { return float4(a)*=b; }
inline float4 operator/(const float4 &a, const float4 &b) { return float4(a)/=b; }

// vector-scalar operations
inline float4 operator+(const float4 &a, const f32 b) { return float4(a)+=float4(b); }
inline float4 operator-(const float4 &a, const f32 b) { return float4(a)-=float4(b); }
inline float4 operator*(const float4 &a, const f32 b) { return float4(a)*=float4(b); }
inline float4 operator/(const float4 &a, const f32 b) { return float4(a)/=float4(b); }

inline float4 operator+(const f32 a, const float4 &b) { return float4(a)+=b; }
inline float4 operator-(const f32 a, const float4 &b) { return float4(a)-=b; }
inline float4 operator*(const f32 a, const float4 &b) { return float4(a)*=b; }
inline float4 operator/(const f32 a, const float4 &b) { return float4(a)/=b; }

// cross product
inline float4 operator^(const float4 &a, const float4 &b) { return float4(a)^=b; }

// stream out
std::ostream& operator<<(std::ostream &out, const float4 &m);

// stream in
std::istream& operator>>(std::istream &in, float4& out);