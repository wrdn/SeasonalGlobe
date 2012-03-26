#include "World.h"

// This file contains global functions used for seasonal events

void StartTreeGrowth(const World *w) // start tree growing
{
	World *wt = (World*)w;
	wt->GetTree()->SetActive(true);
	wt->GetTree()->InitGrow();
};
void InitiateLeafGrowth(const World *w) // make leaves appear
{
	World *wt = (World*)w; 
	wt->GetLeafParticleEmitter()->SetActive(true);
	wt->GetLeafParticleEmitter()->SetStartColor(Color4f(1,1,1,1));
	wt->GetLeafParticleEmitter()->InitiateParticleFadeIn();
};
void InitiateLeafColorChange(const World *w) // make leaves change color
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->SetEndColor(Color4f(0.8f, 0.35f, 0.35f, 1));
	wt->GetLeafParticleEmitter()->InitiateMainColorChange();
};
void InitiateLeafFall(const World *w) // make leaves fall
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->InitiateParticleFall();
};
void InitiateTreeIgnitionFire(const World *w) // make tree catch fire
{
	FireParticleEmitter *fp = (FireParticleEmitter*)w->GetFireParticleEmitter();
	fp->SetRuntime(0);
	fp->SetBurnLevel(0);
	fp->SetBurningState(Igniting);
	fp->SetActive(true);
};
void InitiateTreeDeath(const World *w) // make tree die
{
	FireParticleEmitter *fp = (FireParticleEmitter*)w->GetFireParticleEmitter();
	fp->InitDeath();
};
void InitiateLeafVanish(const World *w) // makes leaves vanish
{
	World *wt = (World*)w;
	wt->GetLeafParticleEmitter()->InitiateParticleFadeOut();
}
void InitialiseHouseSmoke(const World *w) // start smoke
{
	((World*)w)->ActivateSmokeParticleEffect();
};
void InitialiseSnow(const World *w) // start snow
{
	((World*)w)->ActivateSnow();
};

void PlaySpringSummerSound(const World *w)
{
	World *wr = (World*)w;
	
	// ensure every other sound has been stopped
	wr->GetAppSounds().bellsSound->Stop();
	wr->GetAppSounds().fireSound->Stop();
	wr->GetAppSounds().lightningSound->Stop();

	if(wr->GetAppSounds().springSummerSound->GetSourceState() != AL_PLAYING) // accounts for the sound being started during the loading screen
	{
		wr->GetAppSounds().springSummerSound->Play();
	}
};

void PlayLightningSound(const World *w)
{
	World *wr = (World*)w;
	
	wr->GetAppSounds().lightningSound->Play();
	wr->GetAppSounds().springSummerSound->Stop();
};

void PlayFireSound(const World *w)
{
	World *wr = (World*)w;
	wr->GetAppSounds().fireSound->SetVolume(1.0f);
	wr->GetAppSounds().fireSound->Play();
};

void PlayBellsSound(const World *w)
{
	World *wr = (World*)w;
	
	wr->GetAppSounds().fireSound->SetVolume(0.5f);
	wr->GetAppSounds().lightningSound->Stop();

	wr->GetAppSounds().bellsSound->Play();
};

// start terrain elevation, and texture merge
void InitialiseTerrainElevation(const World *w) { ((World*)w)->ActivateTerrainElevation(Up); }
void InitialiseTerrainMelt(const World *w) { ((World*)w)->ActivateTerrainElevation(Down); }
void InitialiseTerrainTextureMergeToSnow(const World *w) { ((World*)w)->ActiveTerrainTextureMerge(1); }
void InitialiseTerrainTextureMergeToGrass(const World *w) { ((World*)w)->ActiveTerrainTextureMerge(-1); }

// show/hide lightning
void InitLightningAppear(const World *w) { ((World*)w)->SetLightningActive(true); }
void InitLightningVanish(const World *w) { ((World*)w)->SetLightningActive(false); }

// deactivate snow
void DeactivateWorldSnow(const World *w) { ((World*)w)->DeactivateSnow(); };

// start slowing snow
void ActivateSnowSlowing(const World *w) { ((World*)w)->SetSnowSlowing(true); }

// setup world seasons
void World::SetupSeasons()
{
	i32 timePerSeasons = conf.Read("TimePerSeason", 16);

	seasonMan.SetTimePerSeason((f32)timePerSeasons); // 8 seconds per season, 32 seconds per cycle
	seasonMan.SetWorldPointer(this);

	// Spring: Tree grows
	seasonMan.AddEvent(Spring, SeasonalEvent(0.1f, PlaySpringSummerSound));

	seasonMan.AddEvent(Spring, SeasonalEvent(0.1f, StartTreeGrowth)); // tree grows at start of spring

	// Summer: Leaves appear on tree
	seasonMan.AddEvent(Summer, SeasonalEvent(0.0f, InitiateLeafGrowth));

	// Autumn: Leaves change colour and fall from tree (then disappear over time), lightning bolt hits tree and it burns down
	seasonMan.AddEvent(Autumn, SeasonalEvent(0, InitiateLeafColorChange));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.2f, InitiateLeafFall));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.3f, InitiateLeafVanish));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.24f, PlayLightningSound));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.28f, InitLightningAppear));
	seasonMan.AddEvent(Autumn, SeasonalEvent(0.3f, InitLightningVanish));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.3f, InitiateTreeIgnitionFire));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.32f, PlayFireSound));

	seasonMan.AddEvent(Autumn, SeasonalEvent(0.78f, InitiateTreeDeath));

	// Winter
	seasonMan.AddEvent(Winter, SeasonalEvent(0, InitialiseSnow ));
	seasonMan.AddEvent(Winter, SeasonalEvent(0.05f, InitialiseHouseSmoke));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.275f, PlayBellsSound));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.3f, InitialiseTerrainTextureMergeToSnow));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.45f, InitialiseTerrainElevation));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.8f, InitialiseTerrainMelt));
	seasonMan.AddEvent(Winter, SeasonalEvent(0.85f, InitialiseTerrainTextureMergeToGrass));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.8f, ActivateSnowSlowing));

	seasonMan.AddEvent(Winter, SeasonalEvent(0.9f, DeactivateWorldSnow));

	UpdateSceneTimings();
};

// update scene timings
void World::UpdateSceneTimings()
{
	tree->SetBuildTime(seasonMan.GetTimePerSeason() * 0.85f);
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToFadeIn(seasonMan.GetTimePerSeason() * 0.25f);
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToChangeColor(seasonMan.GetTimePerSeason() * 0.15f);
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToFall(seasonMan.GetTimePerSeason() * 0.1f);
	particleSystem.GetEmitter<StaticParticleEmitter>(leafParticleEmitterID)->SetTimeToChangeColor(seasonMan.GetTimePerSeason() * 0.2f);
	fireParticleEmitter->SetIgnitionTime(seasonMan.GetTimePerSeason() * 0.45f);
	fireParticleEmitter->SetDeathTime(seasonMan.GetTimePerSeason() * 0.75f);
	particleSystem.GetEmitter<HemiSphericalParticleEmitter>(snowEmitterID)->SetTimeToFall(seasonMan.GetTimePerSeason() * 0.1f);
	timeToMergeTextures = seasonMan.GetTimePerSeason() * 0.25f;
	terrainElevation.timeToElevateFully =  seasonMan.GetTimePerSeason() * 0.25f;
};