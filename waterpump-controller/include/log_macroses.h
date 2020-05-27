
#define LOG_ENABLED
#define LOG_DEBUG_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_WARN_ENABLED 1
#define LOG_ERROR_ENABLED 1

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
#define LOG_DEBUGLN(msg) LOGSTREAM.println(msg)
#define LOG_DEBUGF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_INFOLN(msg) LOGSTREAM.println(msg)
#define LOG_INFOF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
// #define LOG_INFOLN(msg) 
// #define LOG_INFOF(fmt, ...) 
#define LOG_WARNLN(msg) LOGSTREAM.println(msg)
#define LOG_WARNF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_ERRORLN(msg) LOGSTREAM.println(msg)
#define LOG_ERRORF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
