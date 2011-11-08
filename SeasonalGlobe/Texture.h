#pragma once

#include "ctypes.h"

// Used to manage textures from OpenGL.
class Texture
{
private:
	u32 id;
	u32 minFilter, magFilter, wrapS, wrapT; // automatically set on activation
	u32 width, height;

public:
	Texture(void);
	Texture(const u32 _id);
	~Texture(void);

	const u32 GetID() const;
	void SetID(const u32 _id);

	void Activate() const;
	void Deactivate() const;

	// http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml
	void SetMinFilter(const u32 _minFilter);
	void SetMagFilter(const u32 _magFilter);
	void SetWrapS(const u32 wrapMode);
	void SetWrapT(const u32 wrapMode);

	const u32 GetMinFilter() const;
	const u32 GetMagFilter() const;
	const u32 GetWrapS() const;
	const u32 GetWrapT() const;

	void SetParameteri(u32 param, u32 v) const;

	void SetWidth(const u32 _width);
	void SetHeight(const u32 _height);
	const u32 GetWidth() const;
	const u32 GetHeight() const;
};

/*
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
*/