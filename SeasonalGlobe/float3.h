#pragma once

#include "ctypes.h"
#include <iostream>

class float4;

// 3D vector supporting common operations including addition, subtraction, division, multiplication,
// dot product, cross product, magnitude and normalization, negation and lerp
class float3
{
private:
	f32 vec[3]; // x=vec[0], y=vec[1], z=vec[2]

public:
	f32 * GetVec() const { return (f32*)vec; }; // returns vector array

	static const float3 ZERO;
	static const float3 ONE;

	// Accessors for x, y and z
	f32 x() const { return vec[0]; };
	f32 y() const { return vec[1]; };
	f32 z() const { return vec[2]; };

	f32 r() const { return x(); };
	f32 g() const { return y(); };
	f32 b() const { return z(); };

	// Mutators for x, y and z
	void x(const f32 v) { vec[0]=v; };
	void y(const f32 v) { vec[1]=v; };
	void z(const f32 v) { vec[2]=v; };

	// set vector based on individual values, or an array
	void set(const f32 x, const f32 y, const f32 z)
	{
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
	};
	void set(const f32 * const v)
	{
		vec[0] = v[0];
		vec[1] = v[1];
		vec[2] = v[2];
	};

	~float3();
	float3(); // sets all to 0
	explicit float3(const f32 v);
	float3(const f32 _x, const f32 _y, const f32 _z);
	explicit float3(const f32 * v);

	float4 ToFloat4() const;

	void zero(); // zero's the current vector
	
	void setall(const f32 v); // set x, y and z to "v"

	// vector-vector operations
	float3 add(const float3 &v) const;
	float3 sub(const float3 &v) const;
	float3 mul(const float3 &v) const;
	float3 div(const float3 &v) const;
	
	// vector-scalar operations
	float3 add(const f32 t) const;
	float3 sub(const f32 t) const;
	float3 mul(const f32 t) const;
	float3 div(const f32 t) const;

	// scalar dot product
	f32 dot(const float3 &v) const;

	// vector cross product
	float3 cross(const float3 &v) const;
	void cross(const float3 &v, f32 * const output3d) const;

	// vector normalization
	float3 normalize() const;
	void normalize();

	// length and length squared of vector
	f32 magnitude() const;
	f32 LengthSquared() const;

	// distance and distance squared between 2 vectors
	f32 distance(const float3 &v) const;
	f32 DistanceSquared(const float3 &v) const;

	// returns negated vector, negative becomes positive and vice versa
	float3 negate() const;
	
	// returns absolute vector (all values positive)
	float3 absolute() const;

	// lerp between 2 vectors based on lerpFactor between 0 and 1
	float3 vec_lerp(const float3 &target, f32 lerpFactor);

	// Addition, Subtraction, Division, Multiplication and Cross Product between another vector and self
	inline const float3& operator+=(const float3 &rhs) { *this = add(rhs); return *this; }
	inline const float3& operator-=(const float3 &rhs) { *this = sub(rhs); return *this; }
	inline const float3& operator*=(const float3 &rhs) { *this = mul(rhs); return *this; }
	inline const float3& operator/=(const float3 &rhs) { *this = div(rhs); return *this; }
	inline const float3& operator^=(const float3 &rhs) { *this = cross(rhs); return *this; }
};

// Auxiliary Functions (operator overloads)

// vector-vector operations
inline float3 operator+(const float3 &a, const float3 &b) { return float3(a)+=b; }
inline float3 operator-(const float3 &a, const float3 &b) { return float3(a)-=b; }
inline float3 operator*(const float3 &a, const float3 &b) { return float3(a)*=b; }
inline float3 operator/(const float3 &a, const float3 &b) { return float3(a)/=b; }

// vector-scalar operations e.g. vector + scalar
inline float3 operator+(const float3 &a, const f32 b) { return float3(a)+=float3(b); }
inline float3 operator-(const float3 &a, const f32 b) { return float3(a)-=float3(b); }
inline float3 operator*(const float3 &a, const f32 b) { return float3(a)*=float3(b); }
inline float3 operator/(const float3 &a, const f32 b) { return float3(a)/=float3(b); }

// scalar-vector operations e.g. scalar + vector
inline float3 operator+(const f32 a, const float3 &b) { return float3(a)+=b; }
inline float3 operator-(const f32 a, const float3 &b) { return float3(a)-=b; }
inline float3 operator*(const f32 a, const float3 &b) { return float3(a)*=b; }
inline float3 operator/(const f32 a, const float3 &b) { return float3(a)/=b; }

// cross product
inline float3 operator^(const float3 &a, const float3 &b) { return float3(a)^=b; }

// stream out, format "{1,2,3}"
std::ostream& operator<<(std::ostream &out, const float3 &m);

// stream in, format "1 2 3"
std::istream& operator>>(std::istream &in, float3& out);