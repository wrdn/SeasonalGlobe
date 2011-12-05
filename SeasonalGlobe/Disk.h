#pragma once

#include "Model.h"

// disc with grass (etc) on it
class Disk : public Model
{
private:
	f32 radius; u32 slices;
public:
	Disk(void);
	~Disk(void);

	const f32 GetRadius() const { return radius; };
	const u32 GetSlices() const { return slices; };

	bool Create(u32 slices, f32 radius);
};

