#pragma once

#include "ctypes.h"
#include <GXBase.h>
#include <iosfwd>

#include "Mat44.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "Texture.h"

enum ShaderError
{
	ERROR_FAIL=false, // generic failure
	ERROR_OK=true, // success

	// failed to create vertex shader,
	// fragment shader, or both
	ERROR_INVALID_VS_ID,
	ERROR_INVALID_FS_ID,
	ERROR_INVALID_VS_FS_ID,

	ERROR_VS_COMPILE_FAILED,
	ERROR_FS_COMPILE_FAILED,

	ERROR_CREATE_PROGRAM_FAILED,
	ERROR_LINK_FAILED,
};

// Usage process:
// * Initialise with Init()
// * CompileVertexShader(...) and CompileFragmentShader(...)
// * CreateProgram()
// * SetUniform(...) if neccessary
// * Activate() to use shader
// * Deactivate() after use
class Shader : public glex
{
private:
	bool isActive; // deactivates entire shader
	GLuint vertexShaderID, fragmentShaderID, shaderProgramID;
	u32 error; // error flag (used for more details if function returns ERROR_FAIL)
public:
	Shader();
	~Shader();

	const u32 GetLastError() const;

	void SetActive(bool active);
	const bool IsActive() const;

	const bool Init();

	bool CompilerVertexShader(const c8 * const src);
	bool CompileFragmentShader(const c8 * const src);

	bool CreateProgram(); // creates and links shader program

	const GLint GetUnformLocation(const GLchar* name); // 0 on fail or -1

	void SetUniform(const GLint _id, const GLint val);
	void SetUniform(const c8 * const name, const Texture &tex);
	void SetUniform(const c8 * const name, const f32 val);
	void SetUniform(const c8 * const name, const GLint val);
	void SetUniform(const c8 * const name, const float2 &val);
	void SetUniform(const c8 * const name, const float3 &val);
	void SetUniform(const c8 * const name, const float4 &val);
	void SetUniform(const c8 * const name, const Mat44 &val);

	void Activate();
	void Deactivate();

	void PrintActiveUniforms()
	{
		int total = -1;
		glGetProgramiv( shaderProgramID, GL_ACTIVE_UNIFORMS, &total );
		if(total == 0) return;

		for(int i=0; i<total; ++i)  {
			int name_len=-1, num=-1;
			GLenum type = GL_ZERO;
			char name[100];
			glGetActiveUniform( shaderProgramID, GLuint(i), sizeof(name)-1,
				&name_len, &num, &type, name );
			name[name_len] = 0;
			std::cout << name << ", ";
			//GLuint location = glGetUniformLocation( shaderProgramID, name );
		};
		std::cout << std::endl;
	};

	void PrintShaderLog(GLenum shaderType, std::ostream &out);
};