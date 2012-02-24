#pragma once
#include "GraphicsObject.h"
#include <vector>
using namespace std;

enum TerrainShiftDirection { NoShift=0, Up=1, Down=-1 };
struct TerrainShift
{
public:
	f32 maxDisplacementScaleFactor,
		maxGeometryShiftFactor; // translation on Y axis to put terrain back in base
	f32 timeToElevateFully;
	f32 elevationRuntime; // updated by dt each frame
	TerrainShiftDirection shiftDirection;

	TerrainShift() : maxDisplacementScaleFactor(0), maxGeometryShiftFactor(0),
		timeToElevateFully(0), elevationRuntime(0), shiftDirection(NoShift) {};
	TerrainShift(f32 _maxDisplacementScaleFactor, f32 _maxGeometryShiftFactor, f32 _timeToEvalateFully)
		: maxDisplacementScaleFactor(_maxDisplacementScaleFactor), maxGeometryShiftFactor(_maxGeometryShiftFactor),
		timeToElevateFully(_timeToEvalateFully), elevationRuntime(0), shiftDirection(NoShift) {};
	~TerrainShift() {};
};

struct TerrainShaders
{
public:
	u32 Terrain_Displacement_Ambient_ShaderID;
	u32 Terrain_Displacement_Directional_ShaderID;
	u32 Terrain_Displacement_Spotlights_ShaderID;

	TerrainShaders() : Terrain_Displacement_Ambient_ShaderID(0), Terrain_Displacement_Directional_ShaderID(0),
		Terrain_Displacement_Spotlights_ShaderID(0) {};
	~TerrainShaders() {};
};

struct ImageRowInfo
{
	Vec2i pixelStart, pixelEnd;
	u32 pixelCount;
	u32 firstVertexIndex, lastVertexIndex;
};

class TerrainLoader : public GraphicsObject
{
private:
	u32 width, height;

	// storing these allows us to rebuild the
	// terrain if neccessary
	vector<ImageRowInfo> rows;
public:
	TerrainLoader(void);
	~TerrainLoader(void);
	
	bool Load(char *filename);

	const u32 GetHeightmapWidth() const { return width; }
	const u32 GetHeightmapHeight() const { return height; }
};

