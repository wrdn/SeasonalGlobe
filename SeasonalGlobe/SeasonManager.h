#pragma once

#include "ctypes.h"
#include "util.h"

enum Season { Spring=0, Summer=1, Autumn=2, Winter=3 };

class SeasonalEvent
{
private:
	bool hasTriggered;
	f32 timeOffset;
	void (*EventCallback)();
public:
	
	SeasonalEvent() : hasTriggered(false), timeOffset(0), EventCallback(0) { };
	SeasonalEvent(const f32 _timeOffset) : hasTriggered(false), timeOffset(_timeOffset),
		EventCallback(0) { };
	SeasonalEvent(const f32 _timeOffset, void (*callback)()) : hasTriggered(false), timeOffset(_timeOffset),
		EventCallback(callback) { };

	~SeasonalEvent();

	// runs the execution function and modifies the hasTriggered bool
	void Run() { if(EventCallback && !hasTriggered) { EventCallback(); hasTriggered = true; } };

	const bool HasTriggered() const { return hasTriggered; };
	const f32 GetTimeOffset() const { return timeOffset; }

	void SetHasTriggered(const bool b) { hasTriggered = b; }
	void SetTimeOffset(const f32 f) { timeOffset = f; }
};

class SeasonManager
{
private:
	f32 totalTime; // for all seasons
	f32 timePerSeason; // cached, equals totalTime/4
	Season currentSeason; // cached, lerp between seasons over time
	f32 runtime;

	std::vector<SeasonalEvent> seasonEvents[4]; // 0=spring, 1=summer, 2=autumn, 3=winter

	Season GetSeason(const f32 time)
	{
		return (Season)(u32)lerp(0, 4, (1.0f / totalTime) * time);
	};

	const f32 ConvertTimeToSeasonTime(f32 time, Season _currentSeason)
	{
		// converts time into range 0 to 1 for the current season
		// with this number we can then check each of the current seasons
		// seasonal events
		return (1.0f / timePerSeason) * (time - (_currentSeason * timePerSeason));
	};

	void ExecuteSeasonFunctions(const f32 seasonTimeClamp, Season _currentSeason)
	{
		for(std::vector<SeasonalEvent>::iterator it = seasonEvents[_currentSeason].begin();
			it != seasonEvents[currentSeason].end(); ++it)
		{
			if(it->GetTimeOffset() <= seasonTimeClamp)
				it->Run();
		}
	};

public:

	SeasonManager() : totalTime(16), timePerSeason(totalTime/4),
		currentSeason(Spring), runtime(0) { };
	~SeasonManager() { };

	void SetTotalTime(const f32 f) { totalTime = f; timePerSeason = totalTime/4; };
	void SetTimePerSeason(const f32 f) { timePerSeason = f; totalTime = timePerSeason*4; };

	Season GetCurrentSeason() const { return currentSeason; }

	void AddEvent(const Season s, const SeasonalEvent &s_event)
	{
		seasonEvents[s].push_back(s_event);
	};

	// updates runtime and executes seasonal events if they have been reached
	void Update(const f32 dt)
	{
		runtime += dt;

		// runtime can keep increasing, but time will always be limited to 0...totalTime
		runtime = fmod(runtime, totalTime);

		currentSeason = GetSeason(runtime);

		const f32 seasonTimeClamp = ConvertTimeToSeasonTime(runtime, currentSeason);
		ExecuteSeasonFunctions(seasonTimeClamp, currentSeason);
	};
};