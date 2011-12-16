#pragma once

#include <GXBase.h>

class Vector4f {
private:
	GLfloat _xyzw[4];

public:
	void Assign(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		_xyzw[0] = x;
		_xyzw[1] = y;
		_xyzw[2] = z;
		_xyzw[3] = w;
	}

	Vector4f() { 
		Assign(0.0, 0.0, 0.0, 1.0);
	}

	Vector4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		Assign(x, y, z, w);
	}

	explicit Vector4f(GLfloat xyzw[]) {
		Assign(xyzw[0], xyzw[1], xyzw[2], xyzw[3]);
	}

	const GLfloat* GetVec() const { return _xyzw; };

	const float x() const { return _xyzw[0]; }
	const float y() const { return _xyzw[1]; }
	const float z() const { return _xyzw[2]; }
	const float w() const { return _xyzw[3]; }

	void x(float _x) { _xyzw[0] = _x; }
	void y(float _y) { _xyzw[1] = _y; }
	void z(float _z) { _xyzw[2] = _z; }
	void w(float _w) { _xyzw[3] = _w; }
};

class ColorT {
private:
	GLfloat _rgba[4];

public:
	void Assign(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
		_rgba[0] = r;
		_rgba[1] = g;
		_rgba[2] = b;
		_rgba[3] = a;
	}

	ColorT() { 
		Assign(0.0, 0.0, 0.0, 0.0);
	}

	ColorT(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
		Assign(r, g, b, a);
	}

	const GLfloat* GetVec() const { return _rgba; }

	const float r() const { return _rgba[0]; }
	const float g() const { return _rgba[1]; }
	const float b() const { return _rgba[2]; }
	const float a() const { return _rgba[3]; }

	void r(float _r) { _rgba[0] = _r; }
	void g(float _g) { _rgba[1] = _g; }
	void b(float _b) { _rgba[2] = _b; }
	void a(float _a) { _rgba[3] = _a; }

	explicit ColorT(GLfloat rgba[]) {
		Assign(rgba[0], rgba[1], rgba[2], rgba[3]);
	}

	static ColorT black() {
		return ColorT(0.0, 0.0, 0.0, 1.0);
	}

	static ColorT white() {
		return ColorT(1.0, 1.0, 1.0, 1.0);
	}

	static ColorT red() {
		return ColorT(1.0, 0.0, 0.0, 1.0);
	}

	static ColorT green() {
		return ColorT(0.0, 1.0, 0.0, 1.0);
	}

	static ColorT blue() {
		return ColorT(0.0, 0.0, 1.0, 1.0);
	}

	static ColorT yellow() {
		return ColorT(1.0, 1.0, 0.0, 1.0);
	}

	static ColorT null() {
		return ColorT(0.0, 0.0, 0.0, 0.0);
	}
};

class Materials {
private:
	ColorT _ambient;
	ColorT _diffuse;
	ColorT _emission;

public:
	Materials() {
		_ambient = ColorT::black();
		_diffuse = ColorT::white();
	}

	void create(const ColorT &ambient=ColorT::black(), const ColorT &diffuse=ColorT::ColorT(), const ColorT &emission=ColorT::null()) {
		setAmbient(ambient);
		setDiffuse(diffuse);
		setEmission(emission);
	}

	void setAmbient(const ColorT &ambient) {
		_ambient = ambient;
	}		

	void setDiffuse(const ColorT &diffuse) {
		_diffuse = diffuse;
	}		

	void setEmission(const ColorT &emission) {
		_emission = emission;
	}		

	void alphaDiffuse(const float inc) {
		_diffuse.a( _diffuse.a() + inc);
		if (_diffuse.a() < 0.0f) _diffuse.a(0.0f);
		if (_diffuse.a() > 1.0f) _diffuse.a(1.0f);
	}


	void apply() const {
		glMaterialfv(GL_FRONT, GL_AMBIENT, _ambient.GetVec());
		glMaterialfv(GL_FRONT, GL_DIFFUSE, _diffuse.GetVec());
		glMaterialfv(GL_FRONT, GL_EMISSION, _emission.GetVec());
	}
};


class Lights {
private:
	// Light number
	GLint _lightNumber;				

	// Ambient, diffuse and position
	ColorT _ambient;
	ColorT _diffuse;
	Vector4f _position;

public:
	Lights() {
		_lightNumber = -1;
	}

	void create(GLuint lightNum, const ColorT &ambient=ColorT::black(), const ColorT &diffuse=ColorT::white(), const Vector4f &position=Vector4f(0.0f,0.0f,0.0f,1.0f)) {
		// the light being defined
		_lightNumber = lightNum;	

		setAmbient(ambient);
		setDiffuse(diffuse);
		setPosition(position);
	}

	void setAmbient(const ColorT &ambient) {
		_ambient = ambient;
		glLightfv(GL_LIGHT0+_lightNumber, GL_AMBIENT, _ambient.GetVec());
	}		

	void setDiffuse(const ColorT &diffuse) {
		_diffuse = diffuse;
		glLightfv(GL_LIGHT0+_lightNumber, GL_DIFFUSE, _diffuse.GetVec());
	}		

	void setPosition(const Vector4f &position) {
		_position = position;
		glLightfv(GL_LIGHT0+_lightNumber, GL_POSITION, _position.GetVec());
	}

	void apply() const {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0 + _lightNumber);
	}
};