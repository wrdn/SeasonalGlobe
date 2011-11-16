#ifndef __MODEL_H__
#define __MODEL_H__

#include "ctypes.h"
#include "util.h"
#include <GXBase.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//! Constructor will ensure both vboids are set to 0 originally
struct ModelVBO // 8 bytes
{
public:
	u32 modeldata_vboid;
	u32 indices_vboid;

	ModelVBO();
	~ModelVBO();
};

// A vertex should be duplicated as many times as normal (etc) changes
// 32 bytes
struct VERTEX
{
public:
	f32 pos[3]; // 12
	f32 norm[3]; // 12
	f32 uvs[2]; // 8
	
	// NOTE: Remember to change these if you change the size/order of this structure
	// Values are in bytes and are used during VBO creation
	static const u32 POS_BUFFER_OFFSET = 0;
	static const u32 NORMAL_BUFFER_OFFSET = 12;
	static const u32 UV_BUFFER_OFFSET = 24;
};

class Model : public glex
{
private:
	ModelVBO mvbo;
	VERTEX *vertexArray; u32 vertexArraySize;
	u32 *indicesArray; u32 indicesArraySize;
	u32 triangleDrawCount; // indices to use = triangleDrawCount * 3
	GLenum drawMode;

public:
	Model();
	~Model();

	const ModelVBO& GetModelVBO() const { return mvbo; };
	
	const VERTEX * const GetVertexArray() const { return vertexArray; };
	const u32 GetVertexArraySize() const { return vertexArraySize; };
	void SetVertexArray(VERTEX * const _vertexArray, const u32 _vertexArraySize)
	{
		SAFE_DELETE_ARRAY(vertexArray);

		vertexArray = _vertexArray;
		vertexArraySize = _vertexArraySize;
	};

	const u32 * const GetIndicesArray() const { return indicesArray; };
	const u32 GetIndicesArraySize() const { return indicesArraySize; };
	void SetIndicesArray(u32 * const _indicesArray, const u32 _indicesArraySize)
	{
		indicesArray = _indicesArray;
		indicesArraySize = _indicesArraySize;
		triangleDrawCount = indicesArraySize / 3;
	};

	const u32 GetTriangleDrawCount() const { return triangleDrawCount; };
	void RecalculateTriangleDrawCount() { triangleDrawCount = indicesArraySize / 3; };
	void SetTriangleDrawCount(const u32 v) { triangleDrawCount = v; };

	const GLenum GetDrawMode() const { return drawMode; };
	void SetDrawMode(const GLenum m) { drawMode = m; };

	bool BuildVBO();
	void Draw(); // dumb function, does no data checking
};

#endif