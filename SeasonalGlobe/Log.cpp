#include "Log.h"
#include "strutils.h"

#include <fstream>
#include <iostream>

bool Log::LOGGING_ENABLED = true;
TIME_FORMAT Log::logTimeFormat = LOCAL; // ignored currently
u32 Log::flushBufferCount = 15;
c8* Log::flushLogFilename = 0;
std::vector<const c8*> Log::logMsg;

void Log::log( const LOG_LEVEL logLevel, const c8* logmsg )
{
	if(!LOGGING_ENABLED) return;

	std::string st = 
		logLevel == INFO ? "[INFO] " :
		(logLevel == WARN ? "[WARN] " : "[ERROR] ");
	st += logmsg;

	std::cout << st << std::endl;
	logMsg.push_back(st.c_str());

	tryflush();
}

void Log::log( const c8* logmsg )
{
	log(INFO, logmsg);
}

void Log::clear_log_buff()
{
	for(u32 i=0;i<logMsg.size();++i)
		SAFE_DELETE_ARRAY(&logMsg[i]);

	logMsg.clear();
}

bool Log::write_logs( const c8* filename, const bool append )
{
	if(!logMsg.size())
		return false;

	std::ofstream fl;
	fl.open(filename, append ? std::ios::out | std::ios::app : std::ios::out);
	if(fl.is_open())
	{
		for(u32 i=0;i<logMsg.size();++i)
		{
			if(logMsg.at(i))
			{
				fl << logMsg[i];
			}
		}
	}
	else { return false; }

	fl.close();

	return true;
}

bool Log::write_logs( const bool append )
{
	if(flushLogFilename)
		return write_logs(flushLogFilename, append);

	return false;
}

void Log::tryflush()
{
	if(!flushBufferCount || !flushLogFilename || logMsg.size() < flushBufferCount) return;
	write_logs(flushLogFilename, true);
	clear_log_buff();
}


Log::Log()
{

}

Log::Log( const Log & )
{

}

Log::~Log()
{
}

void Log::Cleanup()
{
	SAFE_DELETE_ARRAY(flushLogFilename);
	clear_log_buff();
}

void Log::SetLoggingEnabled( const bool b )
{
	LOGGING_ENABLED = b;
}

const bool Log::GetLoggingEnabled()
{
	return LOGGING_ENABLED;
}

void Log::SetLogTimeFormat( const TIME_FORMAT t )
{
	logTimeFormat = t;
}

const TIME_FORMAT Log::GetLogTimeFormat()
{
	return logTimeFormat;
}

void Log::SetFlushBufferCount( const u32 c )
{
	flushBufferCount = c;
}

const u32 Log::GetFlushBufferCount()
{
	return flushBufferCount;
}

void Log::SetFlushLogFilename( const c8* f )
{
	SAFE_DELETE_ARRAY(flushLogFilename);
	flushLogFilename = copystr(f);
}

const c8* const Log::GetFlushLogFilename()
{
	return flushLogFilename;
}

bool Log::write_log_header( const c8* filename, const bool append )
{
	if(!flushLogFilename) return false;

	std::ofstream fl;
	fl.open(filename, append ? std::ios::out | std::ios::app : std::ios::out);
	if(fl.is_open())
	{
		TimeInfo tinfo;
		const time_t t = tinfo.get_raw_time();
		char *dt = tinfo.tostr(tinfo.get_local_time(&t));
		fl << "***************  LOGGING STARTED  " << dt << "  ***************\n"; // write header on log
	}
	else { return false; }

	return true;
}

bool Log::write_log_header( const bool append )
{
	return write_log_header(flushLogFilename, append);
}

bool Log::write_log_footer( const c8* filename )
{
	if(!filename) return false;

	std::ofstream fl;
	fl.open(filename, std::ios::out | std::ios::app);
	if(fl.is_open())
	{
		TimeInfo tinfo;
		const time_t t = tinfo.get_raw_time();
		char *dt = tinfo.tostr(tinfo.get_local_time(&t));
		fl << "***************  LOGGING ENDED  " << dt << "  ***************\n\n"; // write header on log
	}
	else { return false; }

	return true;
}

bool Log::write_log_footer()
{
	return write_log_footer(flushLogFilename);
}