#pragma once

#include "ParticleEmitter.h"
#include <vector>

class ParticleLine // line particles will be generated along
{
private:
	float3 startPos, endPos, direction;

	// depth of the tree (branch depth) the particles are at
	// may be useful if we want to fade by depth
	u32 lineDepth;

public:
	ParticleLine() : startPos(), endPos(), direction() { };
	ParticleLine(const float3 &start, const float3 &end) : startPos(start), endPos(end),
		direction(endPos-startPos) { };
	~ParticleLine() { };

	void SetLineDepth(u32 d) { lineDepth = d; };
	void SetStartPosition(const float3 &start)
	{
		startPos = start;
		direction = endPos - startPos;
	};
	void SetEndPosition(const float3 &end)
	{
		endPos = end;
		direction = endPos - startPos;
	};
	const u32 GetLineDepth() const { return lineDepth; };
	const float3& GetStartPosition() const { return startPos; };
	const float3& GetEndPosition() const { return endPos; };
	const float3& GetDirection() const { return direction; };
};

class FireParticleEmitter : public ParticleEmitter
{
private:
	std::vector<ParticleLine> particleLines;
	u32 currentParticleAdditionIndex;
	u32 maxParticlesPerLine; // 20-40 is a reasonable number

	Color4f startColor, endColor;

	void Emit(Particle &p);
	void UpdateParticleProperties(Particle &p, const GameTime &gameTime);
public:
	FireParticleEmitter();
	~FireParticleEmitter();

	// Adds the particle line to the list of lines
	// Note: lines should be added in depth order (list is not automatically sorted)
	// Every time you add a line, the particles at particles[currentParticleIndex] to
	// particles[currentParticleIndex+maxParticlesPerLine] will be initialised, with their line indices
	// set to the newly added line
	void AddLine(const ParticleLine &line);

	void SetStartColor(const Color4f &startcol) { startColor = startcol; };
	void SetEndColor(const Color4f &endcol) { endColor = endcol; };
	const Color4f& GetStartColor() const { return startColor; };
	const Color4f& GetEndColor() const { return endColor; };
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