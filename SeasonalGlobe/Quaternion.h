#pragma once

#include <assert.h>

#include "float3.h"
#include "float4.h"
#include "Mat44.h"

#ifdef _DEBUG
//#define CHECK_NORMALISED(v) assert(NearOne(v))
#define CHECK_NORMALISED(v)
#else
#define CHECK_NORMALISED(v)
#endif

class Quaternion
{
private:
	f32 qvec[4]; // x=qvec[0], y=qvec[1], z=qvec[2], w=qvec[3]

	// length squared
	f32 length_sq() const { return (x()*x()) + (y()*y()) + (z()*z()) + (w()*w()); };
public:
	Quaternion();
	Quaternion(const f32 x, const f32 y, const f32 z, const f32 w);
	Quaternion(const float3 &v); // quaternion from vector
	Quaternion(const float3& axis, const f32 angle_in_radians); // quaternion from angle-axis

	void identity(); // sets self to identity quaternion

	// Accessors and Mutators
	inline f32 x() const { return qvec[0]; }
	inline f32 y() const { return qvec[1]; }
	inline f32 z() const { return qvec[2]; }
	inline f32 w() const { return qvec[3]; }

	inline void x(const f32 v) { qvec[0] = v; }
	inline void y(const f32 v) { qvec[1] = v; }
	inline void z(const f32 v) { qvec[2] = v; }
	inline void w(const f32 v) { qvec[3] = v; }

	inline void set(const f32 v) { set(v,v,v,v); };
	inline void set(const f32 _x, const f32 _y, const f32 _z) { x(_x); y(_y); z(_z); w(0); };
	inline void set(const f32 _x, const f32 _y, const f32 _z, const f32 _w) { x(_x); y(_y); z(_z); w(_w); };
	inline void set(const float3 &v, const f32 w) { set(v.x(), v.y(), v.z(), w); }
	inline void set(const float3 &v) { set(v.x(), v.y(), v.z(), 0.0f); };

	inline float3 tofloat3() const { return float3(x(), y(), z()); }

	Quaternion add(const Quaternion &q) const; // add
	Quaternion sub(const Quaternion &q) const; // subtract
	Quaternion mult(const f32 scale) const; // multiply by scalar
	Quaternion mult(const Quaternion &q) const; // multiply by another quaternion
	f32 magnitude() const; // get quaternion magnitude (length)
	Quaternion normalize() const; // normalize quaternion
	Quaternion conjugate() const; // same as inverse for unit length quaternion
	f32 dot(const Quaternion &q) const; // quaternion dot (scalar) product
	static Quaternion FromAxisAngle(const float3 &axis, const f32 angle_in_radians); // w=cos(angle/2), xyz=axis.xyz*sin(angle)
	static Quaternion FromEulerAngles(const f32 yaw, const f32 pitch, const f32 roll);
	
	Mat44 ToMat44() const; // create 4x4 rotation matrix from quaternion

	void ExtractAxisAngle(float3 &out_axis, f32 &out_angle) const; // get angle and axis from quaternion

	float3 rotate(const float3& v) const; // q*v*qinverse, rotate vector using quaternion

	// lerp from 'this' to q using 0 < t < 1
	Quaternion lerp(const Quaternion &q, const f32 t) const;
	static Quaternion lerp(const Quaternion &q1, const Quaternion &q2, const f32 t);

	// spherical interpolation between 2 quaternions using 0 < t < 1
	Quaternion slerp(const Quaternion &q, const f32 t) const;
	static Quaternion slerp(const Quaternion &q1, const Quaternion &q2, const f32 t);

	// Add, subtract and multiply another quaternion with self
	inline const Quaternion& operator+=(const Quaternion &rhs) { *this = add(rhs); return *this; };
	inline const Quaternion& operator-=(const Quaternion &rhs) { *this = sub(rhs); return *this; };
	inline const Quaternion& operator*=(const Quaternion &rhs) { *this = mult(rhs); return *this; };
	inline const Quaternion& operator*=(const f32 scale) { *this = mult(scale); return *this; };
};

// quaternion-quaternion operations (returning a new quaternion)
inline Quaternion operator+(const Quaternion &a, const Quaternion &b) { return Quaternion(a)+=b; }
inline Quaternion operator-(const Quaternion &a, const Quaternion &b) { return Quaternion(a)-=b; }
inline Quaternion operator*(const Quaternion &a, const Quaternion &b) { return Quaternion(a)*=b; }
inline Quaternion operator*(const Quaternion &a, const f32 &b) { return Quaternion(a)*=b; }
inline Quaternion operator*(const f32 &a, const Quaternion &b) { return Quaternion(b)*=a; }