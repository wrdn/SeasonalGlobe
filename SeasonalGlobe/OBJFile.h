#ifndef __OBJFILE_H__
#define __OBJFILE_H__

#include "util.h"
#include "Model.h"

// OBJFile loader (creates and fills Model objects). The loader loads only what is neccessary and nothing else. It does not recalculate normals, or build the
// VBOs, as this generic functionality is contained in the composite Model objects. The loader will load vertices, normals, UV coordinates, materials and faces
// Before loading, the model needs to be __triangulated__
class OBJFile
{
public:
	OBJFile();
	~OBJFile();

	std::vector<Model*> models;

	bool ParseOBJFile(const c8* filename);
	bool ParseOBJFile(std::vector<c8*> objFile);
};

#endif