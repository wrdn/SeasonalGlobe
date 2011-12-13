#pragma once

#include "LSystem.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Mat44.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "FireParticleEmitter.h"

enum TreeShadingMode
{
	NonTexturedNonLitWireframe,
	FlatNonTextured,
	SmoothNonTextured,
	SmoothTextured,
};

// BranchSegment and BranchDepth hold information about where each set of matrices (for branches)
// starts and ends. A branch segment starts at "[" and ends at the corresponding "]". Note however
// a level of the tree (BranchDepth) can contain multiple segments.
// These structures are required so we can grow the tree properly
struct BranchSegment
{
	u32 start; // first matrix in the segment
	u32 end; // last matrix in the segment

	BranchSegment() : start(0), end(0) { };
	BranchSegment(u32 _start, u32 _end) : start(_start), end(_end) { };

	~BranchSegment() { };
};
struct BranchDepth
{
	u32 depth;
	std::vector<BranchSegment> segments;

	BranchDepth() : depth(0) { };
	explicit BranchDepth(u32 _depth) : depth(_depth) { };

	~BranchDepth() { };
};

class FractalTree
{
private:
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
	enum RotationMatrixIndex
	{
		LeftX = 0, RightX = 1, UpY = 2,
		DownY = 3, LeftZ = 4, RightZ = 5,
	};
	
	static const u32 DefaultAngle = 25;

	LSystem lsysTree;

	// Rotation angles (on x y and z) will not change so only
	// calculate the rotation matrices once
	Mat44 rotationMatrices[6];

	f32 rotationAngles[3];
	f32 branchRadius;
	f32 branchRadiusReduction;
	f32 branchLength;
	
	// This holds the index of the first matrix per level of the tree
	// For 0 to levels.size()-1, the number of matrices in the level of
	// the tree is K=(levels[i+1]-levels[i]). The indices for the level
	// are contained in the area transformationMatrices[levels[i]] to
	// transformationMatrices[K]. For the last level, the indices are in
	// transformationMatrices[levels.size()-1] to transformationMatricesArraySize-1
	std::vector<u32> levels;

	std::vector<BranchDepth> treeBranchSegments;

	// Once we have built the tree, we can buildan array of matrices used
	// to transform the cylinder into place. It is much cheaper to calculate
	// these matrices once then simply load them and draw the cylinder, than
	// to perform translation/rotations/scaling each frame
	u32 transformationMatricesArraySize;
	Mat44 *transformationMatrices;

	u32 leafMatrixCount;
	Mat44 *leafMatrices;

	bool loop_growth;
	
	f32 runtime;
	f32 buildTime;

	// single cylinder to be drawn many times
	// created once only (in BuildTree())
	Cylinder gbranch;
	float3 treePos;
	Texture *tex;
	Material mat;
	Shader *treeShader;

	TreeShadingMode treeShadeMode;
	
	f32 alpha;
	u32 deathDepth; // determines the depth of the tree we will fade by alpha
	bool treeDieing;

	// Builds each of the 6 matrices required for applying rotations
	void BuildRotationMatrices();

	// Calculate the tree depth, create transformationMatrices array
	// and levels vector
	void CalculateTreeDepth();

	void DrawBranch(const Mat44 &transformationMatrix, const Mat44 &scaleMatrix);
	void DrawBranch(const Mat44 &transformationMatrix);

	void DeepCopy(const FractalTree *dstp) const; // copy data in "this" into "out"
public:

	const bool GetTreeDieing() const { return treeDieing; }
	void SetTreeDieing(const bool b) { treeDieing = b; }

	void SetTreeDeathDepth(const u32 depth) { deathDepth = depth; }
	const u32 GetTreeDeathDepth() const { return deathDepth; }

	void SetRuntime(f32 r) { runtime = r; };
	void SetBuildTime(f32 b) { buildTime = b; };
	const f32 GetRuntime() const { return runtime; };
	const f32 GetBuildTime() const { return buildTime; };

	void InitGrow()
	{
		SetRuntime(0);
		SetAlpha(1.0f);
		treeDieing = false;
		deathDepth = 0;
	};

	// Constructors / Destructors
	FractalTree();

	FractalTree(FractalTree const& t); // copy constructor (required as pointers exist)
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
	void Reset(); // resets to default values

	bool AddProductionRule(const c8 axiom, const std::string &replacement);

	// Evaluates the LSystem string and if successful, will calculate the tree
	// depth (with branches per level), and create the matrices used for drawing
	void BuildTree();

	void Draw(f32 dt);

	// Adds lines for each branch segment in depth order
	void CalculateParticleLines(std::vector<ParticleLine> &plines);

	const u32 GetDepth() const { return treeBranchSegments.size(); }

	const std::vector<BranchDepth>& GetBranchSegments() const { return treeBranchSegments; }

	#pragma region Accessors and Mutators
	const f32 GetAlpha() const { return alpha; };
	void SetAlpha(const f32 _alpha) { alpha = _alpha; };

	void SetGenerations(const u32 generations) { lsysTree.SetLSystemGenerations(generations); };
	const u32 GetGenerations() const { return lsysTree.GetLSystemGenerations(); };
	
	void SetInitialString(const std::string &initString) { lsysTree.SetStartingAxiom(initString); };
	const std::string GetInitialString() const { return lsysTree.GetStartingAxiom(); };
	
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
	
	const f32 GetBranchRadius() const { return branchRadius; };
	void SetBranchRadius(const f32 radius) { branchRadius = radius; };
	
	const f32 GetBranchRadiusReduction() const { return branchRadiusReduction; };
	void SetBranchRadiusReduction(const f32 radiusReduction) { branchRadiusReduction = radiusReduction; };
	
	const f32 GetBranchLength() const { return branchLength; };
	void SetBranchLength(const f32 length) { branchLength = length; };

	const u32 GetLeafCount() const { return leafMatrixCount; };
	const Mat44* GetLeafMatrices() const { return leafMatrices; };

	// INHERITS GRAPHICS OBJECT
	void SetPosition(const float3& f) { treePos = f; };
	const float3& GetPosition() const { return treePos; }
	void SetTexture(Texture *t) { tex = t; };
	Texture* GetTexture() const { return tex; };
	void SetDrawMode(GLenum dmode) { gbranch.SetDrawMode(dmode); };

	const Material& GetTreeMaterial() const { return mat; }
	void SetMaterial(const Material &m) { mat = m; }

	Shader* GetTreeShader() const { return treeShader; }
	void SetShader(Shader *s) { treeShader = s; }

	const TreeShadingMode GetTreeShadeMode() const
	{
		return treeShadeMode;
	};
	void SetTreeShadeMode(Shader *s, Texture *t, TreeShadingMode m)
	{
		treeShader = s;
		tex = t;
		treeShadeMode = m;
	};

	// Constants (default radius, etc)
	static const f32 GetDefaultBranchRadius() { return 1.0f; };
	static const f32 GetDefaultBranchRadiusReduction() { return 0.0f; };
	static const f32 GetDefaultBranchLength() { return 0.8f; };
	#pragma endregion
};