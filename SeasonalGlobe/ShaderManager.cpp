#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
};

ShaderManager::~ShaderManager()
{
	try
	{
		Clean();
	}
	catch(...) { }
};

u32 ShaderManager::AddShader()
{
	shaders.push_back(new Shader());
	handles.push_back(ShaderHandle(shaders.back()));
	return handles.size()-1;
};

bool ShaderManager::RemoveShader(u32 index)
{
	if(index < handles.size())
	{
		delete handles[index].shader;
		handles[index].shader = 0;
		return true;
	}
	return false;
};

Shader * const ShaderManager::GetShader(u32 index) const
{
	if(index < handles.size())
	{
		return handles[index].shader;
	}
	return 0;
};

void ShaderManager::Clean()
{
	for(u32 i=0;i<handles.size();++i)
	{
		delete handles[i].shader;
	}
	handles.clear();
	shaders.clear();
};

bool ShaderManager::LoadShader(u32 &out_id, const c8* vertexShaderFilename, const c8* fragmentShaderFilename)
{
	u32 id = AddShader();
	Shader *sh = GetShader(id);

	if(!sh->LoadShader(fragmentShaderFilename, vertexShaderFilename))
	{
		sh->PrintShaderLog(GL_VERTEX_SHADER, std::cout);
		sh->PrintShaderLog(GL_FRAGMENT_SHADER, std::cout);
		sh->PrintProgramLog(std::cout);

		RemoveShader(id);

		return false;
	}

	out_id = id;
	return true;
};