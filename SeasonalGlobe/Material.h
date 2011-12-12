#pragma once

#include "ctypes.h"
#include "Color.h"
#include <GXBase.h>

struct Material
{
public:
	float4 ambient, diffuse, specular;
	f32 shininess; // used in Phong shading

	Material() : ambient(float4(0.5f,0.5f,0.5f,1)), diffuse(float4(1,1,1,1)), specular(float4(1,1,1,1)), shininess(30)
	{};
	Material(const float4 &amb, const float4 &diff, const float4 &spec, const f32 _shininess)
		: ambient(amb), diffuse(diff), specular(spec), shininess(_shininess) { };
	~Material() { };

	// Set public variables to only change the object (and nothing else). Use the Set functions below to change
	// the object and the OpenGL state
	void SetAmbient(const float4 &v) { ambient = v; glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.GetVec()); }
	void SetDiffuse(const float4 &v) { diffuse = v; glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.GetVec()); }
	void SetSpecular(const float4 &v) { specular = v; glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.GetVec()); }
	void SetShininess(const f32 v) { shininess = v; glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); }

	void Activate() const
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.GetVec());
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.GetVec());
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.GetVec());
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	};
};

/*
class Material
{
private:
	u32 ka, kd, ks, ke;
	
public:
	Material() : ka(0), kd(0), ks(0), ke(0) { };
	~Material() { };

	const u32 GetAmbientInteger() const;
	const u32 GetDiffuseInteger() const;
	const u32 GetSpecularInteger() const;
	const u32 GetEmissiveInteger() const;

	const Color4f GetAmbientColor() const;
	const Color4f GetDiffuseColor() const;
	const Color4f GetSpecularColor() const;
	const Color4f GetEmissiveColor() const;

	void SetAmbient(const u32 ka);
	void SetAmbient(const Color4f &c);

	void SetDiffuse(const u32 kd);
	void SetDiffuse(const Color4f &c);

	void SetSpecular(const u32 ks);
	void SetSpecular(const Color4f &c);

	void SetEmissive(const u32 ke);
	void SetEmissive(const Color4f &c);
};*/