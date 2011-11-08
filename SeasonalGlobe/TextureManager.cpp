#include "TextureManager.h"
#include <GXBase.h>
using namespace gxbase;

TextureManager::TextureManager(void)
{
}

TextureManager::~TextureManager(void)
{
	// Generally the texture manager will be a singleton, and will exist throughout the entire application
	// The texture manager will ensure that any textures are cleaned up (if not done elsewhere)
	for (std::map<u32,Texture>::iterator it=textures.begin() ; it != textures.end(); it++ )
	{
		glDeleteTextures(1, &it->first);
	}
	textures.clear();
}

const Texture* TextureManager::LoadTextureFromFile(const c8* const _filename)
{
	Image tex;
	bool loaded = tex.Load(_filename);
	if(loaded)
	{
		u32 id;
		glGenTextures(1, &id);
		if(id > 0)
		{
			// Load texture, build mipmaps and unload
			Texture textureObject(id);
			
			textureObject.SetWidth(tex.Width());
			textureObject.SetHeight(tex.Height());

			textureObject.Activate();
			tex.gluBuild2DMipmaps();
			textureObject.Deactivate();
			tex.Free();

			textures[id] = textureObject;
			return &textures[id];
		}
	}
	return NULL;
};

const Texture* TextureManager::LoadTextureFromMemory(const u32* const _mem, const u32 _width,
	const u32 _height, const u32 format)
{
	if(_mem)
	{
		u32 id;
		glGenTextures(1, &id);
		if(id > 0)
		{
			Texture textureObject(id);
			textureObject.SetWidth(_width);
			textureObject.SetHeight(_height);

			textureObject.Activate();
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _width, _height, format, GL_UNSIGNED_BYTE, _mem);
			textureObject.Deactivate();

			textures[id] = textureObject;
			return &textures[id];
		}
	}
	return NULL;
};

const Texture* TextureManager::GetTexture(const u32 _id) const
{
	if(textures.count(_id) > 0)
	{
		return &textures.at(_id);
	}
	return NULL;
};

bool TextureManager::DeleteTexture(const u32 _id)
{
	if(textures.count(_id) > 0)
	{
		glDeleteTextures(1, &_id);
		textures.erase(_id);
		return true;
	}
	return false;
};