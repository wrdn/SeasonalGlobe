#pragma once

#include "LSystem.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Mat44.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "FireParticleEmitter.h"

//! Collects all the shaders for the tree (directional and spot lights)
struct TreeShaders
{
public:
	ShaderHandle FlatNonTextured_Directional_Shader, // flat non textured directional shader
		FlatNonTextured_Spot_Shader, // flat non textured spotlight shader
		Tree_Ambient_Shader, // ambient shader
		SmoothShaded_Directional_Shader, // smooth textured directional shader
		SmoothShaded_Spot_Shader; // smooth textured spotlight shader

	TreeShaders()
		: FlatNonTextured_Directional_Shader(0), FlatNonTextured_Spot_Shader(0),
		Tree_Ambient_Shader(0), SmoothShaded_Directional_Shader(0), SmoothShaded_Spot_Shader(0)
	{};
	~TreeShaders() { };
};

//! Tree shading mode - used so we can select the appropriate shader when drawing
enum TreeShadingMode
{
	NonTexturedNonLitWireframe,
	FlatNonTextured,
	SmoothNonTextured,
	SmoothTextured,
	NormalMappedTextured,
};

// BranchSegment and BranchDepth hold information about where each set of matrices (for branches)
// starts and ends. A branch segment starts at "[" and ends at the corresponding "]". Note however
// a level of the tree (BranchDepth) can contain multiple segments.
// These structures are required so we can grow the tree properly
// The matrices for the branches are used to translate and rotate the branch
struct BranchSegment
{
	u32 start; // first matrix in the segment
	u32 end; // last matrix in the segment

	BranchSegment() : start(0), end(0) { };
	BranchSegment(u32 _start, u32 _end) : start(_start), end(_end) { };

	~BranchSegment() { };
};

// At each depth of the tree (e.g. root of tree is depth 0, first branches are depth 1), there are
// 1 or more branch segments. The branch segment holds the first and last index of the matrices used
// to draw that branch. We require this information to be able to draw the branches properly
struct BranchDepth
{
	u32 depth;
	std::vector<BranchSegment> segments;

	BranchDepth() : depth(0) { };
	explicit BranchDepth(u32 _depth) : depth(_depth) { };

	~BranchDepth() { };
};

//! The Fractal Tree classes uses an L-System grammar (plus the LSystem class) to create the tree
//! It contains support to build a tree (as a cached set of matrices), animate its growth and death
//! Note the tree is NOT classed as a graphics object, as it has a very specialised draw function
//! As a result, it contains its own position, textures etc
class FractalTree
{
private:

	//! Symbols supported by the trees LSystem
	enum SYMBOL_SET
	{
		MOVE_FORWARD = 'F',
		PUSH_MATRIX_STACK = '[',
		POP_MATRIX_STACK = ']',
		ROTATE_LEFT_X = '-',
		ROTATE_RIGHT_X = '+',
		ROTATE_UP_Y = '^',
		ROTATE_DOWN_Y = 'V',
		ROTATE_LEFT_Z = '<',
		ROTATE_RIGHT_Z = '>',
		DRAW_LEAF = 'L'
	};

	//! Rotation matrices are cached so we can easily apply them when building the tree matrices
	enum RotationMatrixIndex
	{
		LeftX = 0, RightX = 1, UpY = 2,
		DownY = 3, LeftZ = 4, RightZ = 5,
	};
	
	//! Angle by which branch is rotated when ROTATE_* symbol is found in the lsystem
	static const u32 DefaultAngle = 25;

	//! LSystem object evaluates the strings provided based on rules that can be passed
	//! With the returned evaluated string, we then build the tree
	LSystem lsysTree;

	// Rotation angles (on x y and z) will not change so only
	// calculate the rotation matrices once
	Mat44 rotationMatrices[6];

	f32 rotationAngles[3];
	f32 branchLength;
	
	// This holds the index of the first matrix per level of the tree
	// For 0 to levels.size()-1, the number of matrices in the level of
	// the tree is K=(levels[i+1]-levels[i]). The indices for the level
	// are contained in the area transformationMatrices[levels[i]] to
	// transformationMatrices[K]. For the last level, the indices are in
	// transformationMatrices[levels.size()-1] to transformationMatricesArraySize-1
	std::vector<u32> levels;

	//! Contains a list of branch segments at each depth
	std::vector<BranchDepth> treeBranchSegments;

	// Once we have built the tree, we can build an array of matrices used
	// to transform the cylinder into place. It is much cheaper to calculate
	// these matrices once then simply load them and draw the cylinder, than
	// to perform translation/rotations/scaling each frame
	// Originally, the entire L-System was evaluated each frame, however this was
	// very slow, hence we use cached matrices
	u32 transformationMatricesArraySize;
	Mat44 *transformationMatrices;

	// Used to transform to the position of leaves (used by leaf particle system)
	u32 leafMatrixCount;
	Mat44 *leafMatrices;

	bool loop_growth; // for debug, can loop growing the tree

	// runtime and build time (growth time). runtime is the time into the build time we are currently
	// at, and is used to calculate which branches should be drawn/hidden, and which branches should
	// be animated (we animate the growth of a cylinder by animating its scale from 0 to the default scale,
	// along the Y axis - i.e. animate its length)
	f32 runtime;
	f32 buildTime;

	// single cylinder to be drawn many times
	// created once only (in BuildTree())
	Cylinder gbranch;

	// Standard properties including position, textures, shaders and material
	float3 treePos;
	TextureHandle diffuseTexture, normalMap;
	glex ogl;
	Material mat;
	ShaderHandle treeShader;

	// Shading mode (smooth, flat, wireframe etc)
	TreeShadingMode treeShadeMode;
	
	// Alpha value used when tree dying (branches fade out)
	f32 alpha;
	u32 deathDepth; // determines the depth of the tree we will fade by alpha
	bool treeDieing;

	// Activate or deactivate the update and display of the tree
	bool isActive;

	// Builds each of the 6 matrices required for applying rotations
	void BuildRotationMatrices();

	// Calculate the tree depth, create transformationMatrices array
	// and levels vector
	void CalculateTreeDepth();

	// Draw branch cylinder given a 4x4 transformation matrix (and optional scale matrix)
	void DrawBranch(const Mat44 &transformationMatrix, const Mat44 &scaleMatrix);
	void DrawBranch(const Mat44 &transformationMatrix);

	void DeepCopy(const FractalTree *dstp) const; // copy data in "this" into "out"
public:

	// Is the tree currently dieing?
	const bool GetTreeDieing() const { return treeDieing; }
	void SetTreeDieing(const bool b) { treeDieing = b; }

	// Set depth of the tree currently dieing (e.g. we can make it immediately die)
	void SetTreeDeathDepth(const u32 depth) { deathDepth = depth; }
	const u32 GetTreeDeathDepth() const { return deathDepth; }

	// Runtime and Build time - build time is total time for the tree to grow
	// Runtime is the time it has currently been growing for
	void SetRuntime(f32 r) { runtime = r; };
	void SetBuildTime(f32 b) { buildTime = b; };
	const f32 GetRuntime() const { return runtime; };
	const f32 GetBuildTime() const { return buildTime; };

	// Initialise growth of the tree by reseting runtime and other variables
	void InitGrow()
	{
		SetRuntime(0);
		SetAlpha(1.0f);
		treeDieing = false;
		deathDepth = 0;
	};

	// Constructors / Destructors
	FractalTree();

	FractalTree(FractalTree const& t); // copy constructor (required as pointers exist) and assignment operator
	FractalTree& operator= (FractalTree const& other)
	{
		if(this != &other)
		{
			other.DeepCopy(this);

			if(transformationMatrices)
				_aligned_free(transformationMatrices);

			if(transformationMatricesArraySize)
			{
				transformationMatrices = (Mat44*)_aligned_malloc(sizeof(Mat44) * transformationMatricesArraySize, 16);
				memcpy(transformationMatrices, &other.transformationMatrices, sizeof(Mat44) * transformationMatricesArraySize);
			}
			else
			{
				transformationMatrices = 0;
			}
		}
		return *this;
	};

	~FractalTree();

	void Reset(); // resets tree to default values

	// Add a replacement rule used when building the tree, e.g. axiom 'A', replacement 'FFA'
	bool AddProductionRule(const c8 axiom, const std::string &replacement);

	// Evaluates the LSystem string and if successful, will calculate the tree
	// depth (with branches per level), and create the matrices used for drawing
	void BuildTree();

	// Draws the tree, updating runtime by dt if growing/dieing
	void Draw(f32 dt);

	// Adds lines for each branch segment in depth order
	// These lines are used by the fire particle system, with fire particles generated in
	// random directions along the tree. There is a line per segment (i.e. per branch matrix)
	// in the tree. Thus, if particles are displayed, but the tree itself is hidden, the fire will
	// be drawn in the shape of the tree, a very nice effect :)
	void CalculateParticleLines(std::vector<ParticleLine> &plines);

	// Returns the full depth of the tree (i.e. how many lists of branch segments there are)
	const u32 GetDepth() const { return treeBranchSegments.size(); }

	const std::vector<BranchDepth>& GetBranchSegments() const { return treeBranchSegments; }

	
	// ACCESSORS AND MUTATORS

	// Alpha value used when tree dying (branches fade out)
	const f32 GetAlpha() const { return alpha; };
	void SetAlpha(const f32 _alpha) { alpha = _alpha; };

	// Number of generations the LSystem contains (i.e. how many times it reevaluates the LSystem string)
	void SetGenerations(const u32 generations) { lsysTree.SetLSystemGenerations(generations); };
	const u32 GetGenerations() const { return lsysTree.GetLSystemGenerations(); };
	
	// Initial string to be evaluated by the LSystem
	void SetInitialString(const std::string &initString) { lsysTree.SetStartingAxiom(initString); };
	const std::string GetInitialString() const { return lsysTree.GetStartingAxiom(); };
	
	// Can set a different angle for X, Y and Z rotation of the branch
	const f32* GetBranchRotationAngles() const { return rotationAngles; };
	void SetBranchRotationAngles(const f32 *rotationAnglesXYZ)
	{
		rotationAngles[0] = rotationAnglesXYZ[0];
		rotationAngles[1] = rotationAnglesXYZ[1];
		rotationAngles[2] = rotationAnglesXYZ[2];
		BuildRotationMatrices();
	};
	void SetBranchRotationAngles(const f32 angle)
	{
		rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = angle;
		BuildRotationMatrices();
	};
	
	// Length of the branch - MUST be set before creating the branch cylinder
	const f32 GetBranchLength() const { return branchLength; };
	void SetBranchLength(const f32 length) { branchLength = length; };

	// Gets the leaf matrices (useful for the leaf particle system)
	const u32 GetLeafCount() const { return leafMatrixCount; };
	const Mat44* GetLeafMatrices() const { return leafMatrices; };

	// Position
	void SetPosition(const float3& f) { treePos = f; };
	const float3& GetPosition() const { return treePos; }

	// Diffuse Texture
	void SetTexture(TextureHandle t) { diffuseTexture = t; };
	TextureHandle GetTexture() const { return diffuseTexture; };

	// Fill mode
	void SetPolygonFillMode(GLenum dmode) { gbranch.SetPolygonFillMode(dmode); };

	// Normal map
	TextureHandle GetNormalMap() const { return normalMap; };
	void SetNormalMap(TextureHandle t) { normalMap = t; };

	// Tree material (ka, kd, ks etc)
	const Material& GetTreeMaterial() const { return mat; }
	void SetMaterial(const Material &m) { mat = m; }

	// Tree shader
	ShaderHandle GetTreeShader() const { return treeShader; };
	void SetShader(ShaderHandle s) { treeShader = s; }

	// Tree shade mode (smooth, flat, wireframe etc)
	const TreeShadingMode GetTreeShadeMode() const
	{
		return treeShadeMode;
	};
	void SetTreeShadeMode(ShaderHandle s, TextureHandle _diffuseTexture, TreeShadingMode m)
	{
		treeShader = s;
		diffuseTexture = _diffuseTexture;
		treeShadeMode = m;
	};

	// Activate or deactivate the update and display of the tree
	void SetActive(bool active) { isActive = active; }
	const bool GetActive() const { return isActive; }

	static const f32 GetDefaultBranchLength() { return 0.8f; };
};