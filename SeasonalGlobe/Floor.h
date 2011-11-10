#pragma once

#include "Model.h"

// disc with grass (etc) on it
class Floor
{
private:
	Model floorModel;
	f32 radius; u32 slices;
public:
	Floor(void);
	~Floor(void);

	void CreateFloor(u32 slices, f32 radius);

	void Draw()
	{
		floorModel.Draw();
	};
};

