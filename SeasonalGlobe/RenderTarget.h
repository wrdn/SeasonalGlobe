#pragma once

#include <vector>
#include "ctypes.h"
#include "GameResource.h"
#include <GXBase.h>
using namespace std;

// Frame Buffer Object properties
struct FBOTexture
{
public:
	GLuint texID;
	unsigned int width, height;
	GLint internal_format;
	GLenum format;
	GLenum type;
	GLenum attachPoint;
};

// RenderTarget is an offscreen buffer you can render to
// Could be used, for example, for shadows by rendering depth to
// a render target, then comparing against this depth when rendering
// objects in the primary window render target
class RenderTarget : public Resource, public glex
{
private:
	GLuint fbo_id; // frame buffer object id
	unsigned int width, height; // width and height of textures

	// textures associated with the rendertarget (allows for multiple render targets)
	vector<FBOTexture> textures;
public:
	
	// Unloads and deletes the render target
	void Unload()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D,0);

		for(u32 i=0;i<textures.size();++i)
		{
			glDeleteTextures(1, &textures[i].texID);
		}
		textures.clear();

		if(fbo_id)
		{
			glDeleteFramebuffers(1, &fbo_id);
			fbo_id = 0;
		}
	};

	// creates the render target OpenGL ID at 800x600
	RenderTarget()
	{
		width = 800;
		height = 600;
		glGenFramebuffers(1, &fbo_id);

		glex::Load();
	};

	// creates the render target OpenGL ID at specified width and height
	RenderTarget(unsigned int pwidth, unsigned int pheight)
	{
		width = pwidth;
		height = pheight;
		glGenFramebuffers(1, &fbo_id);

		glex::Load();
	};

	// Unloads render target in destructor (for safety - but careful with objects on the stack and their scope)
	~RenderTarget()
	{
		Unload();
	};

	// returns width and height of rendertarget
	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }

	// Used to enable drawing and reading from the framebuffer
	void SetDrawReadBufferState(GLenum gl_draw_buffer, GLenum gl_read_buffer)
	{
		Bind();
		glDrawBuffer(gl_draw_buffer);
		glReadBuffer(gl_read_buffer);
		Unbind();
	}

	// Width and height mutators. Use SetWidthAndHeight() if you want to set both, as whenever called
	// they will recreate the fbo textures to the new size (expensive!)
	void SetWidth(unsigned int w) { width = w; RecreateTextures(); }
	void SetHeight(unsigned int h) { height = h; RecreateTextures(); }
	void SetWidthAndHeight(unsigned int w, unsigned int h)
	{
		width = w;
		height = h;
		RecreateTextures();
	};

	// recreate all the FBO textures at the new resolution
	void RecreateTextures()
	{
		for(unsigned int i=0;i<textures.size();++i)
		{
			FBOTexture &tex = textures[i];
			tex.width = width; tex.height = height;

			glBindTexture(GL_TEXTURE_2D, tex.texID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, tex.internal_format, tex.width, tex.height, 0, tex.format, tex.type, 0);

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, tex.attachPoint , GL_TEXTURE_2D, tex.texID, 0);
			Unbind();
		}
		Unbind();
	};

	// creates a depth texture at GL_DEPTH_ATTACHMENT
	FBOTexture CreateDepthTexture()
	{
		return CreateAndAttachTexture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, GL_DEPTH_ATTACHMENT, false, GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP);
	};

	// note: for depth textures, there is only ever 1 attachment point (GL_DEPTH_ATTACHMENT), hence why CreateDepthTexture() takes no arguments
	// in contrast, there can be many color attachments, from GL_COLOR_ATTACHMENT0+0 to GL_COLOR_ATTACHMENT0+GL_MAX_COLOR_ATTACHMENTS-1
	FBOTexture CreateColorTexture(GLenum attachmentPoint)
	{
		return CreateAndAttachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, attachmentPoint,true);
	};

	// Create a texture and attach to the rendertarget. The only required arguments are internal_format, format, type and
	// attachment point
	FBOTexture CreateAndAttachTexture(GLint internal_format, GLenum format, GLenum type, GLenum attachmentPoint,
		bool genmipmaps=true,
		f32 minFilter = GL_LINEAR_MIPMAP_LINEAR, f32 magFilter = GL_LINEAR_MIPMAP_LINEAR,
		f32 wrapS = GL_CLAMP, f32 wrapT = GL_CLAMP)
	{
		FBOTexture tex;
		glGenTextures(1, &tex.texID);
		
		tex.width = width; tex.height = height;
		tex.internal_format = internal_format;
		tex.format = format;
		tex.type = type;
		tex.attachPoint = attachmentPoint;

		glBindTexture(GL_TEXTURE_2D, tex.texID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minFilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magFilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

		glTexImage2D(GL_TEXTURE_2D, 0, tex.internal_format, tex.width, tex.height, 0, tex.format, tex.type, 0);

		if(genmipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, tex.attachPoint , GL_TEXTURE_2D, tex.texID, 0);
		Unbind();

		glBindTexture(GL_TEXTURE_2D, 0);

		textures.push_back(tex);
		return textures.back();
	};

	// Bind render target
	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D,0);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
	};

	void Bind(GLenum target)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(target, fbo_id);
	}

	// Unbind render target
	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

// RenderTargetHandle, a nice name for a shared pointer to a render target
typedef std::tr1::shared_ptr<RenderTarget> RenderTargetHandle;