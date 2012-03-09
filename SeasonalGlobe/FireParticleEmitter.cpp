#include "FireParticleEmitter.h"
#include "FractalTree.h"
#include "util.h"

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

void randomise_fire_particle_attr(Particle &p, ParticleLine &pline, f32 t0, f32 t1, Color4f &color)
{
	f32 t = randflt(t0, t1);
	p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
	p.pos.y(p.pos.y()-0.2f);
	p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
	p.velocity.normalize();
	p.size.setall(0.4f);
	p.color = color;
	p.energy = randflt(0, 0.85f);
};

void FireParticleEmitter::Emit(Particle &p)
{
	p.InvalidateParticle(); // reset particle

	if(p.pada >= 0)
	{
		ParticleLine &pline = particleLines[(u32)(p.pada+0.01f)]; // line the particle needs to be emitted on, shift pada slightly to avoid any FP accuracy issues

		if(burnState == Burning) { randomise_fire_particle_attr(p, pline, 0, 1, startColor); return; }
		else if(burnState == Dieing)
		{
			if(pline.GetLineDepth() < (u32)burnLevel) { randomise_fire_particle_attr(p, pline, 0, 1, startColor); }
			else { p.color.w(0); } return;
		}
		else if(burnState == Igniting)
		{
			if(pline.GetLineDepth() > (u32)burnLevel) { p.InvalidateParticle(); return; } // lines above burn level, hide particles and return
			if(pline.GetLineDepth() < (u32)burnLevel) { randomise_fire_particle_attr(p, pline, 0, 1, startColor); return; }

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

				if(pline.GetDepthOfLineInSegment() < SegmentIndex) { randomise_fire_particle_attr(p, pline, 0, 1, startColor); return; }
				else if(pline.GetDepthOfLineInSegment() == SegmentIndex) { randomise_fire_particle_attr(p, pline, 0, fract(SegmentLerp), startColor); return; }
				else { p.InvalidateParticle(); return; }
			}
		} // end Igniting code
	} // end p.pada > 0
	
	return;
};

void FireParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
	if(burnState == Dead)
	{
		p.color.zero();
		return;
	}

	p.color = startColor.vec_lerp(endColor, 1.0f / p.energy);
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
	if(!IsActive() || burnState == Dead) { return; }

	if(burnState == Dieing)
	{
		if(runtime >= deathTime)
		{
			burnState = Dead;
			SetActive(false);
			return;
		}

		runtime += gameTime.GetDeltaTime();
		i32 depth = tree->GetDepth();
		f32 K = (1.0f / deathTime) * runtime;
		burnLevel = depth - (i32)lerp(0, (f32)depth,K) - 1;

		tree->SetTreeDeathDepth(max(0, burnLevel));

		f32 DeathTimePerDepth = deathTime/depth;
		f32 P = fmod(runtime,DeathTimePerDepth);
		tree->SetAlpha(1.0f - ((1.0f/DeathTimePerDepth)*P));
	}
	else if(burnState == Igniting)
	{
		if(runtime >= ignitionTime)
		{
			runtime = 0;
			SetBurningState(Burning);
		}

		runtime += gameTime.GetDeltaTime();
		ignitionRuntime += gameTime.GetDeltaTime();
		f32 timePerDepthToIgnite = ignitionTime / tree->GetDepth();
		if(ignitionRuntime > timePerDepthToIgnite) { ignitionRuntime = 0; }
		f32 k = (1.0f / ignitionTime) * runtime;
		burnLevel = (i32)lerp(0, (f32)tree->GetDepth(), k) - 1;
		tree->SetTreeDeathDepth(max(0, burnLevel));
	}
};

void FireParticleEmitter::InitDeath()
{
	tree->SetTreeDieing(true);
	tree->SetTreeDeathDepth(0);
	burnState = Dieing;
	runtime = 0;
};