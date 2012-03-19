#pragma once

#include "color.h"
#include <vector>
#include "Texture.h"
#include "Shader.h"

// Class representing common material properties an object may have
// including ambient, diffuse, specular and emissive colours (ka, kd, ks and ke respectively),
// textures (as many as required) and the shader handle
class Material
{
private:
	color ka, kd, ks, ke; // ambient, diffuse, specular and emissive colours
	f32 shininess; // object shininess (used in specular lighting)

	std::vector<TextureHandle> textures; // list of textures to activate for the object
	ShaderHandle activeShader; // shader applied to object

public:
	Material() : ka(0.1f), kd(1.0f), ks(0.15f), ke(0.0f), shininess(10) {};
	Material(const color &_ka, const color &_kd, const color &_ks, const f32 _shininess)
		: ka(_ka), kd(_kd), ks(_ks), shininess(_shininess) {};

	// Accessors
	color GetAmbient() const { return ka; };
	color GetDiffuse() const { return kd; };
	color GetSpecular() const { return ks; };
	color GetEmissive() const { return ke; };
	f32 GetShininess() const { return shininess; };

	// Mutators
	void SetAmbient(const color &col) { ka = col; };
	void SetDiffuse(const color &col) { kd = col; };
	void SetSpecular(const color &col) { ks = col; };
	void SetEmissive(const color &col) { ke = col; };
	void SetShininess(const f32 shine) { shininess = shine; };
	
	void SetAmbientDiffuseSpecularEmissive(const color &ambient, const color &diffuse, const color &specular, const color &emissive=color(0.0f))
	{
		SetAmbient(ambient);
		SetDiffuse(diffuse);
		SetSpecular(specular);
		SetEmissive(emissive);
	}

	std::vector<TextureHandle> &GetTextures() { return textures; };

	void ClearTextures() { textures.clear(); };

	int AddTexture(TextureHandle t)
	{
		textures.push_back(t);
		return textures.size()-1;
	};

	void SetShader(ShaderHandle active_shader)
	{
		activeShader = active_shader;
	};

	ShaderHandle GetShader() { return activeShader; };

	// Activate material colour and shininess properties ONLY
	// DOES NOT enable the textures or shader
	void Activate()
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka.GetVec());
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd.GetVec());
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks.GetVec());
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ke.GetVec());
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	};
};