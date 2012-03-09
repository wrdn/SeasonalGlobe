#pragma once

#include "float3.h"
#include "Quaternion.h"
#include "Material.h"
#include "Mesh.h"

//! The base object for renderable objects, with properties including the Mesh (geometry), textures, shader and
//! material properties (ka, kd, ks, ke, shininess) and position, scale and orientation of the object
class GraphicsObject
{
private:
	//! Globally disable use of shaders and textures (allows easy overriding without modifying objects)
	//! these do not effect local state when changed, so if GLOBAL_USE_TEXTURES=true, but local_use_textures=false,
	//! the object will not use textures. Both initially true
	static bool GLOBAL_USE_SHADERS;
	static bool GLOBAL_USE_TEXTURES;

	//! scale is non-uniform, orientation is amount of rotation (in degrees) around cardinal XYZ axis
	float3 position, scale, orientation;
	MeshHandle mesh; // shared_ptr to mesh (which should be managed in ResourceManager)

	Material objectMaterial;

	GLenum polygonFillMode; // GL_POINT, GL_LINE, GL_FILL

	//! Using local_use_textures and local_use_shaders, even if GLOBAL_USE_SHADERS and GLOBAL_USE_TEXTURES are true,
	//! texturing/shading can be disabled for the object (without affecting the texture list/shader handle)
	bool local_use_textures;
	bool local_use_shaders;

public:
	GraphicsObject() : scale(1.0f), polygonFillMode(GL_FILL), local_use_textures(true), local_use_shaders(true) {};

	//! MATERIAL
	Material &GetMaterial() { return objectMaterial; };

	//! POLYGON FILL MODE
	GLenum GetPolygonFillMode() const { return polygonFillMode; }
	void SetPolygonFillMode(const GLenum fillMode) { polygonFillMode = fillMode; };

	//! USE TEXTURES AND USE SHADERS
	void SetUsingTextures(const bool useTextures) { local_use_textures = useTextures; };
	void SetUsingShaders(const bool useShaders) { local_use_shaders = useShaders; };
	bool UsingTextures() const { return local_use_textures; };
	bool UsingShaders() const { return local_use_shaders; };

	//! MESH
	const MeshHandle GetMesh() const { return mesh; };
	void SetMesh(const MeshHandle m) { mesh = m; };

	//! POSITION
	const float3& GetPosition()const { return position; };
	void SetPosition(const float3 &pos) { position = pos; };

	//! SCALE
	const float3& GetScale() const { return scale; }
	void SetScale(const float3 &sc) { scale = sc; };

	//! ORIENTATION
	const float3 &GetOrientation() const { return orientation; };
	void SetOrientation(const float3 &_orientation) { orientation = _orientation; };
	f32 GetXRotation() const { return orientation.x(); };
	f32 GetYRotation() const { return orientation.y(); };
	f32 GetZRotation() const { return orientation.z(); };
	void SetXRotation(f32 rot) { orientation.x(rot); };
	void SetYRotation(f32 rot) { orientation.y(rot); };
	void SetZRotation(f32 rot) { orientation.z(rot); };

	//! COMMON MATERIAL OPERATIONS (REPEATED FOR EASY ACCESS)
	void ClearTextures() { objectMaterial.ClearTextures(); };
	int AddTexture(TextureHandle t) { return objectMaterial.AddTexture(t); };
	void SetShader(ShaderHandle active_shader) { objectMaterial.SetShader(active_shader); };

	ShaderHandle GetShader() { return objectMaterial.GetShader(); };

	//! DRAWING FUNCTIONS
	void Draw();
	void DrawSimple()
	{
		if(!mesh) return;
		mesh->Draw();
	};
};