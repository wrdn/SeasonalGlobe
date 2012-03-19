#pragma once

#include "ParticleEmitter.h"
#include <vector>

class FractalTree;

// Represents a line that particles will be generated along on the fractal tree
// contains start, end and direction, depth of the tree the line is at, which segment it is on, and
// where in the segment (i.e. which matrix in that set of matrices) the line is at
// This information helps when animating the fire
class ParticleLine
{
private:
	float3 startPos, endPos, direction;

	// depth of the tree (branch depth) the particles are at
	// may be useful if we want to fade by depth
	u32 lineDepth;

	u32 segmentIndex; // which segment is this line in
	u32 depthOfLineInSegment; // how far into the segment is this line

public:
	ParticleLine() : startPos(), endPos(), direction(), lineDepth(0), segmentIndex(0), depthOfLineInSegment(0) { };
	ParticleLine(const float3 &start, const float3 &end) : startPos(start), endPos(end),
		direction(endPos-startPos), lineDepth(0), segmentIndex(0), depthOfLineInSegment(0) { };
	~ParticleLine() { };

	// Accessors and Mutators
	void SetSegmentIndex(const u32 index) { segmentIndex = index; }
	const u32 GetSegmentIndex() const { return segmentIndex; }

	void SetDepthOfLineInSegment(const u32 d) { depthOfLineInSegment = d; }
	const u32 GetDepthOfLineInSegment() const { return depthOfLineInSegment; }

	void SetLineDepth(u32 d) { lineDepth = d; };
	void SetStartPosition(const float3 &start)
	{
		startPos = start;
		direction = endPos; direction -= startPos;
	};
	void SetEndPosition(const float3 &end)
	{
		endPos = end;
		direction = endPos; direction -= startPos;
	};
	const u32 GetLineDepth() const { return lineDepth; };
	const float3& GetStartPosition() const { return startPos; };
	const float3& GetEndPosition() const { return endPos; };
	const float3& GetDirection() const { return direction; };
};

// What state is the tree burning in?
enum BurningState
{
	Igniting,
	Burning,
	Dieing,
	Dead,
};

// Fire Particle Emitter - used to emit fire particles along a set of particle lines (lines associated with branches on the tree)
class FireParticleEmitter : public ParticleEmitter
{
private:
	std::vector<ParticleLine> particleLines; // branch particle lines
	u32 currentParticleAdditionIndex; // the index of the last particle we added (in the particle array)
	u32 maxParticlesPerLine; // 20-40 is a reasonable number

	Color4f startColor, endColor; // start and end color of each particle

	// ignition and death time of the tree
	f32 ignitionTime , deathTime;
	
	// maintain the current time into the ignition/deathTime
	f32 runtime, ignitionRuntime;

	// state of the burning tree
	BurningState burnState;

	// depth we are currently burning (and animating) at
	i32 burnLevel;
	
	// holds tree so we can get burn info
	FractalTree *tree;

	void Emit(Particle &p); // emit fire particle

	void UpdateParticleProperties(Particle &p); // lerps color or kills particle if burnState == Dead

public:
	
	FireParticleEmitter();
	~FireParticleEmitter();

	void UpdateFireParticleEmitter(const GameTime &gameTime);

	// Adds the particle line to the list of lines
	// Note: lines should be added in depth order (list is not automatically sorted)
	// Every time you add a line, the particles at particles[currentParticleIndex] to
	// particles[currentParticleIndex+maxParticlesPerLine] will be initialised, with their line indices
	// set to the newly added line
	void AddLine(const ParticleLine &line);

	// Accessors and Mutators
	void SetStartColor(const Color4f &startcol) { startColor = startcol; };
	void SetEndColor(const Color4f &endcol) { endColor = endcol; };
	const Color4f& GetStartColor() const { return startColor; };
	const Color4f& GetEndColor() const { return endColor; };

	void SetTree(FractalTree *t) { tree = t; }

	BurningState GetBurningState() const { return burnState; }
	void SetBurningState(BurningState st) { burnState = st; }

	void SetRuntime(f32 r) { runtime = r; ignitionRuntime = r; }

	const f32 GetDeathTime() const { return deathTime; }
	void SetDeathTime(f32 t) { deathTime = t; }

	const f32 GetIgnitionTime() const { return ignitionTime; }
	void SetIgnitionTime(f32 t) { ignitionTime = t; }

	void SetBurnLevel(i32 level) { burnLevel = level; }

	void InitDeath(); // kills the fire and the tree in sync
};

/*
For each branch matrix:

- Extract translation from matrix = A
- Translate by (0, branchLength, 0)
- Extract translation from matrix = B

Line direction D = B-A
Line: A + tD, 0<=t<=1

Fire Emitter:

- Each particle should have associated with it the Line it will draw across.
  The line must contain the start position and end position (the direction can be calculated as End-Start)
  and the depth the line exists at (depth of tree).

  When we build the tree, we set the number of particles the emitter allows per branch. For each branch matrix,
  we add a line to the fire emitter.
  Splitting the entire particle array (array size / line count), we then associate each particle at i with the
  line.

  When we emit the particle, we will then find the line index and emit at some point along that line.

  Over time, we can fade out tree branches and fade out the particles. To fade out the particles, work backwards
  in the particle array, and slowly fade from depth=max depth to depth=0
  */