#pragma once

#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "float3.h"
#include "Texture.h"

class GraphicsObject
{
private:
	Model gmodel; // the model associated with the graphics object

	float3 position, scale;
	Material mat;
	f32 xrotation, yrotation, zrotation;

	// these should be pointers to data managed by the shader/texture manager
	Texture *tex;
	Shader *objectShader;

public:
	GraphicsObject(void);
	virtual ~GraphicsObject(void);

	float3& GetPosition() { return position; };

	f32 GetXRotation() { return xrotation; };
	f32 GetYRotation() { return yrotation; };
	f32 GetZRotation() { return zrotation; };
	
	float3& GetScale() { return scale; }
	Material& GetMaterial() { return mat; }
	Texture* GetTexture() { return tex; }
	Shader* GetShader() { return objectShader; }

	Model& GetModel() { return gmodel; };

	void SetModel(Model &m);
	void SetShader(Shader *s) { objectShader = s; };
	void SetTexture(Texture *t) { tex = t; };
	void SetPosition(float3 pos) { position = pos; };

	void SetXRotation(f32 rot) { xrotation = rot; };
	void SetYRotation(f32 rot) { yrotation = rot; };
	void SetZRotation(f32 rot) { zrotation = rot; };

	void SetScale(float3 sc) { scale = sc; };
	void SetMaterial(Material &mat);

	void SetDrawMode(GLenum dmode) { gmodel.SetDrawMode(dmode); };

	void Draw(); // activates textures, shaders, performs transformations and draws the model
	void DrawSimple(); // no activations, just draws the model (useful if transformations done outside GraphicsObject, e.g. tree)
};