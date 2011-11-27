#pragma once

#include <vector>
#include "Model.h"

class TerrainDisk : public Model
{
private:
	std::vector<VERTEX> _vertices;
	std::vector<u32> _indices;
public:
	TerrainDisk();
	~TerrainDisk();

	bool CreateTerrainDisk(const c8 * const heightmap_filename);
};