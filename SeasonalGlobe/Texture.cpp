#include "Texture.h"
using namespace gxbase;

// set integer parameter on texture
void Texture::SetParameteri(GLenum param, u32 v)
{
	Activate();
	glTexParameteri(GL_TEXTURE_2D, param, v);
	Deactivate();
};

// set float parameter on texture
void Texture::SetParameterf(GLenum param, f32 v)
{
	Activate();
	glTexParameterf(GL_TEXTURE_2D, param, v);
	Deactivate();
};

// load texture from file
bool Texture::Load(const char *filename)
{
	Unload(); // unload texture first

	bool returnVal = false;

	// load texture
	Image tex;
	bool loaded = tex.Load(filename);
	if(loaded)
	{
		// generate ID and bind
		glGenTextures(1, &textureID);
		if(textureID)
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minFilter); // set texture parameters based on current settings in object
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

			width = tex.Width();
			height = tex.Height();

			// build texture mipmaps
			Activate();
			tex.gluBuild2DMipmaps();
			Deactivate();

			returnVal = true;
		}

		tex.Free(); // free texture in memory (now in opengl)
	}
	return returnVal;
};

// deletes and invalidates texture
void Texture::Unload()
{
	if(textureID)
	{
		glDeleteTextures(1, &textureID);
	}
	textureID = 0;
};

// activates texture
void Texture::Activate()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	glBindTexture(GL_TEXTURE_2D, GetGLTextureID());
};

// deactivates texture
void Texture::Deactivate() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
};