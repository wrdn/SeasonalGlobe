#pragma once

#include "ctypes.h"
#include <iosfwd>

#include "Mat44.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "Texture.h"
#include "ShaderObject.h"

// Shader object collects a VertexShader and FragmentShader handle, then builds the shader program from these (which
// the application uses when drawing objects). By seperating the objects, we can create multiple shader programs,
// from any reasonable set of 2 shader objects, without ever compiling a shader object twice
class Shader : public Resource, public glex
{
private:
	GLuint shaderProgramID; // shader program ID created when the vertex and fragment shader linked together
	
	// vertex and fragment shaders
	VertexShaderHandle vs;
	FragmentShaderHandle fs;

	// Deletes shader program and calls reset() on vertex and fragment shader handle (invalidating the shared pointer
	// and decrementing its reference count)
	void Unload();
public:
	Shader() : shaderProgramID(0), vs((VertexShaderObject*)0), fs((FragmentShaderObject*)0)
	{
		glex::Load();
	};
	~Shader()
	{
		Unload();
	};

	// Creates the program from a vertex and fragment shader object
	bool CreateProgram(VertexShaderHandle vs_handle, FragmentShaderHandle fs_handle);

	// Accessors
	VertexShaderHandle GetVS() const { return vs; };
	FragmentShaderHandle GetFS() const { return fs; };

	// Did the program link
	bool HasLinked();

	// Activate shader
	void Activate();

	// Deactivate shader
	void Deactivate();

	// Get location id of uniform in shader
	GLint GetUniformLocation(const GLchar* name);

	// Set uniform on shader (int, texture, float, 2D, 3D and 4D vector and 4x4 matrix)
	// These functions save and restore the current active shader, therefore it is not neccessary
	// to do so.
	void SetUniform(const c8 * const name, const GLint val);
	void SetUniform(const c8 * const name, const TextureHandle tex);
	void SetUniform(const c8 * const name, const f32 val);
	void SetUniform(const c8 * const name, const float2 &val);
	void SetUniform(const c8 * const name, const float3 &val);
	void SetUniform(const c8 * const name, const float4 &val);
	void SetUniform(const c8 * const name, const Mat44 &val);
	
	// Is the shader valid
	bool Valid();

	// Returns program link log
	void PrintProgramLog(std::ostream &out);

	// Active uniforms you can get for the shader
	void PrintActiveUniforms(std::ostream &out);
};

// ShaderHandle, a nice name for a shared pointer to a Shader
typedef std::tr1::shared_ptr<Shader> ShaderHandle;