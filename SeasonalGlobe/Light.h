#pragma once

#include "float4.h"
#include "float3.h"
#include <GXBase.h>

// To simplify coding, Light contains all possible light properties.
// The required ones should then be read in a shader using the shader inbuilt light
// variables: gl_LightSource[n].variable
class Light
{
private:
	u32 lightID; // GL_LIGHT0 to GL_LIGHT7

	float4 position, // light position (or direction if a directional light)
		spotlightDirection; // spot light direction
	f32 cutoff; // spotlight cutoff angle
	float4 ambient, diffuse, specular; // colour properties

public:

	// Accessors
	const u32 GetLightID() const { return lightID; }
	const float4& GetPosition() const { return position; }
	const float4& GetSpotLightDirection() const { return spotlightDirection; }
	const float4& GetAmbient() const { return ambient; }
	const float4& GetDiffuse() const { return diffuse; }
	const float4& GetSpecular() const { return specular; }
	const f32 GetCutoffAngle() const { return cutoff; };


	// Updates the variable AND updates the OpenGL state
	void SetPositionAndUpdateOpenGL(const float4 &pos)
	{
		position = pos;
		glLightfv(lightID, GL_POSITION, position.GetVec());
	};
	void SetSpotLightDirectionAndUpdateOpenGL(const float4 &dir)
	{
		spotlightDirection = dir;
		glLightfv(lightID, GL_SPOT_DIRECTION, spotlightDirection.GetVec());
	};
	void SetSpotLightCutoffAngleAndUpdateOpenGL(const f32 angle)
	{
		cutoff = angle;
		glLightf(lightID, GL_SPOT_CUTOFF, cutoff);
	};
	void SetAmbientAndUpdateOpenGL(const float4 &v)
	{
		ambient = v;
		glLightfv(lightID, GL_AMBIENT, ambient.GetVec());
	};
	void SetDiffuseAndUpdateOpenGL(const float4 &v)
	{
		diffuse = v;
		glLightfv(lightID, GL_DIFFUSE, diffuse.GetVec());
	};
	void SetSpecularAndUpdateOpenGL(const float4 &v)
	{
		specular = v;
		glLightfv(lightID, GL_SPECULAR, specular.GetVec());
	};

	// Default constructor sets lightID to GL_LIGHT0
	Light() : lightID(GL_LIGHT0), cutoff(20) { };

	// Constructor for directional or point light
	Light(const float4 &pos, const float4 &amb, const float4 &diff, const float4 &spec)
		: lightID(GL_LIGHT0), position(pos), cutoff(20), ambient(amb), diffuse(diff), specular(spec)
	{ };

	// Constructor for spotlight
	Light(const float4 &pos, const float4 &spotDir, const f32 cutoffAngle,
		const float4 &amb, const float4 &diff, const float4 &spec)
		: lightID(GL_LIGHT0), position(pos), spotlightDirection(spotDir), cutoff(cutoffAngle),
		ambient(amb), diffuse(diff), specular(spec)
	{ };

	// Destructor
	~Light() { };

	// Activates the light by setting all the OpenGL properties
	void Activate() const
	{
		glLightfv(lightID, GL_POSITION, position.GetVec());
		glLightfv(lightID, GL_SPOT_DIRECTION, spotlightDirection.GetVec());
		glLightf(lightID, GL_SPOT_CUTOFF, cutoff);
		glLightfv(lightID, GL_AMBIENT, ambient.GetVec());
		glLightfv(lightID, GL_DIFFUSE, diffuse.GetVec());
		glLightfv(lightID, GL_SPECULAR, specular.GetVec());
	};

	// glDisable()s the light. If using shaders, this has no effect
	void Deactivate() const
	{
		glDisable(lightID);
	};

	// should be GL_LIGHT0 to GL_LIGHT7
	// returns false if not
	const bool SetLightID(GLenum id)
	{
		if(id >= GL_LIGHT0 && id <= GL_LIGHT7)
		{
			lightID = id;
			return true;
		}
		return false;
	};
};