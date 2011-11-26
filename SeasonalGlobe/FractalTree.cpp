#include "FractalTree.h"

#pragma warning( disable : 4482 )

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
	SetBranchRotationAngles(_branchRotationAngles);
	SetBranchRadius(_branchRadius);
	SetBranchRadiusReduction(_branchRadiusReduction);
	SetBranchLength(_branchLength);

	delete _branchModel;
	_branchModel = new Cylinder();
	_branchModel->Create(0.05f, 0.05f, branchLength, 20, 20);
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

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	InitMatrixStack();

	f32 _radius = branchRadius;

	for( u32 i = 0; i < _lsystree.GetEvaluatedString().length(); ++i )
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glMultMatrixf(matrixStack.top().GetMatrix());

		c8 symbol = _lsystree.GetEvaluatedString()[i];

		switch(symbol)
		{
		case MOVE_FORWARD:
			{
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glScalef(_radius, _radius, _radius);
				_branchModel->Draw();
				//_radius -= branchRadiusReduction;
				glPopMatrix();

				Translate(0, branchLength, 0);
			} break;
		case ROTATE_LEFT_X:
			{
				Rotate(branchRotationAngles.x(), 1,0,0);
			} break;
		case ROTATE_RIGHT_X:
			{
				Rotate(-branchRotationAngles.x(), 1,0,0);
			} break;
		case ROTATE_UP_Y:
			{
				Rotate(branchRotationAngles.y(), 0, 1, 0);
			} break;
		case ROTATE_DOWN_Y:
			{
				Rotate(-branchRotationAngles.y(), 0, 1, 0);
			} break;
		case ROTATE_LEFT_Z:
			{
				Rotate(branchRotationAngles.z(), 0, 0, 1);
			} break;
		case ROTATE_RIGHT_Z:
			{
				Rotate(-branchRotationAngles.z(), 0, 0, 1);
			} break;
		case PUSH_MATRIX_STACK:
			{
				PushStack();
			} break;
		case POP_MATRIX_STACK:
			{
				PopStack();
			} break;
		}

		glPopMatrix();
	}

	glPopMatrix();
};

void FractalTree::Reset()
{
	_lsystree.ClearProductionRules();
};

void FractalTree::BuildXRotationMatrices()
{
	f32 *LeftMatrix = rotationMatrices[RotationMatrixIndex::LeftX].GetMatrix();
	f32 *RightMatrix = rotationMatrices[RotationMatrixIndex::RightX].GetMatrix();

	f32 angle = branchRotationAngles.x();
	LeftMatrix[m22] = cos(angle);
	LeftMatrix[m23] = -sin(angle);
	LeftMatrix[m32] = sin(angle);
	LeftMatrix[m33] = cos(angle);

	angle = -angle;
	RightMatrix[m22] = cos(angle);
	RightMatrix[m23] = -sin(angle);
	RightMatrix[m32] = sin(angle);
	RightMatrix[m33] = cos(angle);
};
void FractalTree::BuildYRotationMatrices()
{
	f32 *UpMatrix = rotationMatrices[RotationMatrixIndex::UpY].GetMatrix();
	f32 *DownMatrix = rotationMatrices[RotationMatrixIndex::DownY].GetMatrix();

	f32 angle = branchRotationAngles.y();
	UpMatrix[m11] = cos(angle);
	UpMatrix[m13] = sin(angle);
	UpMatrix[m31] = -sin(angle);
	UpMatrix[m33] = cos(angle);

	angle = -angle;
	DownMatrix[m11] = cos(angle);
	DownMatrix[m13] = sin(angle);
	DownMatrix[m31] = -sin(angle);
	DownMatrix[m33] = cos(angle);
};
void FractalTree::BuildZRotationMatrices()
{
	f32 *LeftMatrix = rotationMatrices[RotationMatrixIndex::LeftZ].GetMatrix();
	f32 *RightMatrix = rotationMatrices[RotationMatrixIndex::RightZ].GetMatrix();

	f32 angle = branchRotationAngles.z();
	LeftMatrix[m11] = cos(angle);
	LeftMatrix[m12] = -sin(angle);
	LeftMatrix[m21] = sin(angle);
	LeftMatrix[m22] = cos(angle);

	angle = -angle;
	RightMatrix[m11] = cos(angle);
	RightMatrix[m12] = -sin(angle);
	RightMatrix[m21] = sin(angle);
	RightMatrix[m22] = cos(angle);
};
void FractalTree::BuildAllRotationMatrices()
{
	BuildXRotationMatrices();
	BuildYRotationMatrices();
	BuildZRotationMatrices();
};

// Accessors and Mutators
Cylinder& FractalTree::GetBranchModel() { return *_branchModel; };

void FractalTree::SetGenerations(const u32 generations) { _lsystree.SetLSystemGenerations(generations); };
const u32 FractalTree::GetGenerations() const { return _lsystree.GetLSystemGenerations(); };

void FractalTree::SetInitialString(const std::string &initString) { _lsystree.SetStartingAxiom(initString); };
const std::string FractalTree::GetInitialString() const { return _lsystree.GetStartingAxiom(); };

const float3& FractalTree::GetBranchRotationAngles() const { return branchRotationAngles; };
void FractalTree::SetBranchRotationAngles(const float3 &rotationAngles)
{
	branchRotationAngles = rotationAngles;
	BuildAllRotationMatrices();
};

const f32 FractalTree::GetBranchRadius() const { return branchRadius; };
void FractalTree::SetBranchRadius(const f32 radius) { branchRadius = radius; };

const f32 FractalTree::GetBranchRadiusReduction() const { return branchRadiusReduction; };
void FractalTree::SetBranchRadiusReduction(const f32 radiusReduction) { branchRadiusReduction = radiusReduction; };

const f32 FractalTree::GetBranchLength() const { return branchLength; };
void FractalTree::SetBranchLength(const f32 length) { branchLength = length; };


void FractalTree::InitMatrixStack()
{
	matrixStack = stack<Mat44>();
	Mat44 mat;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mat.GetMatrix());
	glPopMatrix();

	matrixStack.push(mat);
};

void FractalTree::PopStack()
{
	matrixStack.pop();

	if(matrixStack.empty())
		InitMatrixStack();
};

void FractalTree::PushStack()
{
	Mat44 _m = matrixStack.top();
	matrixStack.push(_m);
};

void FractalTree::Rotate(float angle, float x, float y, float z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(matrixStack.top().GetMatrix());
	glRotatef(angle, x,y,z);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
	glPopMatrix();
};
void FractalTree::Translate(float tx, float ty, float tz)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(matrixStack.top().GetMatrix());
	glTranslatef(tx, ty, tz);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
	glPopMatrix();
};