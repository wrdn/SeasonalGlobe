#pragma once

#include "ctypes.h"
#include "Color.h"

class Material
{
private:
	u32 ka, kd, ks, ke;
	
public:
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
};