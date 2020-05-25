
#define LOGSTREAM Serial
#define LOG_DEBUGLN(msg) LOGSTREAM.println(msg)
#define LOG_DEBUGF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_INFOLN(msg) LOGSTREAM.println(msg)
#define LOG_INFOF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
#define LOG_ERRORLN(msg) LOGSTREAM.println(msg)
#define LOG_ERRORF(fmt, ...) LOGSTREAM.printf(fmt, __VA_ARGS__)
