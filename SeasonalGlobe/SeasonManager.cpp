#include "SeasonManager.h"

const Season SeasonManager::GetSeason(const f32 time) const
{
	return (Season)(u32)lerp(0, 4, (1.0f / totalTime) * time);
};

const f32 SeasonManager::ConvertTimeToSeasonTime(f32 time, Season _currentSeason) const
{
	// converts time into range 0 to 1 for the current season
	// with this number we can then check each of the current seasons
	// seasonal events
	return (1.0f / timePerSeason) * (time - (_currentSeason * timePerSeason));
};

void SeasonManager::ExecuteSeasonFunctions(const f32 seasonTimeClamp, Season _currentSeason)
{
	for(std::vector<SeasonalEvent>::iterator it = seasonEvents[_currentSeason].begin();
		it != seasonEvents[currentSeason].end(); ++it)
	{
		if(it->GetTimeOffset() <= seasonTimeClamp)
			it->Run(worldPtr);
	}
};

void SeasonManager::AddEvent(const Season s, const SeasonalEvent &s_event)
{
	seasonEvents[s].push_back(s_event);
};

// updates runtime and executes seasonal events if they have been reached
void SeasonManager::Update(const f32 dt)
{
	runtime += dt;

	// runtime can keep increasing, but time will always be limited to 0...totalTime
	runtime = fmod(runtime, totalTime);

	currentSeason = GetSeason(runtime);

	const f32 seasonTimeClamp = ConvertTimeToSeasonTime(runtime, currentSeason);
	ExecuteSeasonFunctions(seasonTimeClamp, currentSeason);
};