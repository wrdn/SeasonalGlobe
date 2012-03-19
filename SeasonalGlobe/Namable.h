#pragma once

// Makes a class namable (especially useful when debugging so you can see what the class/instance refers to)
class Namable
{
private:
	char *name;
public:
	Namable() : name(0) {};
	Namable(const char *c) : name((char*)c) {};
	~Namable() {};

	inline void SetName(const char *m) { name = (char*)m; };
	inline const char* GetName() { return name; };
};