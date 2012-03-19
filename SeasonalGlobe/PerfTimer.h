#pragma once

#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Performance timer, used to time code using QueryPerformanceFrequency() and QueryPerformanceCounter()
class PerfTimer
{
private:
#ifdef _WIN32
	LARGE_INTEGER _start,_end,frequency;
#else
	timespec tstart, tend;
#endif
public:
	PerfTimer(void);
	~PerfTimer(void);

	void start(); // call to start timer
	void end(); // call to end timer
	double time() const; // Time between start and end in seconds
};