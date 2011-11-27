#pragma once

#include "LSystem.h"
#include "float3.h"
#include "Cylinder.h"
#include "Mat44.h"

#include <stack> // for our matrix stack
using namespace std;

// Usage:
// Add a number of production rules: AddProductionRule(...)
// Set the initial string for the LSystem: SetInitialString(...)
// And the number of generations it will run for: SetGenerations(...)
// Evaluate the LSystem, building the final string for interpretation: EvaluateTreeLSystem()
// Set the branch radius, radius reduction, length and rotation angles as neccessary (see defaults)
// Draw() the tree
class FractalTree
{
private:
	// dont want full control over the LSystem outside FractalTree
	// (hence composition rather than inheritance)
	LSystem _lsystree;

	stack<Mat44> matrixStack;
	stack<f32> radiusStack;

	enum RotationMatrixIndex
	{
		LeftX = 0,
		RightX = 1,
		UpY = 2,
		DownY = 3,
		LeftZ = 4,
		RightZ = 5,
	};
	Mat44 rotationMatrices[6];

	// All symbols are upper case, during parsing everything
	// is converted to upper case before checks
	enum SYMBOL_SET
	{
		MOVE_FORWARD = 'F',
		PUSH_MATRIX_STACK = '[',
		POP_MATRIX_STACK = ']',
		ROTATE_LEFT_X = '-',
		ROTATE_RIGHT_X = '+',
		ROTATE_UP_Y = '^',
		ROTATE_DOWN_Y = 'V',
		ROTATE_LEFT_Z = '<', // twist left
		ROTATE_RIGHT_Z = '>',
	};

	static const f32 DEFAULT_BRANCH_RADIUS;
	static const f32 DEFAULT_BRANCH_RADIUS_REDUCTION;
	static const f32 DEFAULT_BRANCH_LENGTH;
	static const float3 DEFAULT_ROTATION_ANGLES;

	float3 branchRotationAngles; // amount to rotate in x, y and z
	f32 branchRadius, branchRadiusReduction;
	f32 branchLength; // uniform scale factor

	// drawn multiple times (with different matrices)
	// to avoid scaling issues etc, this should be a unit cylinder
	Cylinder *_branchModel;

	void BuildXRotationMatrices();
	void BuildYRotationMatrices();
	void BuildZRotationMatrices();
	void BuildAllRotationMatrices();

	void InitMatrixStack();
	void PopStack();
	void PushStack();
	void Rotate(float angle, float x, float y, float z);
	void Translate(float tx, float ty, float tz);

public:
	FractalTree(void);
	~FractalTree(void);

	void Init(); // initialise with defaults
	void Init(const float3 _branchRotationAngles, const f32 _branchRadius, 
		const f32 _branchRadiusReduction, const f32 _branchLength);

	// returns false if the rule already exists
	bool AddProductionRule(const c8 symbol, const std::string &replacement);

	void EvaluateTreeLSystem(); // evaluates the tree, producing the final string

	// Builds a list of matrices from the evaluated LSystem (requires EvaluateTreeLSystem call), and orders them by level
	// When drawing, we can then draw the branches at any depth. This also makes it easier to create a tree that "grows",
	// as we can simply scale the matrices over time (per depth). i.e. Start drawing the cylinder using the first set of
	// matrices (depth 0), and scale by 0<=i<=1 over time to make it grow - then move on to the second set of matrices (depth 1)
	// and continue until we have finished drawing the last set
	void BuildTree();

	void Draw(bool writeMatricesToFile);

	void Reset();

	// Accessors and Mutators
	Cylinder& GetBranchModel();

	void SetGenerations(const u32 generations);
	const u32 GetGenerations() const;

	void SetInitialString(const std::string &initString);
	const std::string GetInitialString() const;

	const float3& GetBranchRotationAngles() const;
	void SetBranchRotationAngles(const float3 &rotationAngles);

	const f32 GetBranchRadius() const;
	void SetBranchRadius(const f32 radius);

	const f32 GetBranchRadiusReduction() const;
	void SetBranchRadiusReduction(const f32 radiusReduction);

	const f32 GetBranchLength() const;
	void SetBranchLength(const f32 length);
};