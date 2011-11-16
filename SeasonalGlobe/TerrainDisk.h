#pragma once

#include <vector>
#include "float3.h"
#include "float2.h"
#include "CustomModel.h"

struct Face;
class TerrainDisk : public CustomModel
{
private:
	std::vector<float3> _vertices;
	std::vector<float2> _tex; // uvs
	std::vector<Face> _faces;
public:
	TerrainDisk();
	~TerrainDisk();

	bool CreateTerrainDisk(const c8 * const heightmap_filename);
	
	void Draw(const bool drawPoints);
};