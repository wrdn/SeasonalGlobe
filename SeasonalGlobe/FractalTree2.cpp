#include "FractalTree2.h"
#include <stack>
#include <fstream>
#include <iostream>
using namespace std;
//#pragma warning( disable : 4482 )

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
	f32 *LeftXMatrix = rotationMatrices[LeftX].GetMatrix();
	f32 *RightXMatrix = rotationMatrices[RightX].GetMatrix();
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
	f32 *UpYMatrix = rotationMatrices[UpY].GetMatrix();
	f32 *DownYMatrix = rotationMatrices[DownY].GetMatrix();
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
	f32 *LeftZMatrix = rotationMatrices[LeftZ].GetMatrix();
	f32 *RightZMatrix = rotationMatrices[RightZ].GetMatrix();
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
	// matrix count at each 'depth' in the tree, max depth is matrixCounts.size()
	std::vector<u32> matrixCounts;
	u32 currentDepth = 0;

	for( std::string::const_iterator it = lsysTree.GetEvaluatedString().begin();
		it < lsysTree.GetEvaluatedString().end(); ++it)
	{
		if(*it == MOVE_FORWARD)
		{
			if(!matrixCounts.size())
				matrixCounts.push_back(1);
			else
				++matrixCounts[currentDepth];
		}
		else if(*it == PUSH_MATRIX_STACK)
		{
			if((currentDepth+1) >= matrixCounts.size())
				matrixCounts.push_back(0);
			++currentDepth;
		}
		else if(*it == POP_MATRIX_STACK)
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

void FractalTree2::BuildTree(bool dbg_writeMatricesToFile)
{
	if(!gbranch.Valid())
		gbranch.Create(0.05f, 0.05f, branchLength, 7,7);

	lsysTree.Evaluate();
	CalculateTreeDepth();

	// used to hold the count (per depth) of matrices calculated so far
	// required to ensure we don't overwrite a previous matrix in that depth
	std::vector<u32> MatricesCalculatedPerDepth(levels.size(), 0);
	u32 currentDepth = 0;

	ofstream out;
	if(dbg_writeMatricesToFile) { out = ofstream("FractalTree2_BuildTree_Matrices.txt"); }

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// create and initialise matrix stack
	stack<Mat44> matrixStack;
	Mat44 ma;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, ma.GetMatrix());
	matrixStack.push(ma);
	glPopMatrix();

	for( string::const_iterator it = lsysTree.GetEvaluatedString().begin();
		it < lsysTree.GetEvaluatedString().end(); ++it)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());

		if(*it == MOVE_FORWARD)
		{
			// Grab matrix and store in valid location for currentDepth then
			// translate matrix stack top
			transformationMatrices[levels[currentDepth] + MatricesCalculatedPerDepth[currentDepth]] = matrixStack.top();
			if(dbg_writeMatricesToFile) { out << matrixStack.top() << endl; }
			++MatricesCalculatedPerDepth[currentDepth];

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glTranslatef(0, branchLength, 0);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == ROTATE_LEFT_X)
		{
			// Perform rotation as normal
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glRotatef(rotationAngles[0], 1,0,0);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == ROTATE_RIGHT_X)
		{
			// Perform rotation as normal
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glRotatef(-rotationAngles[0], 1,0,0);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == ROTATE_UP_Y)
		{
			// Perform rotation as normal
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glRotatef(rotationAngles[1],0,1,0);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == ROTATE_DOWN_Y)
		{
			// Perform rotation as normal
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glRotatef(-rotationAngles[1],0,1,0);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == ROTATE_LEFT_Z)
		{
			// Perform rotation as normal
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glRotatef(rotationAngles[2],0,0,1);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == ROTATE_RIGHT_Z)
		{
			// Perform rotation as normal
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glRotatef(-rotationAngles[2],0,0,1);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == PUSH_MATRIX_STACK)
		{
			currentDepth = min(levels.size(), currentDepth+1);

			Mat44 _m = matrixStack.top();
			matrixStack.push(_m);
		}
		else if(*it == POP_MATRIX_STACK)
		{
			currentDepth = max(0, currentDepth-1);

			matrixStack.pop();
			if(matrixStack.empty()) // reinitialise stack if empty
			{
				Mat44 ma;
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glGetFloatv(GL_MODELVIEW_MATRIX, ma.GetMatrix());
				matrixStack.push(ma);
				glPopMatrix();
			}
		}

		glPopMatrix();
	}
	glPopMatrix();

	if(dbg_writeMatricesToFile && out.is_open())
		out.close();
};

void FractalTree2::Draw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	stack<Mat44> matrixStack;

	Mat44 ma;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, ma.GetMatrix());
	matrixStack.push(ma);
	glPopMatrix();

	for(u32 i=0;i<transformationMatricesArraySize;++i)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(transformationMatrices[i].GetMatrix());
		gbranch.Draw();
		glPopMatrix();
	}
	glPopMatrix();
};