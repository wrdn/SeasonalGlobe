#pragma once
#include <GXBase.h>
#include "Model.h"
#include <vector>
using namespace std;

struct Face
{
public:
	VERTEX v1, v2, v3;

	Face() { };
	Face(VERTEX _v1, VERTEX _v2, VERTEX _v3)
		: v1(_v1), v2(_v2), v3(_v3) { };
	~Face() { };
};
struct ImageRowInfo
{
	Vec2i pixelStart, pixelEnd;
	u32 pixelCount; // pixelEnd - pixelStart
	
	u32 firstVertexIndex, lastVertexIndex;
};

class TerrainLoader
{
public:
	float (*vertexarray)[3];
	float (*normalarray)[3];
	float (*texarray)[2];
	GLuint *indexarray;
	int width, height;

	TerrainLoader(void);
	~TerrainLoader(void);

	vector<VERTEX> verts;
	vector<Face> faces;
	vector<ImageRowInfo> rows;

	void Load(char *filename);

	void Draw();
};

