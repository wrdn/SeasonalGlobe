#pragma once

#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define SEC_TO_MICROSEC(x) 1000000*x
#define SEC_TO_MILLISEC(x) 1000*x
#define SEC_TO_NANOSEC(x) 1000000000*x

#define NANOSEC_TO_SEC(x) x/1000000000.0
#define MILLISEC_TO_SEC(x) x/1000.0
#define MICROSEC_TO_SEC(x) x/1000000.0

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

	void start();
	void end();
	double time(); // Time between start and end in seconds
};

// Anyone including PerfTimer.h can easily time there code, without making it look messy using these macros,
// that automatically add the timing around the code segment, and print the results (with optional descriptive NAME)
// In the Release build, any timing is removed
static PerfTimer pt12445465754365f; // complex name to avoid collisions
// Macros to wrap timing code.Simply pass the code, PerfTimer instance and an optional name for the timing segment
// Example: TIME(a=a+b; , pt, "Adding Timing Test");
// Result: Execution time for (Adding Timing Test): 0.0001

#ifdef _DEBUG
#define TIME(CODE, NAME) \
	pt12445465754365f.start(); \
	CODE; \
	pt12445465754365f.end(); \
	printf("Execution time for (%s): %f\n",NAME,pt12445465754365f.time());

#define TIMEF(CODE) \
	pt12445465754365f.start(); \
	CODE; \
	pt12445465754365f.end(); \
	printf("Execution time: %f\n",pt12445465754365f.time());
#else
#define TIME(CODE,NAME) CODE;
#define TIMEF(CODE) CODE;
#endif