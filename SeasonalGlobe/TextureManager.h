#pragma once

#include "Texture.h"
#include <map>
#include <GXBase.h>

class TextureManager : public glex
{
private:
	std::map<u32, Texture> textures;
public:
	TextureManager(void);
	~TextureManager(void);

	// Return NULL on error. These functions use GXBase to load the image
	// and will automatically build mipmaps. The image from GXBase will then
	// be destroyed as it is no longer required.
	const Texture* LoadTextureFromFile(const c8* const _filename);

	// Expects data in RGBA GL_UNSIGNED_BYTE format
	const Texture* LoadTextureFromMemory(const u32* const _mem, const u32 _width, const u32 _height, const u32 format);

	const Texture* GetTexture(const u32 _id) const;
	bool DeleteTexture(const u32 _id); // returns false if texture doesn't exist
};