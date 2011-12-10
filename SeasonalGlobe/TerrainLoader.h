#pragma once
#include "GraphicsObject.h"
#include <vector>
using namespace std;

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

