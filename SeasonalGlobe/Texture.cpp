#include "Texture.h"
#include <GXBase.h>

Texture::Texture(void)
	: id(0), minFilter(GL_LINEAR_MIPMAP_LINEAR), magFilter(GL_LINEAR_MIPMAP_LINEAR),
	wrapS(GL_REPEAT), wrapT(GL_REPEAT)
{
}

Texture::~Texture(void)
{
}

const u32 Texture::GetID() const { return id; };

void Texture::SetID(const u32 _id) { id=_id; };

void Texture::Activate() const
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magFilter);

	glBindTexture(GL_TEXTURE_2D, id);
};

void Texture::Deactivate() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
};

void Texture::SetMinFilter(const u32 _minFilter)
{
	minFilter = _minFilter;
};

void Texture::SetMagFilter(const u32 _magFilter)
{
	magFilter = _magFilter;
};

void Texture::SetWrapS(const u32 wrapMode)
{
	wrapS = wrapMode;
};

void Texture::SetWrapT(const u32 wrapMode)
{
	wrapT = wrapMode;
};

const u32 Texture::GetMinFilter() const
{
	return minFilter;
};

const u32 Texture::GetMagFilter() const
{
	return magFilter;
};

const u32 Texture::GetWrapS() const
{
	return wrapS;
};

const u32 Texture::GetWrapT() const
{
	return wrapT;
};

void Texture::SetParameteri(u32 param, u32 v) const
{
	glTexParameteri(GL_TEXTURE_2D, param, v);
};