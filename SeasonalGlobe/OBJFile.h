#ifndef __OBJFILE_H__
#define __OBJFILE_H__

#include "GraphicsObject.h"
#include <vector>

// OBJFile loader (creates and fills Model objects). The loader loads only what is neccessary and nothing else. It does not recalculate normals, or build the
// VBOs, as this generic functionality is contained in the composite Model objects. The loader will load vertices, normals, UV coordinates, materials and faces
// Before loading, the model needs to be __triangulated__
// The format of faces within an OBJ file (that contains vertices, normals and uvs) is: v/vt/vn (Vertex/UV/Normal)
class OBJFile
{
private:
	// No construction (static class)
	OBJFile(OBJFile const& other);
	OBJFile& operator= (OBJFile const& other);
	OBJFile();
	~OBJFile();

	static std::vector<GraphicsObject*> ParseFile(const std::vector<c8*> &objFile);
public:
	static std::vector<GraphicsObject*> ParseFile(const c8* filename);
};

#endif