#ifndef _MACRO_LOG_STREAM_H_
#define _MACRO_LOG_STREAM_H_

#ifndef LOGSTREAM
    #define LOGSTREAM Serial
#endif

#undef LOGBACKEND
#define LOGBACKEND "Stream"

#define __LOG_WRITE_ENTRY_LN(lvl, msg) LOGSTREAM.printf("%-6s %s\n", lvl, msg) 
#define __LOG_WRITE_ENTRY_F(lvl, fmt, ...) LOGSTREAM.printf("%-6s ", lvl);LOGSTREAM.printf(fmt, __VA_ARGS__) 

#endif  // _MACRO-LOG-STREAM_H_
