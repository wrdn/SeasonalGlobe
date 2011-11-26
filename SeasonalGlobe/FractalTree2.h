#pragma once

#include "LSystem.h"
#include "Cylinder.h"
#include "Mat44.h"

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
	// recreated every time the tree is built (the tree should only
	// be built once, with the matrices reused, so this is not a problem)
	Cylinder gbranch;

	// This holds the index of the first matrix per level of the tree
	// For 0 to levels.size()-1, the number of matrices in the level of
	// the tree is K=(levels[i+1]-levels[i]). The indices for the level
	// are contained in the area transformationMatrices[levels[i]] to
	// transformationMatrices[K]. For the last level, the indices are in
	// transformationMatrices[levels.size()-1] to transformationMatricesArraySize-1
	std::vector<u32> levels;

	// Once we have built the tree, we can buildan array of matrices used
	// to transform the cylinder into place. It is much cheaper to calculate
	// these matrices once then simply load them and draw the cylinder, than
	// to perform translation/rotations/scaling each frame
	u32 transformationMatricesArraySize;
	Mat44 *transformationMatrices;
	#pragma endregion


	// Builds each of the 6 matrices required for applying rotations
	void BuildRotationMatrices();

public:
	// Calculate the tree depth, create transformationMatrices array
	// and levels vector
	void CalculateTreeDepth();

	// Constructors / Destructors
	FractalTree2();
	~FractalTree2();
	void Reset(); // resets to default values

	bool AddProductionRule(const c8 axiom, const std::string &replacement);

	// Evaluates the LSystem string and if successful, will calculate the tree
	// depth (with branches per level), and create the matrices that can then be
	// used for drawing
	void BuildTree();

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