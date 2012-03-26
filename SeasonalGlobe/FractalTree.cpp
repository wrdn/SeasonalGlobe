#include "FractalTree.h"
#include "ResourceManager.h"
#include "util.h"
#include <stack>
#include <fstream>
#include <iostream>
using namespace std;

// constructor sets everything to 0
FractalTree::FractalTree() : branchLength(GetDefaultBranchLength()), transformationMatricesArraySize(0), transformationMatrices(0),
	leafMatrixCount(0), leafMatrices(0), loop_growth(false),
	runtime(0), buildTime(15), diffuseTexture(0), normalMap(0), treeShader(0), treeShadeMode(SmoothTextured), alpha(1),
	deathDepth(0), treeDieing(false), isActive(true)
{
	rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = DefaultAngle;
	BuildRotationMatrices();

	ogl.Load();
};

// copy constructor (deep copies arrays)
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

	ogl.Load();
};

// deep copy FractalTree objects, function used by copy constructors/assignment operators
void FractalTree::DeepCopy(const FractalTree *dstp) const
{
	FractalTree &dst = (FractalTree&)*dstp;

	dst.lsysTree = lsysTree;

	memcpy(dst.rotationMatrices, rotationMatrices, sizeof(Mat44) * 6);
	memcpy(dst.rotationAngles, rotationAngles, sizeof(f32) * 3);

	dst.branchLength = branchLength;
	dst.gbranch = gbranch;
	dst.levels = levels;
	dst.treeBranchSegments = treeBranchSegments;
	dst.transformationMatricesArraySize = transformationMatricesArraySize;
	dst.loop_growth = loop_growth;
	dst.leafMatrixCount = leafMatrixCount;
	dst.runtime = runtime;
	dst.buildTime = buildTime;
	dst.diffuseTexture = diffuseTexture;
	dst.normalMap = normalMap;
	dst.deathDepth = deathDepth;
	dst.treeDieing = treeDieing;
	dst.treeShadeMode = treeShadeMode;
	dst.alpha = alpha;
	dst.treeShader = treeShader;
	dst.isActive = isActive;
};

// Delete the arrays using _aligned_free, as allocated using _alligned_malloc
FractalTree::~FractalTree()
{
	if(transformationMatrices)
		_aligned_free(transformationMatrices);
	if(leafMatrices)
		_aligned_free(leafMatrices);
};

// Reset tree and delete transformation matrices and LSystem stuff
void FractalTree::Reset()
{
	delete [] transformationMatrices;
	transformationMatrices = 0;
	levels.clear();

	lsysTree.ClearProductionRules();
	lsysTree.SetLSystemGenerations(0);

	branchLength = GetDefaultBranchLength();

	// default angle = 25 degrees
	rotationAngles[0] = rotationAngles[1] = rotationAngles[2] = DefaultAngle;
	BuildRotationMatrices();
};

// Add rule used to build tree
bool FractalTree::AddProductionRule(const c8 axiom, const std::string &replacement)
{
	return lsysTree.AddAxiom(axiom, replacement);
};

// Build rotation matrices for the tree branches
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

// Precalculate the depth of the tree and the number of matrices at each depth (so we can allocate memory
// once only)
void FractalTree::CalculateTreeDepth()
{
	// matrix count at each 'depth' in the tree, max depth is matrixCounts.size()
	std::vector<u32> matrixCounts;
	u32 currentDepth = 0, i=0;
	leafMatrixCount = 0;
	
	// for every symbol in the evaluated string
	for( std::string::const_iterator it = lsysTree.GetEvaluatedString().begin();
		it < lsysTree.GetEvaluatedString().end(); ++it, ++i)
	{
		if(*it == MOVE_FORWARD) // move forward "F" (i.e. draw cylinder then move). Adds matrix at current depth
		{
			// Update matrix count for current depth (push new counter if neccessary)
			if(!matrixCounts.size())
				matrixCounts.push_back(0);
			
			++matrixCounts[currentDepth];
		}
		else if(*it == PUSH_MATRIX_STACK) // push matrix stack "[", increments current depth
		{
			// Update depth and push another matrix counter if neccessary
			++currentDepth;
			if(currentDepth >= matrixCounts.size())
				matrixCounts.push_back(0);
		}
		else if(*it == POP_MATRIX_STACK) // pops matrix stack "]", decrements current depth
		{
			--currentDepth;
		}
		else if(*it == DRAW_LEAF) // draw leaf "L", increments leaf matrix count
		{
			++leafMatrixCount;
		}
	}

	// create the leaf matrix array
	if(leafMatrices) { _aligned_free(leafMatrices); leafMatrices = 0; }
	leafMatrices = (Mat44*)_aligned_malloc(sizeof(Mat44) * leafMatrixCount, 16);
	memset(leafMatrices, 0, sizeof(Mat44)*leafMatrixCount);

	if(matrixCounts.size())
	{
		// create the transformation matrix array and levels list (based on depth and matrix count at each depth)

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

// Evaluates the LSystem string and if successful, will calculate the tree
// depth (with branches per level), and create the matrices used for drawing
void FractalTree::BuildTree()
{
	if(!gbranch.GetMesh()) // create branch if it doesnt exist
	{
		gbranch.Create(0.05f, 0.05f, branchLength, 7,7);
	}

	lsysTree.Evaluate(); // evaluate LSystem
	CalculateTreeDepth(); // calculate depth and create matrix arrays

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

	// For each character in the LSystem evaluated string
	for( string::const_iterator it = lsysTree.GetEvaluatedString().begin();
		it < lsysTree.GetEvaluatedString().end(); ++it)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrixStack.top().GetMatrix());

		if(*it == MOVE_FORWARD) // "F", create branch matrix at the current depth and add it to the transformation matrix array
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
		else if(*it == ROTATE_LEFT_X) // rotate the matrix at the top of the stack (current matrix) - this rotation will be applied to the next MOVE_FORWARD call
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
		else if(*it == ROTATE_RIGHT_X) // same as ROTATE_LEFT_X but right about x axis
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
		else if(*it == ROTATE_UP_Y) // same as ROTATE_LEFT_X but up around y axis
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
		else if(*it == ROTATE_DOWN_Y) // same as ROTATE_LEFT_X but down about y axis
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
		else if(*it == ROTATE_LEFT_Z) // same as ROTATE_LEFT_X but left about z axis
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
		else if(*it == ROTATE_RIGHT_Z) // same as ROTATE_LEFT_X but right about z axis
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
		else if(*it == PUSH_MATRIX_STACK) // push matrix stack "[", increments depth
		{
			currentDepth = min(levels.size(), currentDepth+1);

			Mat44 _m = matrixStack.top();
			matrixStack.push(_m);

			treeBranchSegments[currentDepth].segments.push_back(BranchSegment(levels[currentDepth] + MatricesCalculatedPerDepth[currentDepth], 0));
		}
		else if(*it == POP_MATRIX_STACK) // pops matrix stack "]", decrements depth
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
		else if(*it == DRAW_LEAF) // creates leaf matrix
		{
			leafMatrices[currentLeaf++] = matrixStack.top();
		}

		glPopMatrix();
	}
	glPopMatrix();

	treeBranchSegments[0].segments.push_back(BranchSegment(0, MatricesCalculatedPerDepth[0]));
};

// Draws branch given transformation matrix and optional scale matrix
void FractalTree::DrawBranch(const Mat44 &transformationMatrix, const Mat44 &scaleMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(scaleMatrix.Mult(transformationMatrix).GetMatrix());

	if(treeShadeMode == NonTexturedNonLitWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	gbranch.DrawSimple();
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
};
void FractalTree::DrawBranch(const Mat44 &transformationMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(((Mat44&)transformationMatrix).GetMatrix());

	if(treeShadeMode == NonTexturedNonLitWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	gbranch.DrawSimple();
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
};

// Draws the tree, animating its growth and death when required
void FractalTree::Draw(f32 dt)
{
	if(!isActive) return;

	stack<Mat44> matrixStack; glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	Mat44 ma; glPushMatrix(); glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)ma.GetMatrix()); matrixStack.push(ma);
	glPopMatrix();

	if(treeDieing)
	{
		// Activate tree material, textures and shaders
		mat.Activate();
		
		if(treeShader) { treeShader->Activate(); }
		ogl.glActiveTexture(GL_TEXTURE0);
		if(diffuseTexture) { diffuseTexture->Activate(); }
		ogl.glActiveTexture(GL_TEXTURE1);
		if(normalMap) { normalMap->Activate(); }

		glTranslatef(treePos.x(), treePos.y(), treePos.z());

		// Draw all the tree branches BELOW the animated death depth (i.e. those not currently fading out)
		for(u32 i=0;i<levels[deathDepth];++i)
			DrawBranch(transformationMatrices[i]);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		mat.SetDiffuse(float4(1,1,1, alpha));
		mat.Activate();

		if(deathDepth == levels.size()-1)
		{
			for(u32 i=levels[deathDepth];i<transformationMatricesArraySize;++i)
				DrawBranch(transformationMatrices[i]);
		}
		else
		{
			for(u32 i=levels[deathDepth];i<levels[deathDepth+1];++i)
				DrawBranch(transformationMatrices[i]);
		}

		glDisable(GL_BLEND);

		mat.SetDiffuse(float4(1,1,1,1));
		mat.Activate();

		if(normalMap) { normalMap->Deactivate(); }
		glPopMatrix();
		ogl.glActiveTexture(GL_TEXTURE0);
		return;
	}
	
	mat.Activate();
	mat.SetDiffuse(float4(1,1,1,alpha));

	if(treeShader) { treeShader->Activate(); }
	ogl.glActiveTexture(GL_TEXTURE0);
	if(diffuseTexture) { diffuseTexture->Activate(); }
	ogl.glActiveTexture(GL_TEXTURE1);
	if(normalMap) { normalMap->Activate(); }

	glTranslatef(treePos.x(), treePos.y(), treePos.z());

	if(runtime < 0)
	{
		for(u32 i=0; i < levels[levels.size()-1]; ++i) { DrawBranch(transformationMatrices[i]); }
		glPopMatrix();

		glDisable(GL_BLEND);
		if(treeShader) { treeShader->Deactivate(); }
		if(diffuseTexture) { diffuseTexture->Deactivate(); }
		if(normalMap) { normalMap->Deactivate(); }
		ogl.glActiveTexture(GL_TEXTURE0);
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
		if(diffuseTexture) { diffuseTexture->Deactivate(); }
		if(normalMap) { normalMap->Deactivate(); }
		ogl.glActiveTexture(GL_TEXTURE0);
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
	if(diffuseTexture) { diffuseTexture->Deactivate(); }
	if(normalMap) { normalMap->Deactivate(); }
	ogl.glActiveTexture(GL_TEXTURE0);
	return;
};

// Calculate the particle lines used for generating fire along tree branches
void FractalTree::CalculateParticleLines(std::vector<ParticleLine> &plines)
{
	u32 depthIndex=0;
	for(std::vector<BranchDepth>::const_iterator td = treeBranchSegments.begin(); td != treeBranchSegments.end(); ++td, ++depthIndex) // depth
	{
		u32 segmentIndex=0;
		for(std::vector<BranchSegment>::const_iterator it = td->segments.begin(); it != td->segments.end(); ++it, ++segmentIndex) // segment
		{
			u32 indx=0;
			for(u32 i = it->start; i < it->end; ++i, ++indx) // line
			{
				Mat44 *startMatrix = &transformationMatrices[i], endMatrix;
				glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
				glMultMatrixf(startMatrix->GetMatrix()); glTranslatef(0, branchLength, 0); // translate along start of branch to its end then store this matrix
				glGetFloatv(GL_MODELVIEW_MATRIX, (f32*)endMatrix.GetMatrix()); glPopMatrix();

				// particle line found by extracting translation from start and end matrix
				ParticleLine p(startMatrix->GetTranslationFromMatrix(), endMatrix.GetTranslationFromMatrix());
				
				// set depth, segment index and depth within this segment
				p.SetLineDepth(depthIndex);
				p.SetSegmentIndex(segmentIndex);
				p.SetDepthOfLineInSegment(indx); // range 0 to n

				plines.push_back(p);
			}
		}
	}
};