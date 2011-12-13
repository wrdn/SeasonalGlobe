#pragma once

#include "ctypes.h"
#include "util.h"

enum Season { Spring=0, Summer=1, Autumn=2, Winter=3 };

class World;
class SeasonalEvent
{
private:
	bool hasTriggered;
	f32 timeOffset;
	void (*EventCallback)(World *w);

public:
	
	SeasonalEvent() : hasTriggered(false), timeOffset(0), EventCallback(0) { };
	SeasonalEvent(const f32 _timeOffset) : hasTriggered(false), timeOffset(_timeOffset),
		EventCallback(0) { };
	SeasonalEvent(const f32 _timeOffset, void (*callback)(World *w)) : hasTriggered(false), timeOffset(_timeOffset),
		EventCallback(callback) { };

	~SeasonalEvent() { };

	// runs the execution function and modifies the hasTriggered bool
	void Run(World *w) { if(EventCallback && !hasTriggered) { EventCallback(w); hasTriggered = true; } };

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
	World *worldPtr;

	std::vector<SeasonalEvent> seasonEvents[4]; // 0=spring, 1=summer, 2=autumn, 3=winter

	Season GetSeason(const f32 time);
	const f32 ConvertTimeToSeasonTime(f32 time, Season _currentSeason);
	void ExecuteSeasonFunctions(const f32 seasonTimeClamp, Season _currentSeason);

public:

	SeasonManager() : totalTime(16), timePerSeason(totalTime/4),
		currentSeason(Spring), runtime(0) { };
	~SeasonManager() { };

	void SetWorldPointer(World *p) { worldPtr = p; }

	// use whichever is easiest to set season times. Time per season = totalTime/4, total time = time per season * 4
	void SetTotalTime(const f32 f) { totalTime = f; timePerSeason = totalTime/4; };
	void SetTimePerSeason(const f32 f) { timePerSeason = f; totalTime = timePerSeason*4; };

	const f32 GetTimePerSeason() const { return timePerSeason; }
	const f32 GetTotalTime() const { return totalTime; }

	Season GetCurrentSeason() const { return currentSeason; }

	void AddEvent(const Season s, const SeasonalEvent &s_event);

	// updates runtime and executes seasonal events if they have been reached
	void Update(const f32 dt);
};