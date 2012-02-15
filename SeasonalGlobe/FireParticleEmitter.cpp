#include "FireParticleEmitter.h"
#include "FractalTree.h"

#include <fstream>
using namespace std;

FireParticleEmitter::FireParticleEmitter() : currentParticleAdditionIndex(0), maxParticlesPerLine(25),
	startColor(1,0.2f,0,1), endColor(1, 0.28f, 0, 0.8f), fade(0), ignitionTime(10), deathTime(10), runtime(0), burnState(Igniting),
	burnLevel(0), tree(0), ignitionRuntime(0)
{
};

FireParticleEmitter::~FireParticleEmitter()
{
};

void FireParticleEmitter::Emit(Particle &p)
{
	if(burnState != Igniting) { return; }

	if(p.pada >= 0)
	{
		ParticleLine &pline = particleLines[(u32)(p.pada+0.01f)]; // line the particle needs to be emitted on, shift pada slightly to avoid any FP accuracy issues

			if(pline.GetLineDepth() > (u32)burnLevel) { p.InvalidateParticle(); return; } // lines above burn level, hide particles and return
			if(pline.GetLineDepth() < (u32)burnLevel) // standard emission for particles below the burn level
			{
				f32 t = randflt(0, 1);
				p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
				p.pos.y(p.pos.y()-0.2f);
				p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
				p.velocity.normalize();
				p.size.setall(0.4f);
				p.color = startColor;
				p.energy = randflt(0, 0.85f);
				return;
			}

			if((i32)pline.GetLineDepth() == burnLevel)
			{
				const BranchDepth *branchDepth = &tree->GetBranchSegments()[pline.GetLineDepth()];
				const BranchSegment *branchSegment = &branchDepth->segments[pline.GetSegmentIndex()];
			
				f32 timePerDepthToIgnite = ignitionTime / tree->GetDepth();
				f32 LerpFactor = (1.0f / timePerDepthToIgnite) * ignitionRuntime;

				u32 treeSegmentLength = branchSegment->end - branchSegment->start;
				if(treeSegmentLength == 0) { p.InvalidateParticle(); return; }
				
				f32 SegmentLerp = lerp(0, (f32)treeSegmentLength, LerpFactor);
				u32 SegmentIndex = (u32)SegmentLerp;

				if(pline.GetDepthOfLineInSegment() < SegmentIndex)
				{
					f32 t = randflt(0, 1);
					p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
					p.pos.y(p.pos.y()-0.2f);
					p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
					p.velocity.normalize();
					p.size.setall(0.4f);
					p.color = startColor;
					p.energy = randflt(0, 0.85f);
					return;
				}
				else if(pline.GetDepthOfLineInSegment() == SegmentIndex)
				{
					f32 t = randflt(0, fract(SegmentLerp));
					p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
					p.pos.y(p.pos.y()-0.2f);
					p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
					p.velocity.normalize();
					p.size.setall(0.4f);
					p.color = startColor;
					p.energy = randflt(0, 0.85f);
					return;
				}
				else
				{
					p.InvalidateParticle();
					return;
				}
			}
	}

	return;

	//if(p.pada > 0)
	//{
	//	ParticleLine &pline = particleLines[(u32)(p.pada)];

	//	if(pline.GetLineDepth() > (u32)burnLevel) {  p.color.zero(); p.energy = -1; return; }
	//	if(pline.GetLineDepth() < (u32)burnLevel) { f32 t = randflt(0, 1); p.pos = pline.GetStartPosition() + (t * pline.GetDirection()); p.pos.y(p.pos.y()-0.2f);
	//	p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f)); p.velocity.normalize(); p.size.setall(0.4f); p.color = startColor; p.energy = randflt(0, 0.85f); return; }

	//	// pline.GetLineDepth() must equal burnLevel if we get here
	//	u32 treeDepth = tree->GetDepth();
	//	const BranchDepth *branchDepth = &tree->GetBranchSegments()[pline.GetLineDepth()];
	//	const BranchSegment *branchSegment = &branchDepth->segments[pline.GetSegmentIndex()];
	//	
	//	f32 TimePerDepth = ignitionTime / treeDepth;

	//	f32 BurnLevelStartTime = burnLevel * TimePerDepth;
	//	f32 BurnLevelEndTime = BurnLevelStartTime + TimePerDepth;

	//	f32 CurrentTime = fmod(runtime, BurnLevelStartTime);

	//	f32 LerpFactor = (1.0 / TimePerDepth) * CurrentTime;

	//	u32 treeSegmentLength = branchSegment->end - branchSegment->start;
	//	if(treeSegmentLength == 0) { p.color.zero(); return; }
	//}
};

void FireParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	if(burnState == Dead)
	{
		p.color = Color4f(0,0,0,0);
		return;
	}

	f32 lR = lerp(startColor.r(), endColor.r(), 1.0f / p.energy);
	f32 lG = lerp(startColor.g(), endColor.g(), 1.0f / p.energy);
	f32 lB = lerp(startColor.b(), endColor.b(), 1.0f / p.energy);
	f32 lA = lerp(startColor.a(), endColor.a(), 1.0f / p.energy);

	p.color = Color4f(lR, lG, lB, lA);
};

void FireParticleEmitter::AddLine(const ParticleLine &line)
{
	particleLines.push_back(line);
	u32 index = particleLines.size()-1;

	for(u32 i=currentParticleAdditionIndex; i < (currentParticleAdditionIndex + maxParticlesPerLine); ++i)
	{
		GetParticles()[i].pada = (f32)index;
	}
	currentParticleAdditionIndex += maxParticlesPerLine;

	SetLocalParticleMaximum(currentParticleAdditionIndex);
};

void FireParticleEmitter::UpdateFireParticleEmitter(const GameTime &gameTime)
{
	if(burnState != Igniting) { return; }

	runtime += gameTime.GetDeltaTime();
	ignitionRuntime += gameTime.GetDeltaTime();

	//cout << "Ignition Runtime: " << ignitionRuntime << endl;

	f32 timePerDepthToIgnite = ignitionTime / tree->GetDepth();
	if(ignitionRuntime > timePerDepthToIgnite) { ignitionRuntime = 0; }


	f32 k = (1.0f / 10.0f) * runtime;
	burnLevel = (i32)lerp(0, (f32)tree->GetDepth(), k) - 1;

	tree->SetTreeDeathDepth(max(0, burnLevel));

	if(runtime >= deathTime) { runtime = 0; }

	/*if(!IsActive() && burnState!=Dead) return;

	if(burnState == Dieing)
	{
		if(runtime >= deathTime)
		{
			burnState = Dead;
			SetActive(false);
			return;
		}
	}
	else if(burnState == Igniting)
	{
		if(runtime >= ignitionTime)
		{
			runtime = 0;
			SetBurningState(Burning);
		}
	}

	runtime += gameTime.GetDeltaTime();

	u32 depth=0; f32 K=0;
	if(burnState == Dieing)
	{
		depth = tree->GetDepth();
		K = (1.0f / deathTime) * runtime;
		burnLevel = depth - (i32)lerp(0, (f32)depth,K) - 1;

		tree->SetTreeDeathDepth(max(0, burnLevel));
		
		f32 DeathTimePerDepth = deathTime/depth;
		f32 P = fmod(runtime,DeathTimePerDepth);
		tree->SetAlpha(1.0f - ((1.0f/DeathTimePerDepth)*P));
	}
	else
	{
		depth = tree->GetDepth();
		K = (1.0f / ignitionTime) * runtime;
		burnLevel = (i32)lerp(0, (f32)depth,K);
	}*/
};

void FireParticleEmitter::InitDeath()
{
	tree->SetTreeDieing(true);
	tree->SetTreeDeathDepth(0);
	burnState = Dieing;
	runtime = 0;
};