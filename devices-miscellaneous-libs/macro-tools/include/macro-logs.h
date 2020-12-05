#ifndef _MACRO_LOGS_H_
#define _MACRO_LOGS_H_

// Define Log Levels
#define LOGLEVEL_VERBOSE    0
#define LOGLEVEL_DEBUG      1
#define LOGLEVEL_INFO       2
#define LOGLEVEL_WARN       3
#define LOGLEVEL_ERROR      4
#define LOGLEVEL_FATAL      5

/**
 * MINLOGLEVEL define minimal logging level if not defined yet.
 * This value determines which levels should be logged and which are not.
 * Default: LOGLEVEL_VERBOSE - all levels are logged
 * MINLOGLEVEL value can be overriden by global compiler defines.
 */
#ifndef MINLOGLEVEL
    #define MINLOGLEVEL     LOGLEVEL_VERBOSE
#endif

// If no logging macros defined, determine backend and define the macroses
#ifndef __LOG_MSG_VAR

    // Serial backend
    #define __LOG_BACKEND__serial    1
    #define __LOG_BACKEND__Serial    1
    #define __LOG_BACKEND__SERIAL    1
    #define __LOG_BACKEND__stream    1
    #define __LOG_BACKEND__Stream    1
    #define __LOG_BACKEND__STREAM    1

    // Printf backend
    #define __LOG_BACKEND__printf    2
    #define __LOG_BACKEND__Printf    2
    #define __LOG_BACKEND__PRINTF    2

    // Custom backend
    #define __LOG_BACKEND__custom    3
    #define __LOG_BACKEND__Custom    3
    #define __LOG_BACKEND__CUSTOM    3
    #define __LOG_BACKEND__external  3
    #define __LOG_BACKEND__External  3
    #define __LOG_BACKEND__EXTERNAL  3

    // Backend name to Id converting macroses
    #define __LOG_BACKAND_ID_INTERNAL(backendName) __LOG_BACKEND__##backendName
    #define __LOG_BACKAND_ID(backendName) __LOG_BACKAND_ID_INTERNAL(backendName)


    /**
     * LOGBACKEND defines what logging backend should be used by logging macroses.
     * If the value is not already defined, then the below block determines 
     * default logging backend based on the context (platform, framework, microprocessor, etc).
     * 
     * LOGBACKEND value can be overriden by global compiler defines.
     * Available values are:
     * * Serial
     * * printf
     * * Custom
     */
    #ifndef LOGBACKEND
        #if defined(ARDUINO_ARCH_STM32)
            #define LOGBACKEND printf
        #elif defined(ARDUINO_ARCH_ESP8266)
            #define LOGBACKEND Serial
        #else
            #define LOGBACKEND printf
        #endif
    #endif

    /**
     * Define logging backend macroses for selected LOGBACKEND if these macroses are not defined yet.
     * 
     * There are following required macroses:
     * * __LOG_WRITE_ENTRY_LN(lvl_id, lvl_name, msg) - writes a line to the target log backend
     * * __LOG_WRITE_ENTRY_F(lvl_id, lvl_name, fmt, ...) - writes a formatted string to the target log backend
     */
    #if __LOG_BACKAND_ID(LOGBACKEND) == __LOG_BACKEND__STREAM
        
        // Use default Serial printf as output
        #ifndef LOGSTREAM
            #define LOGSTREAM Serial
        #endif

        #define __LOG_WRITE_ENTRY_LN(lvl_id, lvl_name, msg) LOGSTREAM.printf("%-6s %s\n", lvl_name, msg) 
        #define __LOG_WRITE_ENTRY_F(lvl_id, lvl_name, fmt, ...) LOGSTREAM.printf("%-6s ", lvl_name);LOGSTREAM.printf(fmt, __VA_ARGS__) 
    #elif __LOG_BACKAND_ID(LOGBACKEND) == __LOG_BACKEND__PRINTF

        // Use generic printf as output
        #define __LOG_WRITE_ENTRY_LN(lvl_id, lvl_name, msg) printf("%-6s %s\n", lvl_name, msg) 
        #define __LOG_WRITE_ENTRY_F(lvl_id, lvl_name, fmt, ...) printf("%-6s ", lvl_name);printf(fmt, __VA_ARGS__) 
    #elif __LOG_BACKAND_ID(LOGBACKEND) == __LOG_BACKEND__CUSTOM
        
        // Use custom external print functions as output
        extern int (*log_custom_println)(int, const char *, const char *)
        extern int (*log_custom_printf)(int, const char *, const char *, ...)
        
        #define __LOG_WRITE_ENTRY_LN(lvl_id, lvl_name, msg) log_custom_println(lvl_id, lvl_name, msg)
        #define __LOG_WRITE_ENTRY_F(lvl_id, lvl_name, fmt, ...) log_custom_printf(lvl_id, lvl_name, fmt, __VA_ARGS__) 
    #endif
#endif


/**
 * Calculate if logs are enabled (LOGLEVEL_VERBOSE <= MINLOGLEVEL <= LOGLEVEL_FATAL).
 * If logs are enabled, define LOGENABLED marker macros. This macros can be used in code
 * together with LOGENABLED_xxx macroses to conditionally include/exclude code blocks 
 * into/from compilation process.
*/
#if MINLOGLEVEL >= LOGLEVEL_VERBOSE && MINLOGLEVEL <= LOGLEVEL_FATAL
    #define LOGENABLED
    #define __LOG_MSG_VAR(name, value) const char * name = value 
    #define __LOG_PMSG_VAR(name, value) const PROGMEM char name[] = value
#endif

/**
 * Following blocks define following set of macroses for each logging level:
 * * DEF_LOG_{LOGLEVEL}_MSG(name, value) - defines a text variable to use later in logging 
 *   with log level specified in the macros name or above.
 * * DEF_LOG_{LOGLEVEL}_PMSG(name, value) - defines a text variable with data stored in PROGMEM 
 *   to use later in logging with log level specified in the macros name or above.
 * * LOG_{LOGLEVEL}LN(msg) - defines a macros that logs passed message to configured backend 
 *   if corresponding log level is enabled.
 * * LOG_VERBOSEF(fmt, ...) - defines a macros that formats passed fmt string and arguments 
 *   and logs result to configured backend if corresponding log level is enabled.
*/

// Verbose
#if defined(LOGENABLED) && MINLOGLEVEL <= LOGLEVEL_VERBOSE
    #define LOGENABLED_VERBOSE
    #define DEF_LOG_VERBOSE_MSG(name, value) __LOG_MSG_VAR(name, value)
    #define DEF_LOG_VERBOSE_PMSG(name, value) __LOG_PMSG_VAR(name, value)
    #define LOG_VERBOSELN(msg) __LOG_WRITE_ENTRY_LN(LOGLEVEL_VERBOSE, "VERB", msg)
    #define LOG_VERBOSEF(fmt, ...) __LOG_WRITE_ENTRY_F(LOGLEVEL_VERBOSE, "VERB", fmt, __VA_ARGS__)
#else
    #define DEF_LOG_VERBOSE_MSG(name, value)
    #define DEF_LOG_VERBOSE_PMSG(name, value)
    #define LOG_VERBOSELN(msg)
    #define LOG_VERBOSEF(fmt, ...)
#endif

// Debug
#if defined(LOGENABLED) && MINLOGLEVEL <= LOGLEVEL_DEBUG
    #define LOGENABLED_DEBUG
    #define DEF_LOG_DEBUG_MSG(name, value) __LOG_MSG_VAR(name, value)
    #define DEF_LOG_DEBUG_PMSG(name, value) __LOG_PMSG_VAR(name, value)
    #define LOG_DEBUGLN(msg) __LOG_WRITE_ENTRY_LN(LOGLEVEL_DEBUG, "DEBUG", msg)
    #define LOG_DEBUGF(fmt, ...) __LOG_WRITE_ENTRY_F(LOGLEVEL_DEBUG, "DEBUG", fmt, __VA_ARGS__)
#else
    #define DEF_LOG_DEBUG_MSG(name, value)
    #define DEF_LOG_DEBUG_PMSG(name, value)
    #define LOG_DEBUGLN(msg)
    #define LOG_DEBUGF(fmt, ...)
#endif


// Info
#if defined(LOGENABLED) && MINLOGLEVEL <= LOGLEVEL_INFO
    #define LOGENABLED_INFO
    #define DEF_LOG_INFO_MSG(name, value) __LOG_MSG_VAR(name, value)
    #define DEF_LOG_INFO_PMSG(name, value) __LOG_PMSG_VAR(name, value)
    
    /**
     * Macros sends specified message text to  logging subsystem to log it 
     * with INFO level. The message is logged only if INFO level logging is enabled, 
     * see MINLOGLEVEL for details.
     * 
     * @param  {char*}  msg              : Text nessage to log with INFO level
     * @return {int}                     : Number of chars logged
     */
    #define LOG_INFOLN(msg) __LOG_WRITE_ENTRY_LN(LOGLEVEL_INFO, "INFO", msg)
    /**
     * Macros formats specified fmt string with arguments and then sends resuolt text 
     * to  logging subsystem to log it with INFO level. The text is logged 
     * only if INFO level logging is enabled, see MINLOGLEVEL for details.
     * 
     * @param  {const char*} <fmt>        : Format string
     * @param  {...} __VA_ARGS__          : Format agruments
     * @return {int}                      : Number of chars logged
     */
    #define LOG_INFOF(fmt, ...) __LOG_WRITE_ENTRY_F(LOGLEVEL_INFO, "INFO", fmt, __VA_ARGS__)
#else
    #define DEF_LOG_INFO_MSG(name, value)
    #define DEF_LOG_INFO_PMSG(name, value)
    #define LOG_INFOLN(msg)
    #define LOG_INFOF(fmt, ...)
#endif

// Warn
#if defined(LOGENABLED) && MINLOGLEVEL <= LOGLEVEL_WARN
    #define LOGENABLED_WARN
    #define DEF_LOG_WARN_MSG(name, value) __LOG_MSG_VAR(name, value)
    #define DEF_LOG_WARN_PMSG(name, value) __LOG_PMSG_VAR(name, value)
    #define LOG_WARNLN(msg) __LOG_WRITE_ENTRY_LN(LOGLEVEL_WARN, "WARN", msg)
    #define LOG_WARNF(fmt, ...) __LOG_WRITE_ENTRY_F(LOGLEVEL_WARN, "WARN", fmt, __VA_ARGS__)
#else
    #define DEF_LOG_WAR_MSG(name, value)
    #define DEF_LOG_WARN_PMSG(name, value)
    #define LOG_WARNLN(msg)
    #define LOG_WARNF(fmt, ...)
#endif

 // Error
#if defined(LOGENABLED) && MINLOGLEVEL <= LOGLEVEL_ERROR
    #define LOGENABLED_ERROR
    #define DEF_LOG_ERROR_MSG(name, value) __LOG_MSG_VAR(name, value)
    #define DEF_LOG_ERROR_PMSG(name, value) __LOG_PMSG_VAR(name, value)
    #define LOG_ERRORLN(msg) __LOG_WRITE_ENTRY_LN(LOGLEVEL_ERROR, "ERROR", msg)
    #define LOG_ERRORF(fmt, ...) __LOG_WRITE_ENTRY_F(LOGLEVEL_ERROR, "ERROR", fmt, __VA_ARGS__)
#else
    #define DEF_LOG_ERROR_MSG(name, value)
    #define DEF_LOG_ERROR_PMSG(name, value)
    #define LOG_ERRORLN(msg)
    #define LOG_ERRORF(fmt, ...)
#endif

 // Fatal
#if defined(LOGENABLED) && MINLOGLEVEL <= LOGLEVEL_FATAL
    #define LOGENABLED_FATAL
    #define DEF_LOG_FATAL_MSG(name, value) __LOG_MSG_VAR(name, value)
    #define DEF_LOG_FATAL_PMSG(name, value) __LOG_PMSG_VAR(name, value)
    #define LOG_FATALLN(msg) __LOG_WRITE_ENTRY_LN(LOGLEVEL_FATAL, "FATAL", msg)
    #define LOG_FATALF(fmt, ...) __LOG_WRITE_ENTRY_F(LOGLEVEL_FATAL, "FATAL", fmt, __VA_ARGS__)
#else
    #define DEF_LOG_FATAL_MSG(name, value)
    #define DEF_LOG_FATAL_PMSG(name, value)
    #define LOG_FATALLN(msg)
    #define LOG_FATALF(fmt, ...)
#endif

#endif  // _MACRO_LOGS_H_