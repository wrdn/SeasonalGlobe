#include "Camera2.h"
#include "float4.h"
#include <GXBase.h>

Camera2::Camera2() : nearPlane(0.3f), farPlane(100), fov(45)
{
};

Camera2::~Camera2()
{
};

void Camera2::Init(const float3 &camPos, const float3& camDir, const float3& camUp)
{
	pos = camPos;
	dir = camDir.normalize();
	up = camUp.normalize();
	right = dir ^ up; right.normalize();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos.x(), pos.y(), pos.z(), dir.x(), dir.y(), dir.z(), up.x(), up.y(), up.z());
};

void Camera2::UpdateProjection(const f32 fov, const f32 aspect, const f32 nearplane, const f32 farplane)
{
	nearPlane = nearplane; farPlane = farplane;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
};

void Camera2::Translate(const float3 &f, const f32 t)
{
	pos += (f * t);
};

void Camera2::Rotate(const f32 angle_in_degrees, const float3& rotationVector)
{
	Mat44 r = Mat44::BuildRotationMatrix(angle_in_degrees, rotationVector.x(), rotationVector.y(),
		rotationVector.z());

	dir = r.Mult(dir.ToFloat4()).ToFloat3();
	up = r.Mult(up.ToFloat4()).ToFloat3();
};

void Camera2::Update()
{
	dir.normalize();
	
	right = dir ^ up;
	right.normalize();

	up = right ^ dir;
	up.normalize();

	gluLookAt(pos.x(), pos.y(), pos.z(), dir.x(), dir.y(), dir.z(), up.x(), up.y(), up.z());
};

void Camera2::MoveForward(const f32 t)
{
	pos += dir*t;
};
void Camera2::MoveBackwards(const f32 t)
{
	pos -= dir*t;
};

void Camera2::MoveLeft(const f32 t)
{
	pos -= right*t;
};
void Camera2::MoveRight(const f32 t)
{
	pos += right*t;
};

void Camera2::MoveUp(const f32 t)
{
	pos += up*t;
};
void Camera2::MoveDown(const f32 t)
{
	pos -= up*t;
};