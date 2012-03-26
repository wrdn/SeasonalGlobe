#include "ResourceManager.h"
ResourceManager* ResourceManager::resMan = 0;
i32 ResourceManager::idProvider = 0;

// returns resource manager singleton (new'ing it if neccessary)
ResourceManager& ResourceManager::get() // returns reference to help avoid mucking with pointers
{
	if(!ResourceManager::resMan)
	{
		ResourceManager::resMan = new ResourceManager();
	}
	return *ResourceManager::resMan;
};

// Adds resource using id, name and resource pointer. By using a pointer,
// we can add anything that inherits resource safely. This is a private function used internally
// by the resource manager
void ResourceManager::AddResource(i32 id, const char *name, Resource *r)
{
	r->SetResourceID(id);
	r->SetName(name);
	resourceMap[id] = std::tr1::shared_ptr<Resource>(r);
};

// Create texture handle then load the texture. If texture already loaded, returns current texture
TextureHandle LoadTexture(const char *filename, const char *textureResourceName) // if not provided, the default resource name used is the filename
{
	TextureHandle hnd = textureResourceName ? ResourceManager::get().CreateAndGetResource<Texture>(textureResourceName) :
		ResourceManager::get().CreateAndGetResource<Texture>(filename);

	if(hnd->GetGLTextureID())
	{
		return hnd;
	}; // return resource if already loaded
	if(!hnd->Load(filename))
	{
		ResourceManager::get().RemoveResource(hnd->GetResourceID());
		hnd = TextureHandle(0);
	}
	return hnd;
};

SoundHandle LoadSound(const char *filename, const char *soundResourceName)
{
	SoundHandle snd = soundResourceName ? ResourceManager::get().CreateAndGetResource<Sound>(soundResourceName) : ResourceManager::get().CreateAndGetResource<Sound>(filename);
	if(snd->Valid())
	{
		return snd;
	}
	if(!snd->Load(filename))
	{
		ResourceManager::get().RemoveResource(snd->GetResourceID());
		snd = SoundHandle(0);
	}
	return snd;
};

// Creates shader object (compiled and linked) from vertex and fragment shader. Unless already compiled, the vertex and fragment
// shaders will be compiled. If compilation/linking fails, any logs will be printed to console
ShaderHandle LoadShader(const char *vertexShaderFilename, const char *fragmentShaderFilename, const char *shaderResourceName) // if not provided, there is no default shader resource name
{
	if(!vertexShaderFilename || !fragmentShaderFilename) { return ShaderHandle((Shader*)0); };

	// Compile and check vertex shader
	VertexShaderHandle vsh = ResourceManager::get().CreateAndGetResource<VertexShaderObject>(vertexShaderFilename); // ASSUME WE WANT TO RELOAD SHADER (COULD CHECK IF ID ALREADY VALID)
	if(!vsh->Valid())
	{
		if(!vsh->CompileFromFile(vertexShaderFilename))
		{
			vsh->PrintShaderLog(std::cout);
			ResourceManager::get().RemoveResource(vsh->GetResourceID());
			return ShaderHandle((Shader*)0);
		}
	}
	
	// Compile and check fragment shader
	FragmentShaderHandle fsh = ResourceManager::get().CreateAndGetResource<FragmentShaderObject>(fragmentShaderFilename); // ASSUME WE WANT TO RELOAD SHADER (COULD CHECK IF ID ALREADY VALID)
	if(!fsh->Valid())
	{
		if(!fsh->CompileFromFile(fragmentShaderFilename))
		{
			fsh->PrintShaderLog(std::cout);
			ResourceManager::get().RemoveResource(vsh->GetResourceID());
			ResourceManager::get().RemoveResource(fsh->GetResourceID());
			return ShaderHandle((Shader*)0);
		}
	}

	// Compile and check program object
	ShaderHandle sh = shaderResourceName ? ResourceManager::get().CreateAndGetResource<Shader>(shaderResourceName) : ResourceManager::get().CreateAndGetResource<Shader>();

	if(sh->GetVS() && sh->GetFS()) // SAME SHADER
	{
		if(sh->GetVS()->GetResourceID() == vsh->GetResourceID() && sh->GetFS()->GetResourceID() == fsh->GetResourceID())
		{
			return sh;
		}
	};

	// create and link shader program
	if(!sh->CreateProgram(vsh, fsh))
	{
		sh->PrintProgramLog(std::cout);
		ResourceManager::get().RemoveResource(vsh->GetResourceID());
		ResourceManager::get().RemoveResource(fsh->GetResourceID());
		ResourceManager::get().RemoveResource(sh->GetResourceID());
		return ShaderHandle((Shader*)0);
	}
	
	return sh;
};

// Creates render target to specified width and height
RenderTargetHandle CreateRenderTarget(int width, int height, const char *renderTargetResourceName)
{
	RenderTargetHandle rth = renderTargetResourceName ? ResourceManager::get().CreateAndGetResource<RenderTarget>(renderTargetResourceName)
		: ResourceManager::get().CreateAndGetResource<RenderTarget>();
	rth->SetWidthAndHeight(width, height);
	return rth;
};

// Create mesh, with unique ID and specified name
MeshHandle CreateMesh(char *name)
{
	MeshHandle mh = ResourceManager::get().CreateAndGetResource<Mesh>();
	mh->SetName(name);
	return mh;
};