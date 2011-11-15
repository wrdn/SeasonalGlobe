#pragma once

#include "GXBase.h"
using namespace gxbase;

#include <vector>

struct Vec3f
{
public:
	float x,y,z;
};

struct Vec2f
{
public:
	float x,y;

	Vec2f() : x(0), y(0) { };
	Vec2f(float _x, float _y) : x(_x), y(_y) { };

	float dot(Vec2f &v) { return x*v.x + y*v.y; };

	Vec2f subtract(Vec2f &v) { return Vec2f(x-v.x, y-v.y); };
	Vec2f add(Vec2f &v) { return Vec2f(x+v.x, y+v.y); };
	Vec2f mul(Vec2f &v) { return Vec2f(x*v.x, y*v.y); };
	Vec2f mul(float v) { return Vec2f(x*v, y*v); };

	Vec2f operator+(Vec2f &v) { return add(v); };
	Vec2f operator-(Vec2f &v) { return subtract(v); };
	Vec2f operator*(Vec2f &v) { return mul(v); };
	Vec2f operator*(float v) { return mul(v); };
};
struct Vec2i
{
public:
	int x,y;

	Vec2i() : x(0), y(0) { };
	Vec2i(int _x, int _y) : x(_x), y(_y) { };
};

struct Line
{
public:
	Vec2f start, end;
};

struct Circle
{
public:
	Vec2f pos;
	float radius;
};

struct CollisionPoint
{
	// If only 1 solution exists, t2 will be set to -1

	Vec2f p1; float t1; // will always be the shortest distance
	Vec2f p2; float t2; // will always be the largest distance
};

struct Face
{
public:
	Vec3f pos1;
	Vec3f pos2;
	Vec3f pos3;
};

class terrain {
private:
	Image _heightfield;
	float (*_vertexarray)[3];
	float (*_normalarray)[3];
	GLuint *_indexarray;
	float (*_texarray)[2];
	int _width, _height;

	std::vector<Vec3f> _vertices;
	std::vector<Vec2f> _tex;
	std::vector<unsigned int> _indices;

	std::vector<Face> faces;

public:
	terrain();

	~terrain();

	void OnCreate();

	void draw(bool texon) const;

	void normal(bool normon) const;

private:
	void _gen_normals();

	void _normalize();

};
