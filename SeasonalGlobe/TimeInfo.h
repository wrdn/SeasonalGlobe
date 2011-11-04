#ifndef __TIME_INFO_H__
#define __TIME_INFO_H__

#include <time.h>
#include "ctypes.h"

enum TIME_FORMAT
{
	LOCAL,
	UTC,
};

/*
#define SECONDS_IN_X_DAYS(X) ((60*60)*24)*X
#define SECONDS_IN_X_HOURS(X) (60*60)*X
#define SECONDS_IN_X_MINUTES(X) 60*X
*/

// Used so you can specify a custom time function.
// Common functions are localtime(time_t*) and gmtime(time_t*)

#ifdef _WIN32
typedef struct tm *(__cdecl *fp)(const time_t *);
#else
typedef struct tm* (*fp)(const time_t *) __attribute__((cdecl)); // GCC
#endif

class TimeInfo
{
private:
	time_t raw_time;
public:
	TimeInfo(void);
	~TimeInfo(void);

	const time_t get_raw_time();
	const tm *get_time_info(fp p, const time_t *rawtime) const;

	const tm *get_utc_time(const time_t *rawtime) const;
	const tm *get_local_time(const time_t *rawtime) const;
	const tm *get_utc_time() const; // reads raw_time from class
	const tm *get_local_time() const; // reads raw_time from class

	c8* tostr(const tm *t) const;
	void set_raw_time(time_t t) { raw_time=t; };
};

#endif
