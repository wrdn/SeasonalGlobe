#include "TimeInfo.h"
#include <string.h>

TimeInfo::TimeInfo(void)
{
	time(&raw_time);
};

TimeInfo::~TimeInfo(void)
{ };

const time_t TimeInfo::get_raw_time()
{
	time(&raw_time);
	return raw_time;
};

const tm *TimeInfo::get_time_info(fp p, const time_t *rawtime) const
{
	tm *timeinfo = p(rawtime);
	return timeinfo;
};

const tm *TimeInfo::get_utc_time(const time_t *rawtime) const
{
	fp p = &gmtime;
	return get_time_info(p,rawtime);
};

const tm * TimeInfo::get_utc_time() const
{
	fp p = &gmtime;
	return get_time_info(p,&raw_time);
}

const tm *TimeInfo::get_local_time(const time_t *rawtime) const
{
	fp p = &localtime;
	return get_time_info(p,rawtime);
};

const tm * TimeInfo::get_local_time() const
{
	fp p = &localtime;
	return get_time_info(p, &raw_time);
}

c8* TimeInfo::tostr(const tm *t) const
{
	c8 *c = asctime(t);
	c[strlen(c)-1] = '\0'; // get rid of the newline character asctime puts in
	return c;
};