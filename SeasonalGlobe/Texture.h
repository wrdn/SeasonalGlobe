#pragma once

#include <GXBase.h>
#include "ctypes.h"
#include "GameResource.h"

// Used instead of direct OpenGL texture IDs, for better type safety
enum OpenGLTextureSlot
{
	SLOT_GL_TEXTURE_0 = GL_TEXTURE0,
	SLOT_GL_TEXTURE_1 = GL_TEXTURE1,
	SLOT_GL_TEXTURE_2 = GL_TEXTURE2,
	SLOT_GL_TEXTURE_3 = GL_TEXTURE3,
	SLOT_GL_TEXTURE_4 = GL_TEXTURE4,
	SLOT_GL_TEXTURE_5 = GL_TEXTURE5,
	SLOT_GL_TEXTURE_6 = GL_TEXTURE6,
	SLOT_GL_TEXTURE_7 = GL_TEXTURE7,
};

// Texture object, containing the OpenGL texture ID, its texture slot (allowing multitexturing) and width and height
// It inherits Resource so may be managed in the ResourceManager
class Texture : public Resource
{
private:
	u32 textureID; // OpenGL texture ID
	u32 minFilter, magFilter, wrapS, wrapT; // common texture properties
	u32 width, height; // texture width and height
	OpenGLTextureSlot glTexSlot; // texture slot SLOT_GL_TEXTURE_0 to SLOT_GL_TEXTURE_7

public:
	Texture() : textureID(0), minFilter(GL_LINEAR_MIPMAP_LINEAR), magFilter(GL_LINEAR_MIPMAP_LINEAR), wrapS(GL_REPEAT), wrapT(GL_REPEAT),
		width(0), height(0), glTexSlot(SLOT_GL_TEXTURE_0)
	{
	};

	~Texture()
	{
		Unload();
	}; // REMEMBER: CAREFUL WITH TEXTURE ON STACK, TEXTURE IS DELETED IN DESTRUCTOR

	// Load texture from file
	bool Load(const char *filename);

	// Deletes texture object
	void Unload();

	// Activates texture
	void Activate();
	
	// Deactivate (unbinds) texture
	void Deactivate() const;

	// Sets paramter on texture
	void SetParameteri(GLenum param, u32 v);
	void SetParameterf(GLenum param, f32 v);

	// Easily set common parameters
	void SetWrapS(u32 wrap_s) { SetParameteri(GL_TEXTURE_WRAP_S, wrap_s); }
	void SetWrapT(u32 wrap_t) { SetParameteri(GL_TEXTURE_WRAP_T, wrap_t); }
	void SetMinFilter(u32 minFilter) { SetParameteri(GL_TEXTURE_MIN_FILTER, minFilter); }
	void SetMagFilter(u32 magFilter) { SetParameteri(GL_TEXTURE_MAG_FILTER, magFilter); }

	u32 GetGLTextureID() const { return textureID; };
	void SetTextureSlot(OpenGLTextureSlot s) { glTexSlot = s; };
	OpenGLTextureSlot GetTextureSlot() const { return glTexSlot; };
	u32 GetTextureSlotIndex() const { return glTexSlot - SLOT_GL_TEXTURE_0; };
};

// shared pointer to texture
typedef std::tr1::shared_ptr<Texture> TextureHandle;