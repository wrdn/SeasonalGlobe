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
	Texture *textureA, *textureB;
	Shader *objectShader;
	glex ogl;

public:
	GraphicsObject(void);
	virtual ~GraphicsObject(void);

	const float3& GetPosition()const { return position; };

	const f32 GetXRotation() const { return xrotation; };
	const f32 GetYRotation() const { return yrotation; };
	const f32 GetZRotation() const { return zrotation; };
	
	const float3& GetScale() const { return scale; }
	const Material& GetMaterial() const { return mat; }
	const Texture* GetTextureA() const { return textureA; }
	const Texture* GetTextureB() const { return textureB; }
	Shader* GetShader() const { return objectShader; }

	 Model& GetModel() { return gmodel; };

	void SetModel(Model &m);
	void SetShader(Shader *s) { objectShader = s; };
	void SetTextureA(Texture *t) { textureA = t; };
	void SetTextureB(Texture *t) { textureB = t; };
	void SetPosition(const float3 &pos) { position = pos; };
	
	void SetXRotation(f32 rot) { xrotation = rot; };
	void SetYRotation(f32 rot) { yrotation = rot; };
	void SetZRotation(f32 rot) { zrotation = rot; };

	void SetScale(const float3 &sc) { scale = sc; };
	void SetMaterial(const Material &_mat) { mat = _mat; }

	void SetDrawMode(GLenum dmode) { gmodel.SetDrawMode(dmode); };

	void Draw(); // activates textures, shaders, performs transformations and draws the model
	void DrawSimple(); // no activations, just draws the model (useful if transformations done outside GraphicsObject, e.g. tree)
};