#include "FractalTree2.h"
#pragma warning( disable : 4482 )

FractalTree2::FractalTree2() : transformationMatrices(0), transformationMatricesArraySize(0),
	branchRadius(_GetDefaultBranchRadius()), branchRadiusReduction(_GetDefaultBranchRadiusReduction()),
	branchLength(_GetDefaultBranchLength())
{
	// default angle = 25 degrees
	rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = DefaultAngle;

	BuildRotationMatrices();
};

FractalTree2::~FractalTree2()
{
	//delete [] transformationMatrices;
	if(transformationMatrices)
		_aligned_free(transformationMatrices);
};

void FractalTree2::Reset()
{
	delete [] transformationMatrices;
	transformationMatrices = 0;
	levels.clear();

	lsysTree.ClearProductionRules();
	lsysTree.SetLSystemGenerations(0);

	branchRadius = _GetDefaultBranchRadius();
	branchRadiusReduction = _GetDefaultBranchRadiusReduction();
	branchLength = _GetDefaultBranchLength();

	// default angle = 25 degrees
	rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = DefaultAngle;
	BuildRotationMatrices();
};

bool FractalTree2::AddProductionRule(const c8 axiom, const std::string &replacement)
{
	return lsysTree.AddAxiom(axiom, replacement);
};

void FractalTree2::BuildRotationMatrices()
{
	// The matrices are already identity matrices, so we only 
	// need to set individual elements

	// X Rotation Matrix
	f32 *LeftXMatrix = rotationMatrices[RotationMatrixIndex::LeftX].GetMatrix();
	f32 *RightXMatrix = rotationMatrices[RotationMatrixIndex::RightX].GetMatrix();
	f32 angle = rotationAngles[0];

	LeftXMatrix[m22] = cos(angle);
	LeftXMatrix[m23] = -sin(angle);
	LeftXMatrix[m32] = sin(angle);
	LeftXMatrix[m33] = cos(angle);

	angle = -angle;
	RightXMatrix[m22] = cos(angle);
	RightXMatrix[m23] = -sin(angle);
	RightXMatrix[m32] = sin(angle);
	RightXMatrix[m33] = cos(angle);

	// Y Rotation Matrix
	f32 *UpYMatrix = rotationMatrices[RotationMatrixIndex::UpY].GetMatrix();
	f32 *DownYMatrix = rotationMatrices[RotationMatrixIndex::DownY].GetMatrix();
	angle = rotationAngles[1];
	
	UpYMatrix[m11] = cos(angle);
	UpYMatrix[m13] = sin(angle);
	UpYMatrix[m31] = -sin(angle);
	UpYMatrix[m33] = cos(angle);

	angle = -angle;
	DownYMatrix[m11] = cos(angle);
	DownYMatrix[m13] = sin(angle);
	DownYMatrix[m31] = -sin(angle);
	DownYMatrix[m33] = cos(angle);

	// Z Rotation Matrix
	f32 *LeftZMatrix = rotationMatrices[RotationMatrixIndex::LeftZ].GetMatrix();
	f32 *RightZMatrix = rotationMatrices[RotationMatrixIndex::RightZ].GetMatrix();
	angle = rotationAngles[2];

	LeftZMatrix[m11] = cos(angle);
	LeftZMatrix[m12] = -sin(angle);
	LeftZMatrix[m21] = sin(angle);
	LeftZMatrix[m22] = cos(angle);

	angle = -angle;
	RightZMatrix[m11] = cos(angle);
	RightZMatrix[m12] = -sin(angle);
	RightZMatrix[m21] = sin(angle);
	RightZMatrix[m22] = cos(angle);
};

void FractalTree2::CalculateTreeDepth()
{
	lsysTree.Evaluate(); // todo: remove this, debugging aid only

	// matrix count at each 'depth' in the tree, max depth is matrixCounts.size()
	std::vector<u32> matrixCounts;
	u32 currentDepth = 0;

	for( std::string::const_iterator it = lsysTree.GetEvaluatedString().begin();
		it < lsysTree.GetEvaluatedString().end(); ++it)
	{
		if(*it == SYMBOL_SET::MOVE_FORWARD)
		{
			if(!matrixCounts.size())
				matrixCounts.push_back(1);
			else
				++matrixCounts[currentDepth];
		}
		else if(*it == SYMBOL_SET::PUSH_MATRIX_STACK)
		{
			if((currentDepth+1) >= matrixCounts.size())
				matrixCounts.push_back(0);
			++currentDepth;
		}
		else if(*it == SYMBOL_SET::POP_MATRIX_STACK)
		{
			--currentDepth;
		}
	}

	if(matrixCounts.size())
	{
		levels.push_back(0);
		transformationMatricesArraySize = *matrixCounts.begin();

		for(std::vector<u32>::const_iterator it = matrixCounts.begin()+1;
			it < matrixCounts.end(); ++it)
		{
			levels.push_back(transformationMatricesArraySize);
			transformationMatricesArraySize += *it; // sum matrix counts per level
		}

		if(transformationMatrices) { _aligned_free(transformationMatrices); }
		transformationMatrices = (Mat44*)_aligned_malloc(
			transformationMatricesArraySize * sizeof(Mat44), 16);
		
		// Set each matrix to the identity matrix
		for(u32 i=0;i<transformationMatricesArraySize;++i)
			transformationMatrices[i].Identity();
	}
};