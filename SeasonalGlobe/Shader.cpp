#include "Shader.h"
#include "strutils.h"
#include <iostream>

// used to save and restore the currently active shader
#define PUSH_ACTIVE_SHADER(t) i32 t=0; glGetIntegerv(GL_CURRENT_PROGRAM, &t);
#define POP_ACTIVE_SHADER(t) glUseProgram(t);

// calls reset on shared pointers (to invalidate the references, and decrement the reference count)
// then deletes the shader program
void Shader::Unload()
{
	vs.reset();
	fs.reset();
	if(shaderProgramID)
	{
		glDeleteProgram(shaderProgramID);
		shaderProgramID = 0;
	}
};

// returns true if shader program linked correctly
bool Shader::HasLinked()
{
	i32 link_status = 0;
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &link_status);
	return link_status == GL_TRUE;
};

// create program from vertex and fragment shader handle
bool Shader::CreateProgram(VertexShaderHandle vs_handle, FragmentShaderHandle fs_handle)
{
	// ensure the vertex and fragment shaders are valid
	if(!vs_handle || !fs_handle || !vs_handle->Valid() || !fs_handle->Valid()) { return false; }

	vs = vs_handle;
	fs = fs_handle;

	// delete program if currently exists
	if(shaderProgramID) { glDeleteProgram(shaderProgramID); };
	shaderProgramID = glCreateProgram();

	// attach shaders and link
	glAttachShader(shaderProgramID, vs->GetShaderID());
	glAttachShader(shaderProgramID, fs->GetShaderID());
	glLinkProgram(shaderProgramID);

	return HasLinked();
};

// activate shader
void Shader::Activate() { glUseProgram(shaderProgramID); };

// deactivate shader
void Shader::Deactivate() { glUseProgram(0); };

// get uniform location given uniform name
GLint Shader::GetUniformLocation(const GLchar* name) // 0 on fail or -1
{
	return glGetUniformLocation(shaderProgramID, name);
};

// set uniform to int
void Shader::SetUniform(const c8 * const name, const GLint val)
{
	PUSH_ACTIVE_SHADER(t);
	Activate();
	glUniform1i(GetUniformLocation(name), val);
	POP_ACTIVE_SHADER(t);
};

// set uniform to texture slot index (0 to 7) - same as using SetUniform(name, int) but less error prone
void Shader::SetUniform(const c8 * const name, const TextureHandle tex)
{
	SetUniform(name, (GLint)tex->GetTextureSlotIndex());
};

// set uniform to float
void Shader::SetUniform(const c8 * const name, const f32 val)
{
	PUSH_ACTIVE_SHADER(t);
	Activate();
	glUniform1f(GetUniformLocation(name), val);
	POP_ACTIVE_SHADER(t);
};

// set uniform to 2D vector
void Shader::SetUniform(const c8 * const name, const float2 &val)
{
	PUSH_ACTIVE_SHADER(t);
	Activate();
	glUniform2fv(GetUniformLocation(name),1, val.GetVec());
	POP_ACTIVE_SHADER(t);
};

// set uniform to 3D vector
void Shader::SetUniform(const c8 * const name, const float3 &val)
{
	PUSH_ACTIVE_SHADER(t);
	Activate();
	glUniform3fv(GetUniformLocation(name),1, val.GetVec());
	POP_ACTIVE_SHADER(t);
};

// set uniform to 4D vector
void Shader::SetUniform(const c8 * const name, const float4 &val)
{
	PUSH_ACTIVE_SHADER(t);
	Activate();
	glUniform4fv(GetUniformLocation(name),1, val.GetVec());
	POP_ACTIVE_SHADER(t);
};

// set uniform to 4x4 matrix
void Shader::SetUniform(const c8 * const name, const Mat44 &val)
{
	PUSH_ACTIVE_SHADER(t);
	Activate();
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, val.GetMatrix());
	POP_ACTIVE_SHADER(t);
};

// returns true if shader valid
bool Shader::Valid()
{
	if(!fs || !vs) return false;

	return fs->Valid() && vs->Valid();
};

// prints program log
void Shader::PrintProgramLog(std::ostream &out)
{
	const int LOG_SZ = 4096;
	char log[LOG_SZ];
	glGetProgramInfoLog(shaderProgramID, LOG_SZ, NULL, log);
	out << log << std::endl;
};

// prints active uniforms
void Shader::PrintActiveUniforms(std::ostream &out)
{
	i32 total = -1;
	glGetProgramiv(shaderProgramID, GL_ACTIVE_UNIFORMS, &total);
	if(total == 0) return;

	for(i32 i=0; i<total; ++i)  {
		i32 name_len=-1, num=-1;
		GLenum type = GL_ZERO;
		c8 name[100]; // NOTE: max of 100 character names supported in this function. Increase array size if required.
		glGetActiveUniform( shaderProgramID, GLuint(i), sizeof(name)-1,
			&name_len, &num, &type, name );
		name[name_len] = 0;
		out << name << ", ";
	};
	out << std::endl;
};