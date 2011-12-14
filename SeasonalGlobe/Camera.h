#pragma once

#include "Mat44.h"
#include "float3.h"
#include "float4.h"
#include <GXBase.h>

class Camera
{
private:
	static const f32 DEFAULT_FOV; // =45 degrees
	static const float3 DEFAULT_UP; // =(0,1,0)

	float3 position, direction, up, right;
	f32 fov;
	f32 aspect;
	f32 nearplane;
	f32 farplane;

public:
	Camera(void) : position(), direction(0,0,-1), up(0,1,0), fov(45), aspect(1.333f), nearplane(0.3f), farplane(100.0f)
	{
		right = up; right ^= direction;
		right.normalize();
	};
	~Camera(void)
	{
	};

	// Accessors and Mutators
	const float3& GetPosition() const;
	const float3& GetDirection() const;
	const float3& GetUp() const;
	const f32 GetFOV() const;
	const float3& GetRight() const { return right; };
	const f32 GetAspect() const { return aspect; };
	const f32 GetNearPlane() const { return nearplane; };
	const f32 GetFarPlane() const { return farplane; };

	void SetPosition(const float3& p);
	void SetDirection(const float3& d);
	void SetUp(const float3& u);
	void SetFOV(const f32 _fov);
	void SetAspect(const f32 _aspect) { aspect = _aspect; };
	void SetNear(const f32 _nearplane) { nearplane = _nearplane; };
	void SetFar(const f32 _farplane) { farplane = _farplane; };

	void Update() const
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		/*gluLookAt(position.x(), position.y(), position.z(),
			direction.x(), direction.y(), direction.z(),
			up.x(), up.y(), up.z());*/

		/*gluLookAt(position.x(), position.y(), position.z(),
			direction.x(), direction.y(), direction.z(),
			up.x(), up.y(), up.z());*/

		// build projection matrix
		f32 mat[16];
		
		mat[m11] = right.x();
		mat[m12] = up.x();
		mat[m13] = -direction.x();
		mat[m14] = 0;

		mat[m21] = right.y();
		mat[m22] = up.y();
		mat[m23] = -direction.y();
		mat[m24] = 0;

		mat[m31] = right.z();
		mat[m32] = up.z();
		mat[m33] = -direction.z();
		mat[m34] = 0;

		mat[m41] = -right.dot(position);
		mat[m42] = -up.dot(position);
		mat[m43] = direction.dot(position);
		mat[m44] = 1.0f;

		glMultMatrixf(mat);

	};

	
	void Init(const f32 _fov, const f32 _aspect, const f32 _near, const f32 _far,
		const float3 &_pos, const float3 &_dir, const float3 &_up)
	{
		fov = _fov;
		aspect = _aspect;
		nearplane = _near;
		farplane = _far;

		position = _pos;
		direction = _dir;
		up = _up;

		Init();
	};

	void Init() const
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, aspect, nearplane, farplane);
		glMatrixMode(GL_MODELVIEW);

		Update();
	};

	void Translate(const float3 &v)
	{
		position += v;
		Update();
	};

	void Rotate(const Mat44 &rotationMatrix)
	{
		float4 dirvec = direction.ToFloat4();
		direction = rotationMatrix.Mult(dirvec).ToFloat3();
		direction.normalize();

		float4 upvec = up.ToFloat4();
		up = rotationMatrix.Mult(upvec).ToFloat3();
		up.normalize();

		right = direction; right ^= up;
		right.normalize();

		up = right; up ^= direction;
		up.normalize();

		Update();
	};
};

