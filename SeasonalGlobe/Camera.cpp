#include "Camera.h"

const f32 Camera::DEFAULT_FOV = 45.0f;
const float3 Camera::DEFAULT_UP = float3(0,1,0);

/*
Camera::Camera(void) : position(), direction(), up(DEFAULT_UP), fov(DEFAULT_FOV)
{
}

Camera::~Camera(void)
{
}
*/

const float3& Camera::GetPosition() const
{
	return position;
};

const float3& Camera::GetDirection() const
{
	return direction;
};

const float3& Camera::GetUp() const
{
	return up;
};

const f32 Camera::GetFOV() const
{
	return fov;
};

void Camera::SetPosition(const float3& p)
{
	position = p;
};

void Camera::SetDirection(const float3& d)
{
	direction = d;
};

void Camera::SetUp(const float3& u)
{
	up = u;
};

void Camera::SetFOV(const f32 _fov)
{
	fov = _fov;
};