#ifndef __MODEL_H__
#define __MODEL_H__

#include "ctypes.h"
#include "util.h"
#include "float2.h"
#include "float3.h"
#include <GXBase.h>

const GLvoid* BUFFER_OFFSET(const u32 i);

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
	//f32 pos[3]; // 12
	//f32 norm[3]; // 12
	//f32 uvs[2]; // 8

	float3 pos, normal;
	float2 uvs;

	// NOTE: Remember to change these if you change the size/order of this structure
	// Values are in bytes and are used during VBO creation
	static const u32 POS_BUFFER_OFFSET = 0;
	static const u32 NORMAL_BUFFER_OFFSET = 12;
	static const u32 UV_BUFFER_OFFSET = 24;

	VERTEX() { };
	VERTEX(float3 _pos, float3 _normal, float2 _uv)
		: pos(_pos), normal(_normal), uvs(_uv)
	{
	};

};

enum TriangleDrawMethod
{
	DM_TRIANGLES = GL_TRIANGLES,
	DM_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	DM_POINTS = GL_POINTS,
};

class Model : public glex
{
private:
	ModelVBO mvbo;
	VERTEX *vertexArray; u32 vertexArraySize;
	u32 *indicesArray; u32 indicesArraySize;
	u32 triangleDrawCount; // indices to use = triangleDrawCount * 3
	GLenum drawMode;
	TriangleDrawMethod triDrawMethod;

	void DeepCopy(Model *dst) const
	{
		dst->mvbo = mvbo;
		dst->vertexArraySize = vertexArraySize;
		dst->indicesArraySize = indicesArraySize;
		dst->triangleDrawCount = triangleDrawCount;
		dst->drawMode = drawMode;
		dst->triDrawMethod = triDrawMethod;

		dst->vertexArray = new VERTEX[dst->vertexArraySize];
		dst->indicesArray = new u32[dst->indicesArraySize];
		memcpy(dst->vertexArray, vertexArray, sizeof(VERTEX) * dst->vertexArraySize);
		memcpy(dst->indicesArray, indicesArray, sizeof(u32) * dst->indicesArraySize);
	};

public:
	Model();
	virtual ~Model();

	Model(Model const& other) // copy constructor
	{
		other.DeepCopy(this);
	};
	Model& operator= (Model const& other)
	{
		if(this != &other)
		{
			if(mvbo.modeldata_vboid) { glDeleteBuffers(1, &mvbo.modeldata_vboid); }
			if(mvbo.indices_vboid) { glDeleteBuffers(1, &mvbo.indices_vboid); }
			delete [] vertexArray;
			delete [] indicesArray;

			other.DeepCopy(this);
		}
		return *this;
	};

	const ModelVBO& GetModelVBO() const { return mvbo; };

	const VERTEX * const GetVertexArray() const { return vertexArray; };
	const u32 GetVertexArraySize() const { return vertexArraySize; };
	void SetVertexArray(VERTEX * _vertexArray, const u32 _vertexArraySize)
	{
		if(vertexArray)
			delete [] vertexArray;

		/*vertexArraySize = _vertexArraySize;
		vertexArray = new VERTEX[vertexArraySize];
		memcpy(vertexArray, _vertexArray, sizeof(VERTEX)*vertexArraySize);*/
		vertexArray = _vertexArray;
		vertexArraySize = _vertexArraySize;
	};

	const u32 * const GetIndicesArray() const { return indicesArray; };
	const u32 GetIndicesArraySize() const { return indicesArraySize; };
	void SetIndicesArray(u32 * _indicesArray, const u32 _indicesArraySize)
	{
		//SAFE_DELETE_ARRAY(indicesArray);
		if(indicesArray)
			delete [] indicesArray;

		indicesArraySize = _indicesArraySize;
		indicesArray = _indicesArray;
		/*indicesArraySize = _indicesArraySize;
		indicesArray = new u32[indicesArraySize];
		memcpy(indicesArray, _indicesArray, sizeof(u32)*indicesArraySize);*/
		//indicesArray = _indicesArray;
	};

	const u32 GetTriangleDrawCount() const { return triangleDrawCount; };
	void RecalculateTriangleDrawCount() { triangleDrawCount = indicesArraySize / 3; };
	void SetTriangleDrawCount(const u32 v) { triangleDrawCount = v; };

	void SetDrawMethod(TriangleDrawMethod method) // points, triangle, triangle strip
	{
		triDrawMethod = method;
	};

	const GLenum GetDrawMode() const { return drawMode; };
	void SetDrawMode(const GLenum m) { drawMode = m; }; // GL_FILL, GL_LINE, GL_POINTS etc

	bool BuildVBO();
	void Draw(); // dumb function, does no data checking

	bool Valid() const; // returns true if the VBO is valid (IDs != 0)
};

#endif