#ifndef _MACRO_LOG_STREAM_H_
#define _MACRO_LOG_STREAM_H_

#undef LOGBACKEND
#define LOGBACKEND "printf"

#define __LOG_WRITE_ENTRY_LN(lvl, msg) printf("%-6s %s\n", lvl, msg) 
#define __LOG_WRITE_ENTRY_F(lvl, fmt, ...) printf("%-6s ", lvl);printf(fmt, __VA_ARGS__) 

#endif  // _MACRO-LOG-STREAM_H_
