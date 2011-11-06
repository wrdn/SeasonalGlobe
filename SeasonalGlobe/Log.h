#ifndef __LOG_H__
#define __LOG_H__

#include <vector>
#include "TimeInfo.h"

enum LOG_LEVEL
{
	INFO,
	WARN,
	ERROR
};

// Global logging class (NOT thread safe)
class Log
{
private:
	static bool LOGGING_ENABLED;
	static TIME_FORMAT logTimeFormat;
	static std::vector<const c8*> logMsg;

	// When the count of log messages in logMsg reaches flushBufferCount, the messages will be appended to
	// the file specified by flushLogFilename (appended) and logMsg will be cleared.
	// Set flushBufferCount to 1 to write every message, or set to 0 to disable flushing
	// This may help with memory usage, ensuring there aren't too many messages in the buffer
	static u32 flushBufferCount; 
	static c8* flushLogFilename; // max filename lengths

	Log();
	~Log();
	Log(const Log &);

public:
	// calls printf, automatically adds newline so don't add it to logmsg
	// Also adds message to logMsg vector so it can be later written to file if necessary
	static void log(const LOG_LEVEL logLevel, const c8* logmsg);

	// calls log(const LOG_LEVEL3343,const c8*) with log level set to TINFO
	static void log(const c8* logmsg);

	// Writes logs to filename
	static bool write_logs(const c8* filename, const bool append);
	static bool write_logs(const bool append); // writes logs to flushLogFilename

	static bool write_log_header(const c8* filename, const bool append);
	static bool write_log_header(const bool append);
	// When writing the end of the log, it is ALWAYS appended to the end of the log file (it makes no sense to write the log footer in a new
	// file by itself)
	static bool write_log_footer(const c8* filename);
	static bool write_log_footer();


	static void clear_log_buff();
	static void tryflush();
	static void Cleanup(); // You MUST call this to avoid memory leaks when finished with Log


	static void SetLoggingEnabled(const bool);
	static const bool GetLoggingEnabled();

	static void SetLogTimeFormat(const TIME_FORMAT);
	static const TIME_FORMAT GetLogTimeFormat();

	static void SetFlushBufferCount(const u32);
	static const u32 GetFlushBufferCount();

	static void SetFlushLogFilename(const c8*);
	static const c8* const GetFlushLogFilename();
};


// These global functions only exist to allow for cleaner code. I.e. one can write
// log(...) rather than Log.log(...)
static inline void log(const LOG_LEVEL logLevel, const c8* logmsg)
{
	Log::log(logLevel,logmsg);
}

static inline void log(const c8* logmsg)
{
	Log::log(logmsg);
}

#endif
