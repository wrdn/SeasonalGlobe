#pragma once

#include "Shader.h"
#include <vector>

class ShaderManager
{
private:
	struct ShaderHandle
	{
	public:
		Shader *shader;

		ShaderHandle() : shader(0) { };
		explicit ShaderHandle(Shader *sh) : shader(sh) { };
	};

	std::vector<Shader*> shaders;
	std::vector<ShaderHandle> handles;

public:
	ShaderManager();
	~ShaderManager();

	u32 AddShader();
	bool RemoveShader(u32 index);
	Shader * const GetShader(u32 index) const;

	void Clean();
};