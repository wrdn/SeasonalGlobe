#include "FractalTree.h"

const f32 FractalTree::DEFAULT_BRANCH_RADIUS = 0.2f;
const f32 FractalTree::DEFAULT_BRANCH_RADIUS_REDUCTION = 0.005f;
const f32 FractalTree::DEFAULT_BRANCH_LENGTH = 0.5f;
const float3 FractalTree::DEFAULT_ROTATION_ANGLES = float3(25, 25, 25);

FractalTree::FractalTree(void)
{
	_branchModel = 0;
}

FractalTree::~FractalTree(void)
{
	delete _branchModel;
}

void FractalTree::Init()
{
	Init(DEFAULT_ROTATION_ANGLES, DEFAULT_BRANCH_RADIUS, DEFAULT_BRANCH_RADIUS_REDUCTION, DEFAULT_BRANCH_LENGTH);
};

void FractalTree::Init(const float3 _branchRotationAngles, const f32 _branchRadius, 
	const f32 _branchRadiusReduction, const f32 _branchLength)
{
	branchRotationAngles = _branchRotationAngles;
	branchRadius = _branchRadius;
	branchRadiusReduction = _branchRadiusReduction;
	branchLength = _branchLength;

	delete _branchModel;
	_branchModel = new Cylinder();
	_branchModel->Create(branchRadius, branchRadius, branchLength, 20, 20);
};

bool FractalTree::AddProductionRule(const c8 symbol, const std::string &replacement)
{
	return _lsystree.AddAxiom(symbol, replacement);
};

void FractalTree::EvaluateTreeLSystem()
{
	_lsystree.Evaluate();
	_lsystree.ConvertEvaluatedStringToUpperCase();
};

void FractalTree::Draw()
{
	//glTranslatef(0, _branchModel.GetHeight()/2, 0);
	//_branchModel.Draw();
	//return;

	// This code will be replaced (just testing the LSystem will actually work properly)
	// Consider each element of the string, and act accordingly

	for( u32 i = 0; i < _lsystree.GetEvaluatedString().length(); ++i )
	{
		c8 symbol = _lsystree.GetEvaluatedString()[i];

		if(symbol == MOVE_FORWARD)
		{
			_branchModel->Draw();
			glTranslatef(0, branchLength, 0);
		}
	}
};

void FractalTree::Reset()
{
	_lsystree.ClearProductionRules();
};

// Accessors and Mutators
Cylinder& FractalTree::GetBranchModel() { return *_branchModel; };

void FractalTree::SetGenerations(const u32 generations) { _lsystree.SetLSystemGenerations(generations); };
const u32 FractalTree::GetGenerations() const { return _lsystree.GetLSystemGenerations(); };

void FractalTree::SetInitialString(const std::string &initString) { _lsystree.SetStartingAxiom(initString); };
const std::string FractalTree::GetInitialString() const { return _lsystree.GetStartingAxiom(); };

const float3& FractalTree::GetBranchRotationAngles() const { return branchRotationAngles; };
void FractalTree::SetBranchRotationAngles(const float3 &rotationAngles) { branchRotationAngles = rotationAngles; };

const f32 FractalTree::GetBranchRadius() const { return branchRadius; };
void FractalTree::SetBranchRadius(const f32 radius) { branchRadius = radius; };

const f32 FractalTree::GetBranchRadiusReduction() const { return branchRadiusReduction; };
void FractalTree::SetBranchRadiusReduction(const f32 radiusReduction) { branchRadiusReduction = radiusReduction; };

const f32 FractalTree::GetBranchLength() const { return branchLength; };
void FractalTree::SetBranchLength(const f32 length) { branchLength = length; };