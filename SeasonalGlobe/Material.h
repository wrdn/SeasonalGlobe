#pragma once

#include "ctypes.h"
#include "Color.h"

struct Material
{
public:
	Color4f ambient, diffuse, specular;
	f32 shininess; // used in Phong shading

	Material() : ambient(Color4f(0.3f,0.3f,0.3f,1)), diffuse(Color4f(0.75f,0.75f,0.75f,1)), specular(Color4f(0.8f,0.8f,0.8f,1)), shininess(30)
	{};
	Material(Color4f &amb, Color4f &diff, Color4f &spec, f32 _shininess)
		: ambient(amb), diffuse(diff), specular(spec), shininess(_shininess) { };
	~Material() { };
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