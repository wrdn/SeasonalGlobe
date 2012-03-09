#include <vector>
#include <map>
#include <fstream>
#include <string>
using namespace std;
#include "GraphicsObject.h"

struct OBJMaterial
{
public:
	u32 hashedName; // hash the name of the material
	float3 ka, kd, ks;
	string map_Kd_filename, // diffuse map
		map_Ks_filename, // specular map
		map_Bump_filename, // bump/normal map
		map_d_filename; // mask (used for plants etc)

	OBJMaterial() : hashedName(0) {};
};

//! notes: uses the streaming operators in float2 and float3 to read in data
//! The fail bit is then read (stream.fail()) to ensure the data was read correctly
//! before it can be used
class OBJLoader
{
private:
	enum OBJLineType
	{
		IgnoredLine=0, // Used if line not in ValidLineInitiators

		VertexLine, // "v" line
		TextureCoordinateLine, // "vt" line
		NormalLine, // "vn" line
		FaceLine, // "f" line
		ObjectLine, // "o" line
		MTLLIBLine, // "mtllib" line (specifies material file to parse)
		USEMTLLine, // "usemtl" line (specifies material from material library to use)
	};
	enum OBJMaterialNodeType
	{
		IgnoredNode=0,
		Mat_NewMaterial, // newmtl
		Mat_MapKd, // map_Kd
		Mat_MapKs, // map_Ks
		Mat_MapBump, // map_Bump
		Mat_MapD, // map_d
		Mat_Ka, // Ka
		Mat_Kd, // Kd
		Mat_Ks, // Ks
	};

	//! Returns string representation of values in OBJLineType
	static const char* LineTypeAsString(OBJLineType t);

	//! Returns the line type, or IgnoredLine 0. if(ValidLineStart(s)) is true if result !=IgnoredLine
	static OBJLineType ValidateLineStart(const string &lineStart);

	//! Used when parsing obj material file (mtl), to find the type of node the line represents (e.g. diffuse texture)
	static OBJMaterialNodeType ValidateOBJMaterialLineStart(const string &str);

	//! Any line that does not start with a string found here ignored
	static const c8* ValidLineInitiators[];
	static const OBJLineType AssociatedLineTypes[];

	static const c8* OBJMaterialValidLineInitiators[];
	static const OBJMaterialNodeType OBJMaterialAssociatedNodeTypes[];

	OBJLoader();
	~OBJLoader();

	//! Parses obj material (mtl) file, setting materials in outputMap
	static bool ParseMaterialFile(const c8* mtl_filename, std::map<u32, OBJMaterial> &outputMap);

public:

	//! Parses obj file
	static bool LoadOBJFile(const string& filename, vector<GraphicsObject> &output_vec);
};