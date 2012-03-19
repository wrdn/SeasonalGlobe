#pragma once

#include "ctypes.h"
#include "util.h"
#include <vector>

enum Season { Spring=0, Summer=1, Autumn=2, Winter=3 };

class World;

// A seasonal event is triggered at a specified time (given in range 0 to 1 into the season time)
// The callback function has the form void Callback(const World *w)
// Events are used in the application to trigger things such as snow starting to fall
class SeasonalEvent
{
private:
	bool hasTriggered; // dont trigger the event twice in this year
	f32 timeOffset; // offset into timePerSeason (see SeasonManager) in range 0 to 1. If value outside range, event callback never executed
	void (*EventCallback)(const World *w);

public:
	
	SeasonalEvent() : hasTriggered(false), timeOffset(0), EventCallback(0) { };
	explicit SeasonalEvent(const f32 _timeOffset) : hasTriggered(false), timeOffset(_timeOffset),
		EventCallback(0) { };
	SeasonalEvent(const f32 _timeOffset, void (*callback)(const World *w)) : hasTriggered(false), timeOffset(_timeOffset),
		EventCallback(callback) { };

	~SeasonalEvent() { };

	// runs the execution function and modifies the hasTriggered bool
	void Run(const World *w)
	{
		if(EventCallback && !hasTriggered)
		{
			EventCallback(w);
			hasTriggered = true;
		}
	};

	// Accessors and Mutators
	const bool HasTriggered() const { return hasTriggered; };
	const f32 GetTimeOffset() const { return timeOffset; }

	void SetHasTriggered(const bool b) { hasTriggered = b; }
	void SetTimeOffset(const f32 f) { timeOffset = f; }
};

// Manages the seasons you pass through (Spring, Summer, Autumn and Winter)
// and the events associated with each season
class SeasonManager
{
private:
	f32 totalTime; // for all seasons
	f32 timePerSeason; // cached, equals totalTime/4, every season lasts the same amount of time
	Season currentSeason; // cached, lerp between seasons over time
	f32 runtime; // time we are currently at (into the seasons)
	World *worldPtr; // pointer to the world (so the Update function can pass it to the seasonal event)

	std::vector<SeasonalEvent> seasonEvents[4]; // 0=spring, 1=summer, 2=autumn, 3=winter

	// returns season at specified time
	const Season GetSeason(const f32 time) const;

	// converts time into range 0 to 1 for current season
	const f32 ConvertTimeToSeasonTime(f32 time, Season _currentSeason) const;

	// executes functions for this season at the current time (seasonTimeClamp)
	void ExecuteSeasonFunctions(const f32 seasonTimeClamp, Season _currentSeason);

public:

	SeasonManager() : totalTime(16), timePerSeason(totalTime/4),
		currentSeason(Spring), runtime(0), worldPtr(0) { };
	~SeasonManager() { };

	void SetWorldPointer(World *p) { worldPtr = p; }

	// Use whichever you want to set season times. Time per season = totalTime/4, total time = time per season * 4
	void SetTotalTime(const f32 f);
	void SetTimePerSeason(const f32 f);

	const f32 GetTimePerSeason() const { return timePerSeason; }
	const f32 GetTotalTime() const { return totalTime; }

	Season GetCurrentSeason() const { return currentSeason; }

	// add event to season s
	void AddEvent(const Season s, const SeasonalEvent &s_event);

	// updates runtime and executes seasonal events if they have been reached
	void Update(const f32 dt);

	// resets seasonal events
	void Reset();
};