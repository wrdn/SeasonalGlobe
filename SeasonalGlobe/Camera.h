#pragma once

#include "float3.h"

class Camera
{
private:
	static const f32 DEFAULT_FOV; // =45 degrees
	static const float3 DEFAULT_UP; // =(0,1,0)

	float3 position, direction, up;
	f32 fov;
public:
	Camera(void);
	~Camera(void);

	// Accessors and Mutators
	const float3& GetPosition() const;
	const float3& GetDirection() const;
	const float3& GetUp() const;
	const f32 GetFOV() const;

	void SetPosition(const float3& p);
	void SetDirection(const float3& d);
	void SetUp(const float3& u);
	void SetFOV(const f32 _fov);

};

