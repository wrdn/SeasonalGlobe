#include "FractalTree.h"
#include <stack>
#include <fstream>
#include <iostream>
using namespace std;

FractalTree::FractalTree() : branchRadius(_GetDefaultBranchRadius()), branchRadiusReduction(_GetDefaultBranchRadiusReduction()),
	branchLength(_GetDefaultBranchLength()), transformationMatricesArraySize(0), transformationMatrices(0), 
	currentScale(0), lastTime(0), AnimationLevel(0), drawLevel(0), loop_growth(false), leafMatrixCount(0), leafMatrices(0)
{
	rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = DefaultAngle;
	BuildRotationMatrices();
};

FractalTree::FractalTree(FractalTree const& t)
{
	t.DeepCopy(this);

	if(transformationMatricesArraySize)
	{
		transformationMatrices = (Mat44*)_aligned_malloc(sizeof(Mat44) * transformationMatricesArraySize, 16);
		memcpy(transformationMatrices, &t.transformationMatrices, sizeof(Mat44) * transformationMatricesArraySize);
	}
	else
	{
		transformationMatrices = 0;
	}
};

void FractalTree::DeepCopy(const FractalTree *dstp) const
{
	FractalTree &dst = (FractalTree&)*dstp;

	dst.lsysTree = lsysTree;

	memcpy(dst.rotationMatrices, rotationMatrices, sizeof(Mat44) * 6);
	memcpy(dst.rotationAngles, rotationAngles, sizeof(f32) * 3);

	dst.branchRadius = branchRadius;
	dst.branchRadiusReduction = branchRadiusReduction;
	dst.branchLength = branchLength;
	dst.gbranch = gbranch;
	dst.levels = levels;
	dst.treeBranchSegments = treeBranchSegments;
	dst.transformationMatricesArraySize = transformationMatricesArraySize;

	dst.currentScale = currentScale;
	dst.lastTime = lastTime;
	dst.AnimationLevel = AnimationLevel;
	dst.drawLevel = drawLevel;
	dst.loop_growth = loop_growth;
};

FractalTree::~FractalTree()
{
	if(transformationMatrices)
		_aligned_free(transformationMatrices);
	if(leafMatrices)
		_aligned_free(leafMatrices);
};

void FractalTree::Reset()
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

bool FractalTree::AddProductionRule(const c8 axiom, const std::string &replacement)
{
	return lsysTree.AddAxiom(axiom, replacement);
};

void FractalTree::BuildRotationMatrices()
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

void FractalTree::CalculateTreeDepth()
{
	// matrix count at each 'depth' in the tree, max depth is matrixCounts.size()
	std::vector<u32> matrixCounts;
	u32 currentDepth = 0, i=0;
	leafMatrixCount = 0;
	
	for( std::string::const_iterator it = lsysTree.GetEvaluatedString().begin();
		it < lsysTree.GetEvaluatedString().end(); ++it, ++i)
	{
		if(*it == MOVE_FORWARD)
		{
			// Update matrix count for current depth (push new counter if neccessary)
			if(!matrixCounts.size())
				matrixCounts.push_back(0);
			
			++matrixCounts[currentDepth];
		}
		else if(*it == PUSH_MATRIX_STACK)
		{
			// Update depth and push another matrix counter if neccessary
			++currentDepth;
			if(currentDepth >= matrixCounts.size())
				matrixCounts.push_back(0);
		}
		else if(*it == POP_MATRIX_STACK)
		{
			--currentDepth;
		}
		else if(*it == DRAW_LEAF)
		{
			++leafMatrixCount;
		}
	}

	if(leafMatrices) { _aligned_free(leafMatrices); leafMatrices = 0; }
	leafMatrices = (Mat44*)_aligned_malloc(sizeof(Mat44) * leafMatrixCount, 16);
	memset(leafMatrices, 0, sizeof(Mat44)*leafMatrixCount);

	if(matrixCounts.size())
	{
		levels.push_back(0);
		transformationMatricesArraySize = *matrixCounts.begin();

		for(std::vector<u32>::const_iterator it = matrixCounts.begin()+1; it < matrixCounts.end(); ++it)
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

void FractalTree::PruneTree()
{
	for(std::vector<BranchDepth>::iterator it = treeBranchSegments.begin(); it != treeBranchSegments.end(); ++it)
	{
		for(u32 i=0;i<it->segments.size();++i)
		{
			u32 len = it->segments[i].end - it->segments[i].end;
			if(len == 0)
			{
				it->segments.erase( it->segments.begin() + i);
				i = max(0, i-1);
			}
		}
		//// Remove segments with no branches
		//for(std::vector<BranchSegment>::iterator seg = it->segments.begin(); seg != it->segments.end(); ++seg)
		//{
		//	u32 len = seg->end - seg->start;
		//	if(len == 0)
		//	{
		//		it->segments.erase(seg);
		//	}
		//}
	}
};

void FractalTree::BuildTree()
{
	if(!gbranch.Valid())
	{
		gbranch.Create(0.05f, 0.05f, branchLength, 7,7);
	}
	if(!leafModel.Valid())
	{
		leafModel.CreateSphere(0.05f, 10,10);
	}

	lsysTree.Evaluate();
	CalculateTreeDepth();

	// used to hold the count (per depth) of matrices calculated so far
	// required to ensure we don't overwrite a previous matrix in that depth
	std::vector<u32> MatricesCalculatedPerDepth(levels.size(), 0);
	u32 currentDepth = 0;

	u32 currentLeaf=0;

	treeBranchSegments.resize(levels.size(), BranchDepth(0));
	for(u32 i=0;i<treeBranchSegments.size();++i) treeBranchSegments[i].depth = i;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// create and initialise matrix stack
	stack<Mat44> matrixStack;
	Mat44 ma;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)ma.GetMatrix());
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
			++MatricesCalculatedPerDepth[currentDepth];

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(matrixStack.top().GetMatrix());
			glTranslatef(0, branchLength, 0);
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
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
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
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
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
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
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
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
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
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
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
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
			glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)matrixStack.top().GetMatrix());
			glPopMatrix();
		}
		else if(*it == PUSH_MATRIX_STACK)
		{
			currentDepth = min(levels.size(), currentDepth+1);

			Mat44 _m = matrixStack.top();
			matrixStack.push(_m);

			treeBranchSegments[currentDepth].segments.push_back(BranchSegment(levels[currentDepth] + MatricesCalculatedPerDepth[currentDepth], 0));
		}
		else if(*it == POP_MATRIX_STACK)
		{
			treeBranchSegments[currentDepth].segments.back().end = levels[currentDepth] + MatricesCalculatedPerDepth[currentDepth];

			currentDepth = max(0, currentDepth-1);

			matrixStack.pop();
			if(matrixStack.empty()) // reinitialise stack if empty
			{
				Mat44 ma;
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)ma.GetMatrix());
				matrixStack.push(ma);
				glPopMatrix();
			}
		}
		else if(*it == DRAW_LEAF)
		{
			leafMatrices[currentLeaf++] = matrixStack.top();
		}

		glPopMatrix();
	}
	glPopMatrix();

	treeBranchSegments[0].segments.push_back(BranchSegment(0, MatricesCalculatedPerDepth[0]));

	//PruneTree(); // THIS FUNCTION CAUSES TREE GROWTH TO BREAK, FIX OR LEAVE COMMENTED
};

void FractalTree::DrawBranch(const Mat44 &transformationMatrix, const Mat44 &scaleMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(scaleMatrix.Mult(transformationMatrix).GetMatrix());
	gbranch.Draw();
	glPopMatrix();
};
void FractalTree::DrawBranch(const Mat44 &transformationMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(((Mat44&)transformationMatrix).GetMatrix());
	gbranch.Draw();
	glPopMatrix();
};

void FractalTree::DrawLeaves()
{
	for(u32 i=0;i<leafMatrixCount;++i)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(leafMatrices[i].GetMatrix());
		leafModel.Draw();
		glPopMatrix();
	}
};

void FractalTree::Draw(const f32 dt)
{
	stack<Mat44> matrixStack;
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
	Mat44 ma; glPushMatrix(); glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)ma.GetMatrix());
	matrixStack.push(ma); glPopMatrix();

	// Static draw
	if(AnimationLevel < 0)
	{
		for(u32 i=0; i < transformationMatricesArraySize; ++i)
		{
			DrawBranch(transformationMatrices[i]);
		}
		return;
	}

	// Draw levels we aren't animating
	for(u32 i=0; i < levels[AnimationLevel]; ++i)
	{
		DrawBranch(transformationMatrices[i]);
	}


	currentScale += dt;
	if(currentScale >= (1.0 - EPSILON))
	{
		AnimationLevel++;
		if((u32)AnimationLevel >= levels.size())
		{
			if(loop_growth)
			{
				AnimationLevel = 0;
			}
			else
			{
				AnimationLevel = -1;
				return;
			}
		}

		currentScale=0;
	}

	for(u32 i = 0; i < treeBranchSegments[AnimationLevel].segments.size(); ++i)
	{
		f32 t = currentScale;

		BranchSegment &seg = treeBranchSegments[AnimationLevel].segments[i];

		f32 timetb = 3.0f; // time to build each segment
		f32 rate = 1.0f / timetb;
		t += dt * rate;
		if(t >= (1.0f - EPSILON))
		{
			t = 0;
		}

		u32 len = seg.end - seg.start;

		// Don't draw the empty segments (causes graphical glitches)
		if(len == 0) { continue; }

		f32 K = lerp(0, (f32)len, t);
		u32 branchToDisplay = (u32)K;

		for(u32 j=seg.start;j<seg.start+branchToDisplay;++j)
		{
			DrawBranch(transformationMatrices[j]);
		}

		Mat44 scaleMatrix = Mat44::BuildScaleMatrix(1, fract(K), 1);
		DrawBranch(transformationMatrices[seg.start+branchToDisplay], scaleMatrix);
	}
};