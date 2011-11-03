#ifndef __MODEL_H__
#define __MODEL_H__

#include "ctypes.h"

//! Constructor will ensure both vboids are set to 0 originally
struct ModelVBO // 8 bytes
{
public:
	u32 modeldata_vboid;
	u32 indices_vboid;

	ModelVBO();
	~ModelVBO();
};

// Required model data:
// Vertex data (4D SIMD, SOA)
// Normal data (4D SIMD, SOA)
// UV Data (2D, SOA)

// Other:
// Transformation matrix, or pos (vec), scale (float for uniform, vec for nonuniform), rotation (quaternion? euler vec)

// INDICES_PER_TRIANGLE = 3 (vertices only), 6 (vertices and normals), 8 (vertices, normals and UVs)
// Note UVs could be 3D, thus INDICES_PER_TRIANGLE would equal 9. However, as we mostly deal with 2D data, we can save a channel and use 8
//#define INDICES_PER_TRIANGLE 6

// Generic Model class supporting vertices, normals and texture coordinates (UVs). Any model file loaders (e.g. obj loader)
// should load the data into a Model, so there is a single point of maintenance for data. Feel free to add model loading classes
// (e.g. OBJLoader) to contain specifics to that format. The Model object may be embedded within the format (composition)
class Model
{
private:
	i32 INDICES_PER_TRIANGLE; // should be a minimum of 6, rebuild normals if they don't exist (UVs not always required though)

public:
	Model(void);
	~Model(void);

	// Vertices
	i32 vertexCount;
	//f32 *vx, *vy, *vz;
	f32 *vertex_data;

	// Normals
	i32 normalCount;
	//f32 *nx, *ny, *nz;
	f32 *normal_data;

	// UVs
	i32 uvCount;
	//f32 *tu, *tv;
	f32 *uv_data;

	// generated texture ids (the texture info is not required,
	// only the ID the texture is bound to)
	u32 texCount;
	u32 *textures;

	// size of triSet array is INDICES_PER_TRIANGLE * triCount.
	// To get to triangle N: triSet[INDICES_PER_TRIANGLE * N]
	// triSet has format Vertex-Normal-TexCoord -- Vertex-Normal-TexCoord...
	// Therefore, for full data (vertex,normal,texcoord), indices per triangle is 9
	u32 triCount;
	u32 *triSet;

	void RecalculateNormals();

	// OpenGL specific
	ModelVBO BuildVBO(); // creates a vbo, sets the data then returns its id. If an ID cannot be generated, it returns 0

	i32 GetIndicesPerTriangle();
	bool SetIndicesPerTriangle(c8 indicesPerTri); // min=6, max=9
};

#endif