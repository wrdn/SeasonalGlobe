#include "FractalTree.h"
#include <stack>
#include <fstream>
#include <iostream>
using namespace std;

FractalTree::FractalTree() : branchRadius(GetDefaultBranchRadius()), branchRadiusReduction(GetDefaultBranchRadiusReduction()),
	branchLength(GetDefaultBranchLength()), transformationMatricesArraySize(0), transformationMatrices(0),
	leafMatrixCount(0), leafMatrices(0), loop_growth(false),
	runtime(0), buildTime(15), tex(0), treeShader(0), treeShadeMode(SmoothTextured), alpha(0)
{
	rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = DefaultAngle;
	BuildRotationMatrices();
};

FractalTree::FractalTree(FractalTree const& t) : transformationMatrices(0)
{
	t.DeepCopy(this);

	if(transformationMatricesArraySize)
	{
		transformationMatrices = (Mat44*)_aligned_malloc(sizeof(Mat44) * transformationMatricesArraySize, 16);
		memcpy(transformationMatrices, &t.transformationMatrices, sizeof(Mat44) * transformationMatricesArraySize);
	}
	if(leafMatrixCount)
	{
		leafMatrices = (Mat44*)_aligned_malloc(sizeof(Mat44) * leafMatrixCount, 16);
		memcpy(leafMatrices, &t.leafMatrices, sizeof(Mat44)*leafMatrixCount);
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
	dst.loop_growth = loop_growth;
	dst.leafMatrixCount = leafMatrixCount;
	dst.runtime = runtime;
	dst.buildTime = buildTime;
	dst.tex = tex;
	dst.treeShadeMode = treeShadeMode;
	dst.alpha = alpha;
	dst.treeShader = treeShader;
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

	branchRadius = GetDefaultBranchRadius();
	branchRadiusReduction = GetDefaultBranchRadiusReduction();
	branchLength = GetDefaultBranchLength();

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

void FractalTree::BuildTree()
{
	if(!gbranch.GetModel().Valid())
	{
		gbranch.Create(0.05f, 0.05f, branchLength, 7,7);
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
};

void FractalTree::DrawBranch(const Mat44 &transformationMatrix, const Mat44 &scaleMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(scaleMatrix.Mult(transformationMatrix).GetMatrix());
	//gbranch.Draw();
	gbranch.DrawSimple();
	glPopMatrix();
};
void FractalTree::DrawBranch(const Mat44 &transformationMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(((Mat44&)transformationMatrix).GetMatrix());
	//gbranch.Draw();
	gbranch.DrawSimple();
	glPopMatrix();
};

void FractalTree::Draw(f32 dt)
{
	stack<Mat44> matrixStack; glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	Mat44 ma; glPushMatrix(); glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)ma.GetMatrix()); matrixStack.push(ma);
	glPopMatrix();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//mat.SetAmbient(float4(1,1,1,0));
	mat.SetDiffuse(float4(1,1,1,alpha));
	//mat.SetSpecular(float4(1,1,1,1));
	mat.Activate();


	if(treeShader) { treeShader->Activate(); }
	if(tex) { tex->Activate(); }

	glTranslatef(treePos.x(), treePos.y(), treePos.z());

	if(runtime < 0)
	{
		for(u32 i=0; i < levels[levels.size()-1]; ++i) { DrawBranch(transformationMatrices[i]); }
		glPopMatrix();

		glDisable(GL_BLEND);
		if(treeShader) { treeShader->Deactivate(); }
		if(tex) { tex->Deactivate(); }
		return;
	}

	runtime += dt;
	if(runtime >= buildTime) // Finished drawing tree, reset runtime (rebuild tree) or make it static (-1)
	{
		if(loop_growth) { runtime=0; }
		else { runtime = -1; }
		for(u32 i=0; i < levels[levels.size()-1]; ++i) { DrawBranch(transformationMatrices[i]); }
		glPopMatrix();

		glDisable(GL_BLEND);
		if(treeShader) { treeShader->Deactivate(); }
		if(tex) { tex->Deactivate(); }
		return;
	}

	// Animation Level Calculation
	f32 K = (1.0f / buildTime) * runtime;
	f32 lerp_anim_level = lerp(0, (f32)(treeBranchSegments.size()-1), K);
	i32 anim_level = (i32)lerp_anim_level;

	// Draw everything under the animation level
	for(u32 i=0; i < levels[anim_level]; ++i) { DrawBranch(transformationMatrices[i]); }

	// Tree has uniform depth build time (i.e. first depth takes the same amount of time as the last etc)
	// currentModTime is a varible in the range 0 to N, where N is (buildTime/TreeDepth)
	f32 TimePerDepth = buildTime / (levels.size()-1);
	f32 currentModTime = runtime - (anim_level * TimePerDepth);

	// for each branch segment at the current animation level, scale the appropriate branch (and draw the non-scaled ones)
	for(u32 i=0;i<treeBranchSegments[anim_level].segments.size();++i)
	{
		BranchSegment &segment = treeBranchSegments[anim_level].segments[i];
		u32 segmentLength = segment.end - segment.start;
		if(segmentLength == 0) { continue; }

		f32 LerpFactor = (1.0f / TimePerDepth) * currentModTime ; //e.g. 1.0 / (3 seconds) * 1.5 = 0.5
		f32 LerpBranchSeg = lerp(0, (f32)segmentLength, LerpFactor);
		u32 BranchToScale = (u32)LerpBranchSeg;
		f32 ScaleFactor = fract(LerpBranchSeg);

		// draw non scaled branches
		for(u32 j=segment.start;j<segment.start+BranchToScale;++j) { DrawBranch(transformationMatrices[j]); }

		// draw scaled branch
		Mat44 scaleMatrix = Mat44::BuildScaleMatrix(1, ScaleFactor, 1);
		DrawBranch(transformationMatrices[segment.start+BranchToScale], scaleMatrix);
	}

	glDisable(GL_BLEND);

	glPopMatrix();
	if(treeShader) { treeShader->Deactivate(); }
	if(tex) { tex->Deactivate(); }
	return;
};

void FractalTree::CalculateParticleLines(std::vector<ParticleLine> &plines)
{
	u32 depthIndex=0;
	for(std::vector<BranchDepth>::const_iterator td = treeBranchSegments.begin(); td != treeBranchSegments.end(); ++td, ++depthIndex) // depth
	{
		u32 segmentIndex=0;
		for(std::vector<BranchSegment>::const_iterator it = td->segments.begin(); it != td->segments.end(); ++it, ++segmentIndex) // segment
		{
			for(u32 i = it->start; i < it->end; ++i) // line
			{
				Mat44 *startMatrix = &transformationMatrices[i], endMatrix;
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
				glMultMatrixf(startMatrix->GetMatrix()); glTranslatef(0, branchLength, 0);
				glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)endMatrix.GetMatrix()); glPopMatrix();

				ParticleLine p(startMatrix->GetTranslationFromMatrix(), endMatrix.GetTranslationFromMatrix());
				
				p.SetLineDepth(depthIndex);
				p.SetSegmentIndex(segmentIndex);
				p.SetDepthOfLineInSegment(i-it->start); // range 0 to n

				plines.push_back(p);
			}
		}
	}
};