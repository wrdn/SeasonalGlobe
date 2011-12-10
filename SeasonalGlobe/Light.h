#pragma once

#include "float4.h"
#include "float3.h"
#include <GXBase.h>

// To make life simpler, Light contains all possible light properties.
// The required ones should then be read in a shader
class Light
{
private:
	u32 lightID; // GL_LIGHT0 to GL_LIGHT7

public:
	float4 position, // light position (or direction if a directional light)
		spotlightDirection; // spot light direction
	f32 cutoff; // spotlight cutoff angle
	float4 ambient, diffuse, specular; // colour properties

	// If you just want to set variables without affecting OpenGL state, use the public
	// variables above. If the light has been Activate()'d and you wish to change its variable
	// in OpenGL, call the Set functions below
	void SetPosition(float4 &pos)
	{
		position = pos;
		glLightfv(lightID, GL_POSITION, position.GetVec());
	};
	void SetSpotLightDirection(float4 &dir)
	{
		spotlightDirection = dir;
		glLightfv(lightID, GL_SPOT_DIRECTION, spotlightDirection.GetVec());
	};
	void SetSpotLightCutoffAngle(f32 angle)
	{
		cutoff = angle;
		glLightf(lightID, GL_SPOT_CUTOFF, cutoff);
	};
	void SetAmbient(float4 &v)
	{
		ambient = v;
		glLightfv(lightID, GL_AMBIENT, ambient.GetVec());
	};
	void SetDiffuse(float4 &v)
	{
		diffuse = v;
		glLightfv(lightID, GL_DIFFUSE, diffuse.GetVec());
	};
	void SetSpecular(float4 &v)
	{
		specular = v;
		glLightfv(lightID, GL_SPECULAR, specular.GetVec());
	};

	// Default constructor sets lightID to GL_LIGHT0
	Light() : lightID(GL_LIGHT0) { };

	// Constructor for directional or point light
	Light(float4 &pos, float4 &amb, float4 &diff, float4 &spec)
		: lightID(GL_LIGHT0), position(pos), ambient(amb), diffuse(diff), specular(spec)
	{ };

	// Constructor for spotlight
	Light(float4 &pos, float4 &spotDir, f32 cutoffAngle,
		float4 &amb, float4 &diff, float4 &spec)
		: lightID(GL_LIGHT0), position(pos), spotlightDirection(spotDir), cutoff(cutoffAngle),
		ambient(amb), diffuse(diff), specular(spec)
	{ };

	// Destructor
	~Light() { };

	void Activate()
	{
		//glEnable(GL_LIGHTING);
		//glEnable(lightID);

		glLightfv(lightID, GL_POSITION, position.GetVec());
		glLightfv(lightID, GL_SPOT_DIRECTION, spotlightDirection.GetVec());
		glLightf(lightID, GL_SPOT_CUTOFF, cutoff);
		glLightfv(lightID, GL_AMBIENT, ambient.GetVec());
		glLightfv(lightID, GL_DIFFUSE, diffuse.GetVec());
		glLightfv(lightID, GL_SPECULAR, specular.GetVec());
	};

	// glDisable()s the light. If using shaders, this has no effect
	void Deactivate()
	{
		glDisable(lightID);
	};

	// should be GL_LIGHT0 to GL_LIGHT7
	// returns false if not
	bool SetLightID(GLenum id)
	{
		if(id >= GL_LIGHT0 && id <= GL_LIGHT7)
		{
			lightID = id;
			return true;
		}
		return false;
	};
};