#include "FireParticleEmitter.h"
#include "FractalTree.h"

FireParticleEmitter::FireParticleEmitter() : currentParticleAdditionIndex(0), maxParticlesPerLine(25),
	startColor(1,0.2f,0,1), endColor(1, 0.28f, 0, 0.8f), ignitionTime(10), runtime(0), burnState(Igniting),
	burnLevel(0), tree(0)
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
			if(pline.GetLineDepth() < burnLevel)
			{
				p.color = float4(0,0,0,0);
				p.energy = -1;
				return;
			}
			else if(pline.GetLineDepth() >= burnLevel)
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

			/*if(pline.GetLineDepth() == burnLevel)
			{
				const BranchDepth _d = tree->GetBranchSegments()[pline.GetLineDepth()];
				const BranchSegment _s = _d.segments[pline.GetSegmentIndex()];
				f32 tpd = ignitionTime / tree->GetDepth(); //particleLines.back().GetLineDepth();
				f32 cmodtime = runtime - (burnLevel * tpd);

				f32 lfactor = (1.0 / tpd) * cmodtime;
				f32 f = lerp(0, (f32)_s.end - _s.start, lfactor);
				u32 b = f;

				if(b == pline.GetDepthOfLineInSegment())
				{
					f32 t = randflt(fract(f),1);
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
					p.color = float4(0,0,0,0);
					p.energy = -1;
					return;
				}
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

void FireParticleEmitter::Update(const GameTime &gameTime)
{
	runtime += gameTime.GetDeltaTime();

	if(runtime > ignitionTime)
	{
		burnState = Burning; // static burning
		return;
	}

	u32 depth = this->particleLines.back().GetLineDepth();

	K = (1.0f / ignitionTime) * runtime;
	f32 lerp_anim_level = lerp(0, depth, K);
	//burnLevel = depth - lerp_anim_level;
	burnLevel = depth - lerp_anim_level;

	//std::cout << "Burn level: " << lerp_anim_level << ", " << runtime << std::endl;
};