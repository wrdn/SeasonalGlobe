#pragma once

#include "LSystem.h"
#include "Cylinder.h"
#include "Mat44.h"

// Usage:
// * 
class FractalTree2
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
		ROTATE_RIGHT_Z = '>'
	};
	enum RotationMatrixIndex
	{
		LeftX = 0, RightX = 1, UpY = 2,
		DownY = 3, LeftZ = 4, RightZ = 5,
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
		BranchDepth(u32 _depth) : depth(_depth) { };

		~BranchDepth() { };
	};

	static const u32 DefaultAngle = 25;

	#pragma region Class Members
	LSystem lsysTree;

	// Rotation angles (on x y and z) will not change so only
	// calculate the rotation matrices once
	Mat44 rotationMatrices[6];

	f32 rotationAngles[3];
	f32 branchRadius;
	f32 branchRadiusReduction;
	f32 branchLength;

	// single cylinder to be drawn many times
	// created once only (in BuildTree())
	Cylinder gbranch;

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
	#pragma endregion

	// Builds each of the 6 matrices required for applying rotations
	void BuildRotationMatrices();

	// Calculate the tree depth, create transformationMatrices array
	// and levels vector
	void CalculateTreeDepth();

	f32 currentScale; // used to make branches grow, update by dt, range 0<=currentScale<=1
	f32 lastTime;

	i32 AnimationLevel; // which level of the tree are we drawing?

	// used to update currentScale (0...1) and update the animation level 
	// of the tree if neccessary
	void CalculateAnimationLevel(const float dt)
	{
		/*if( (currentScale+dt) > 1.0f )
		{
			++AnimationLevel;
			currentScale = 0;
		}
		currentScale += dt;

		if((u32)AnimationLevel >= levels.size())
			AnimationLevel = 0;*/
		
		lastTime += dt;
		currentScale = fract(lastTime);
		AnimationLevel = min(levels.size()-1, (u32)lastTime);
	};

	u32 drawLevel;

	void DrawBranch(const Mat44 &transformationMatrix, const Mat44 &scaleMatrix);
	void DrawBranch(const Mat44 &transformationMatrix);

	bool loop_growth;

public:

	void SetAnimationLevel(i32 v)
	{
		AnimationLevel = v;
	};

	const u32 GetDrawLevel() const { return drawLevel; };
	void SetDrawLevel(const i32 dl)
	{
		if(dl < 0)
			drawLevel = 0;
		else
			drawLevel = min(treeBranchSegments.size()-1, (u32)dl);
	};

	// Constructors / Destructors
	FractalTree2();
	~FractalTree2();
	void Reset(); // resets to default values

	bool AddProductionRule(const c8 axiom, const std::string &replacement);

	// Evaluates the LSystem string and if successful, will calculate the tree
	// depth (with branches per level), and create the matrices used for drawing
	void BuildTree();

	void Draw(const float dt);

	#pragma region Accessors and Mutators
	Cylinder& GetBranchModel() { return gbranch; };
	
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

	// Constants (default radius, etc)
	static const f32 _GetDefaultBranchRadius() { return 1.0f; };
	static const f32 _GetDefaultBranchRadiusReduction() { return 0.0f; };
	static const f32 _GetDefaultBranchLength() { return 0.8f; };
	#pragma endregion
};