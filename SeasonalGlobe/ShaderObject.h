#pragma once

#include "ctypes.h"
#include <iosfwd>

#include "Mat44.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "Texture.h"
#include "GameResource.h"
#include <GXBase.h>

// Used to represent a vertex or fragment shader, allowing its compilation
// Since the only difference between the compilation and storage of a vertex/fragment shader is
// the type is GL_VERTEX_SHADER or GL_FRAGMENT_SHADER, we can use a single class with a type variable
// Note however, the type is private and cannot be accessed nor set manually.
// By default, the type variable is GL_VERTEX_SHADER. To create a fragment shader, use FragmentShaderObject (see below)
class ShaderObject : public Resource, public glex
{
private:
	GLenum shaderType; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER. Default: GL_VERTEX_SHADER
	GLuint shaderID; // OpenGL id of the shader

	// Deletes OpenGL shader
	void Unload();

protected:
	void SetShaderType(GLenum type) { shaderType = type; }; // used so FragmentShaderObject can set the shader type
public:
	ShaderObject() : shaderType(GL_VERTEX_SHADER), shaderID(0)
	{
		glex::Load();
	};
	~ShaderObject()
	{
		Unload();
	};

	// returns true if the OpenGL shader id is valid
	bool Valid()
	{
		return shaderID != 0;
	};

	GLuint GetShaderID() const { return shaderID; };

	// Did the shader compile OK
	bool HasCompiled();
	
	// Compilation log
	void PrintShaderLog(std::ostream &out);

	// Compile the shader from source
	bool Compile(const c8 *src);

	// An easily utility function that loads shader from file then calls Compile(src)
	// on the code. Unless specific functionality is required, use this function
	// to avoid worries about memory leaks etc
	bool CompileFromFile(const char *filename);
};

// Same as ShaderObject, except the shaderType is GL_FRAGMENT_SHADER
class FragmentShaderObject : public ShaderObject
{
public:
	// only difference between vertex and fragment shader is the type variable used in glCreateShader(type)
	FragmentShaderObject() { SetShaderType(GL_FRAGMENT_SHADER); };
};

// Handles to vertex, fragment shader and generic shader object
typedef ShaderObject VertexShaderObject;
typedef std::tr1::shared_ptr<ShaderObject> GenericShaderHandle;
typedef std::tr1::shared_ptr<VertexShaderObject> VertexShaderHandle;
typedef std::tr1::shared_ptr<FragmentShaderObject> FragmentShaderHandle;