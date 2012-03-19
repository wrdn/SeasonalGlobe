#include "SeasonManager.h"
#include "World.h"

// returns season at specific time
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

// execute functions in _currentSeason before seasonTimeClamp
void SeasonManager::ExecuteSeasonFunctions(const f32 seasonTimeClamp, Season _currentSeason)
{
	for(std::vector<SeasonalEvent>::iterator it = seasonEvents[_currentSeason].begin();
		it != seasonEvents[currentSeason].end(); ++it)
	{
		if(it->GetTimeOffset() <= seasonTimeClamp)
			it->Run(worldPtr);
	}
};

// add seasonal event at season s
void SeasonManager::AddEvent(const Season s, const SeasonalEvent &s_event)
{
	seasonEvents[s].push_back(s_event);
};

// updates runtime and executes seasonal events if they have been reached
void SeasonManager::Update(const f32 dt)
{
	runtime += dt; // updated runtime

	if(runtime >= totalTime-EPSILON) // reset world and seasons when runtime>totalTime
	{
		worldPtr->ResetWorld();
		currentSeason = Spring;
		runtime = 0;
		return;
	}

	// runtime can keep increasing, but time will always be limited to 0...totalTime
	//runtime = fmod(runtime, totalTime-EPSILON);

	// get current season at runtime
	currentSeason = GetSeason(runtime);

	// convert time to seasonal time in 0 to 1 range and execute season functions
	const f32 seasonTimeClamp = ConvertTimeToSeasonTime(runtime, currentSeason);
	ExecuteSeasonFunctions(seasonTimeClamp, currentSeason);
};

void SeasonManager::Reset()
{
	for(u32 i=0;i<4;++i)
	{
		for(u32 j=0;j<seasonEvents[i].size();++j)
		{
			seasonEvents[i][j].SetHasTriggered(false);
		}
	}
	currentSeason = Spring;
	runtime = 0;
};

// use whichever is easiest to set season times. Time per season = totalTime/4, total time = time per season * 4
void SeasonManager::SetTotalTime(const f32 f)
{
	totalTime = f;
	timePerSeason = totalTime/4;
};

// set time per season
void SeasonManager::SetTimePerSeason(const f32 f)
{
	timePerSeason = max(4,f);
	totalTime = timePerSeason*4;
};