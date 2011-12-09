#pragma once

#include <vector>
#include "GraphicsObject.h"

class TerrainDisk : public GraphicsObject
{
private:
	std::vector<VERTEX> _vertices;
	std::vector<u32> _indices;
public:
	TerrainDisk();
	~TerrainDisk();

	bool CreateTerrainDisk(const c8 * const heightmap_filename);
};