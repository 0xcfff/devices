#ifndef _TNLOG_H_
#define _TNLOG_H_

#define LOG_ENABLED
#define LOG_VERB_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_WARN_ENABLED 1
#define LOG_ERROR_ENABLED 1
#define LOG_FATAL_ENABLED 1

// TODO: add min_log_level, make it affect the actual list of LOG_*_ENABLED defibned above
// TODO: replace each LOG_*_ENABLED with two items, LOG_*_ENABLED and LOG_*_ENABLED_FLAG, one of the two should not be defined if the logging for specified level is not enabled

#define LOG_DDBG_TXT(name, value) const char * name = value
#define LOG_DDBG_FTXT(name, value) const PROGMEM char name[] = value
#define LOG_DINF_TXT(name, value) const char * name = value
#define LOG_DING_FTXT(name, value) const PROGMEM char name[] = value
// #define LOG_DINF_TXT(name, value) 
// #define LOG_DING_FTXT(name, value) 
#define LOG_DWRN_TXT(name, value) const char * name = value
#define LOG_DWRN_FTXT(name, value) const PROGMEM char name[] = value
#define LOG_DERR_TXT(name, value) const char * name = value
#define LOG_DERR_FTXT(name, value) const PROGMEM char name[] = value

#define LOGSTREAM Serial
#define LOG_VERBLN(msg) LOGSTREAM.println(msg)
#define LOG_VERBF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_DEBUGLN(msg) LOGSTREAM.println(msg)
#define LOG_DEBUGF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_INFOLN(msg) LOGSTREAM.println(msg)
#define LOG_INFOF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_WARNLN(msg) LOGSTREAM.println(msg)
#define LOG_WARNF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_ERRORLN(msg) LOGSTREAM.println(msg)
#define LOG_ERRORF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_FATALLN(msg) LOGSTREAM.println(msg)
#define LOG_FATALF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)

#endif