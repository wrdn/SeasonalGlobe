#include "FireParticleEmitter.h"
#include "FractalTree.h"

FireParticleEmitter::FireParticleEmitter() : currentParticleAdditionIndex(0), maxParticlesPerLine(25),
	startColor(1,0.2f,0,1), endColor(1, 0.28f, 0, 0.8f), fade(0), ignitionTime(20), runtime(0), burnState(Igniting),
	burnLevel(0), tree(0), K(0)
{
};

FireParticleEmitter::~FireParticleEmitter()
{
};

void FireParticleEmitter::Emit(Particle &p)
{
	if(p.pada >= 0)
	{
		ParticleLine &pline = particleLines[(u32)p.pada]; // line the particle needs to be emitted on

		tree->SetAlpha(1.0f);
		
		if(burnState == Dieing)
		{
			tree->SetAlpha(0.5f);

			if(pline.GetLineDepth() <= burnLevel) // only generate particles below the burn level
			{
				f32 t = randflt(0, 1);
				p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
				p.pos.y(p.pos.y()-0.2f);
				p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
				p.velocity.normalize();
				p.size.setall(0.4f);
				p.color = startColor;
				p.energy = randflt(0, 0.85f);
			}
			else
			{
				p.color.w(0);
			}

			return;
		}
		
		
		
		
		
		
		
		
		
		
		
		if(burnState == Burning)
		{
			// Ray equation: O + tD, where O is origin, D is direction and t is distance along line. End point
			// of ParticleLine is startPos + (1*direction)
			f32 t = randflt(0, 1);
			p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
			p.pos.y(p.pos.y()-0.2f);

			p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
			p.velocity.normalize();

			p.size.setall(0.4f);

			p.color = startColor;
			p.energy = randflt(0, 0.85f);
		}



		else if(burnState == Igniting)
		{
			if(pline.GetLineDepth() > burnLevel) {  p.color = float4(0,0,0,0); p.energy = -1; return; }

			if(pline.GetLineDepth() < burnLevel) { f32 t = randflt(0, 1); p.pos = pline.GetStartPosition() + (t * pline.GetDirection()); p.pos.y(p.pos.y()-0.2f);
			p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f)); p.velocity.normalize(); p.size.setall(0.4f); p.color = startColor; p.energy = randflt(0, 0.85f); return; }


			if(pline.GetLineDepth() == burnLevel)
			{
				// Do the LERP over the segment in here (pline line depth == burnLevel)
				u32 depth = tree->GetDepth();
				BranchDepth &d = (BranchDepth&)tree->GetBranchSegments()[pline.GetLineDepth()];
				BranchSegment &particlesBranchSegment = d.segments[pline.GetSegmentIndex()];

				u32 timePerDepth = ignitionTime / depth;
				f32 P = runtime - (burnLevel * timePerDepth);
				f32 LFactor = (1.0f / timePerDepth) * P;
				LFactor = min(0.99999f, LFactor);
				u32 segLen = particlesBranchSegment.end - particlesBranchSegment.start;
				if(segLen == 0) { p.color = float4(0,0,0,0); p.energy = -1; return; }

				f32 J = lerp(0, segLen, LFactor);
				u32 N = J;

				if(pline.GetDepthOfLineInSegment() < N)
				{
					f32 t = randflt(0, 1); p.pos = pline.GetStartPosition() + (t * pline.GetDirection()); p.pos.y(p.pos.y()-0.2f);
					p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f)); p.velocity.normalize(); p.size.setall(0.4f); p.color = startColor; p.energy = randflt(0, 0.85f); return;
				}
				else if(pline.GetDepthOfLineInSegment() == N)
				{
					f32 t = randflt(0, fract(J)); p.pos = pline.GetStartPosition() + (t * pline.GetDirection()); p.pos.y(p.pos.y()-0.2f);
					p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f)); p.velocity.normalize(); p.size.setall(0.4f); p.color = startColor; p.energy = randflt(0, 0.85f); return;
				}
				else
				{
					p.color = float4(0,0,0,0); p.energy = -1; return;
				}
			}

			/*if(pline.GetLineDepth() > burnLevel)
			{
				p.color = float4(0,0,0,0);
				p.energy = -1;
				return;
			}
			else if(pline.GetLineDepth() <= burnLevel)
			{
				// Ray equation: O + tD, where O is origin, D is direction and t is distance along line. End point
				// of ParticleLine is startPos + (1*direction)
				f32 t = randflt(0, 1);
				p.pos = pline.GetStartPosition() + (t * pline.GetDirection());
				p.pos.y(p.pos.y()-0.2f);

				p.velocity = float3(0.2f, randflt(0,1), randflt(-0.3f,0.3f));
				p.velocity.normalize();

				p.size.setall(0.4f);

				p.color = startColor;
				p.energy = randflt(0, 0.85f);
			}*/
		}
	}
};

void FireParticleEmitter::UpdateParticleProperties(Particle &p/*, const GameTime &gameTime*/)
{
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
	//burnState = Dieing;
	burnState = Igniting;
	
	runtime += gameTime.GetDeltaTime();

	if(runtime > ignitionTime)
	{
		runtime = 0;
		burnState = Burning; // static burning
		return;
	}

	u32 depth = tree->GetDepth();
	f32 K = (1.0f / ignitionTime) * runtime;
	burnLevel = (i32)lerp(0,depth,K);

	/*
	//u32 depth = this->particleLines.back().GetLineDepth();
	u32 depth = tree->GetDepth()-1;

	K = (1.0f / ignitionTime) * runtime;
	f32 lerp_anim_level = lerp(0, (f32)depth+1, K);
	//burnLevel = depth - (u32)lerp_anim_level;
	burnLevel = depth - lerp_anim_level - 1;

	f32 timePerDepth = ignitionTime / depth+1;
	f32 cmodtime = runtime - (burnLevel * timePerDepth);
	f32 a = (1.0f/timePerDepth) * cmodtime;*/
	
	tree->SetTreeDieing(false);
	//tree->SetAlpha(max(0.2, 1-fract(a)));
	//tree->SetTreeDeathDepth(max(0,burnLevel));
};