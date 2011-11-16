#pragma once

#include "CustomModel.h"

// disc with grass (etc) on it
class Floor : public CustomModel
{
private:
	f32 radius; u32 slices;
public:
	Floor(void);
	~Floor(void);

	const f32 GetRadius() const { return radius; };
	const u32 GetSlices() const { return slices; };

	void CreateFloor(u32 slices, f32 radius);
};

