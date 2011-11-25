#pragma once

#include <vector>
#include "float3.h"
#include "float2.h"
#include "Model.h"
 
struct Face
{
public:
	VERTEX pos1, pos2, pos3;
};

class TerrainDisk : public Model
{
private:
	//std::vector<float2> _tex; // uvs
	std::vector<Face> _faces;
	std::vector<VERTEX> _vertices;
public:
	TerrainDisk();
	~TerrainDisk();

	bool CreateTerrainDisk(const c8 * const heightmap_filename);
	
	void Draw(const bool drawPoints);

	std::vector<VERTEX> &GetVertices() { return _vertices; };
	std::vector<Face> &GetFaces() { return _faces; };
};