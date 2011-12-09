#pragma once

#include "Mat44.h"
#include "float3.h"
#include "ctypes.h"

class Camera2
{
public:
	float3 pos, dir, up, right;
	f32 nearPlane, farPlane, fov;

public:
	Camera2();
	~Camera2();

	const float3& GetPosition() const { return pos; }
	const float3& GetDirection() const { return dir; }
	const float3& GetUp() const { return up; }
	const float3& GetRight() const { return right; }
	const f32 GetNear() const { return nearPlane; }
	const f32 GetFar() const { return farPlane; }
	const f32 GetFOV() const { return fov; }

	void Init(const float3 &camPos, const float3& camDir, const float3& camUp);

	// call when you change window size
	void UpdateProjection(const f32 fov, const f32 aspect, const f32 nearplane, const f32 farplane);

	void Translate(const float3 &f, const f32 t);
	void Rotate(const f32 angle_in_degrees, const float3& rotationVector);

	void Update();

	void MoveForward(const f32 t);
	void MoveBackwards(const f32 t);

	void MoveLeft(const f32 t);
	void MoveRight(const f32 t);
	
	void MoveUp(const f32 t);
	void MoveDown(const f32 t);
};