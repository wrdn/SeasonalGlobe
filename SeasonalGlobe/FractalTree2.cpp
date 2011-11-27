#include "FractalTree2.h"
#include <stack>
#include <fstream>
#include <iostream>
using namespace std;
//#pragma warning( disable : 4482 )

FractalTree2::FractalTree2() : transformationMatrices(0), transformationMatricesArraySize(0),
	branchRadius(_GetDefaultBranchRadius()), branchRadiusReduction(_GetDefaultBranchRadiusReduction()),
	branchLength(_GetDefaultBranchLength()), currentScale(0), AnimationLevel(0)
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
	f32 angle = rotationAngles[0];
	rotationMatrices[LeftX] = Mat44::BuildRotationMatrix(-angle, 1, 0, 0);
	rotationMatrices[RightX] = Mat44::BuildRotationMatrix(angle, 1, 0, 0);

	// Y Rotation Matrix
	angle = rotationAngles[1];
	rotationMatrices[UpY] = Mat44::BuildRotationMatrix(angle, 0, 1, 0);
	rotationMatrices[DownY] = Mat44::BuildRotationMatrix(-angle, 0, 1, 0);

	// Z Rotation Matrix
	angle = rotationAngles[2];
	rotationMatrices[LeftZ] = Mat44::BuildRotationMatrix(angle, 0, 0, 1);
	rotationMatrices[RightZ] = Mat44::BuildRotationMatrix(-angle, 0, 0, 1);
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
			glMultMatrixf(rotationMatrices[LeftX].GetMatrix());
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
			glMultMatrixf(rotationMatrices[RightX].GetMatrix());
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
			glMultMatrixf(rotationMatrices[UpY].GetMatrix());
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
			glMultMatrixf(rotationMatrices[DownY].GetMatrix());
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
			glMultMatrixf(rotationMatrices[LeftZ].GetMatrix());
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
			glMultMatrixf(rotationMatrices[RightZ].GetMatrix());
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

void FractalTree2::Draw(float dt)
{
	CalculateAnimationLevel(dt); // update scale and level of the tree we are animating

	// Initialisation
	stack<Mat44> matrixStack;
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
	Mat44 ma; glPushMatrix(); glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, ma.GetMatrix());
	matrixStack.push(ma); glPopMatrix();
	
	//u32 _end=0; // each level under animation level
	//_end = levels[AnimationLevel];

	// Draw each level below the animation level with a scale of 1.0 (No Scale)
	/*for(u32 i = levels[0]; i < _end; ++i)
	{
		glMatrixMode(GL_MODELVIEW); glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());
		glMatrixMode(GL_MODELVIEW); glPushMatrix();
		glMultMatrixf(transformationMatrices[i].GetMatrix());
		gbranch.Draw();
		glPopMatrix();
	}
	*/

	Mat44 scaleMatrix = Mat44::BuildScaleMatrix(currentScale, currentScale, currentScale);
	u32 _start = 0;
	u32 _end = transformationMatricesArraySize;

	int P = ( _end - _start); // number of branches (cylinder)
	float Range = 1.0 / P; // range of time (0..1) we have to draw each branch (cylinder) in
	float t = currentScale; // time
	float Q = t / Range;
	int BranchToScale = (int)Q; // index of the branch (in the current depth) we are scaling
	float BranchScaleFactor = fract(Q);
	scaleMatrix = Mat44::BuildScaleMatrix(1, BranchScaleFactor, 1);

	for(u32 i = _start; i < (_start+BranchToScale); ++i) // non scaled branch
	{
		glMatrixMode(GL_MODELVIEW); glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());
		glMatrixMode(GL_MODELVIEW); glPushMatrix();
		glMultMatrixf(transformationMatrices[i].GetMatrix());
		gbranch.Draw();
		glPopMatrix();
	}
	for(u32 i = BranchToScale; i < BranchToScale+1; ++i)
	{
		glMatrixMode(GL_MODELVIEW); glPushMatrix();
		//glMultMatrixf(transformationMatrices[i].GetMatrix());
		glMultMatrixf(scaleMatrix.Mult(transformationMatrices[i]).GetMatrix());
		gbranch.Draw();
		glPopMatrix();
	}

	glPopMatrix();

	return;

	/*glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	currentScale += dt;
	Mat44 scaleMatrix = Mat44::BuildScaleMatrix(currentScale, currentScale, currentScale);

	stack<Mat44> matrixStack;

	Mat44 ma;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, ma.GetMatrix());
	matrixStack.push(ma);
	glPopMatrix();

	for(u32 i=levels[0]; i <= transformationMatricesArraySize;++i)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//glMultMatrixf(transformationMatrices[i].GetMatrix());
		glMultMatrixf(transformationMatrices[i].Mult(scaleMatrix).GetMatrix());
		gbranch.Draw();
		glPopMatrix();
	}*/

	/*for(u32 i=0;i<transformationMatricesArraySize;++i)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//glMultMatrixf(transformationMatrices[i].GetMatrix());
		glMultMatrixf(transformationMatrices[i].Mult(scaleMatrix).GetMatrix());
		gbranch.Draw();
		glPopMatrix();
	}*/

	/*glPopMatrix();

	if(currentScale > 1.0f)
		currentScale = 0.0f;*/
};