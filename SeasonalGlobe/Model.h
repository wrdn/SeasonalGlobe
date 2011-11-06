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
	f32 *vertex_data, *normal_data, *uv_data;
	u32 *textures, *triSet; // triSet format is v/vt/vn

	// minimum if 6 (vertex pos and normal), maximum if 9 (vertex pos, normal, and vertex uv)
	u32 INDICES_PER_TRIANGLE;

	u32 vertexCount, normalCount, uvCount, texCount, triCount;

public:
	static const char FLOATS_PER_VERTEX_POS = 3; // x,y,z vertex pos
	static const char FLOATS_PER_VERTEX_NORMAL = 3; // x,y,z vertex normal
	static const char FLOATS_PER_VERTEX_UV = 2; // u,v (no 3D texture coordinates)

	Model(void);
	~Model(void);

	// The current implementation is naiive and does not account for
	// shared vertices. For smooth shading, we need to normalize the sum
	// of the normalised normals of each shared vertex
	bool RecalculatePerVertexNormals();

	// OpenGL specific
	ModelVBO BuildVBO() const; // creates a vbo, sets the data then returns its id. If an ID cannot be generated, it returns 0


	/* ACCESSORS AND MUTATORS */

	u32 GetIndicesPerTriangle() const;
	bool SetIndicesPerTriangle(c8 indicesPerTri); // min=6, max=9

	void SetVertexCount(const u32 v) { vertexCount = v; };
	void SetNormalCount(const u32 v) { normalCount = v; };
	void SetUVCount(const u32 v) { uvCount = v; };
	void SetTextureCount(const u32 v) { texCount = v; };
	void SetTriCount(const u32 v) { triCount = v; };

	const u32 GetVertexCount() const { return vertexCount; };
	const u32 GetNormalCount() const { return normalCount; };
	const u32 GetUVCount() const { return uvCount; };
	const u32 GetTextureCount() const { return texCount; };
	const u32 GetTriCount() const { return triCount; };

	void SetVertexArray(f32 * const v) { vertex_data = v; };
	void SetNormalArray(f32 * const v) { normal_data = v; };
	void SetUVArray(f32 * const v) { uv_data = v; };

	const f32* GetVertexArray() const { return vertex_data; };
	const f32* GetNormalArray() const { return normal_data; };
	const f32* GetUVArray() const { return uv_data; };

	void SetTriSet(u32 * const v) { triSet = v; };
	const u32* GetTriSet() const { return triSet; };
};

#endif