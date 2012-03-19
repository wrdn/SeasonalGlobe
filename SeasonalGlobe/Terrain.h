#pragma once
#include "GraphicsObject.h"
#include <vector>
using namespace std;

enum TerrainShiftDirection { NoShift=0, Up=1, Down=-1 };

//! Used during winter season when terrain mesh is displaced using displacement map
//! used to ensure the mesh stays in the same location at all times, seated in the
//! globe base
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

//! Displacement/Lighting shaders for the terrain
struct TerrainShaders
{
public:
	ShaderHandle Terrain_Displacement_Ambient_Shader;
	ShaderHandle Terrain_Displacement_Directional_Shader;
	ShaderHandle Terrain_Displacement_Spotlights_Shader;

	TerrainShaders() : Terrain_Displacement_Ambient_Shader(0), Terrain_Displacement_Directional_Shader(0),
		Terrain_Displacement_Spotlights_Shader(0) {};
	~TerrainShaders() {};
};

//! contains info to use when parsing heightmap
struct ImageRowInfo
{
	vec2i pixelStart, pixelEnd;
	u32 pixelCount;
	u32 firstVertexIndex, lastVertexIndex;
};

//! Class to load terrain from a heightmap, inherits GraphicsObject for
//! common operations (e.g. translation, rotation, scale, shaders, textures)
class Terrain : public GraphicsObject
{
private:
	u32 width, height; // width and height of heightmap

	//! storing these allows us to rebuild the
	//! terrain if neccessary
	vector<ImageRowInfo> rows;

public:
	Terrain(void);
	~Terrain(void);
	
	//! Load terrain from heightmap and create mesh
	//! By default, we build a circular mesh from the heightmap
	bool Load(char *filename);

	// Accessors
	const u32 GetHeightmapWidth() const { return width; }
	const u32 GetHeightmapHeight() const { return height; }
};

