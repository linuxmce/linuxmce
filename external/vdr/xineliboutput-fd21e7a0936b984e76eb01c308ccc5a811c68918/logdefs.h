/*
 * logdefs.h: Logging and debug output
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: logdefs.h,v 1.16 2011-11-01 12:26:36 phintuka Exp $
 *
 */

#ifndef _LOGDEFS_H_
#define _LOGDEFS_H_


#if !defined(__GNUC__) || __GNUC__ < 3
#  define LIKELY(x)   (x)
#  define UNLIKELY(x) (x)
#else
#  define LIKELY(x)   __builtin_expect((x),1)
#  define UNLIKELY(x) __builtin_expect((x),0)
#endif

/*
 * Default module name (goes to every log line)
 */

#ifndef LOG_MODULENAME
#  define LOG_MODULENAME "[xine..put] "
#endif

/*
 * Logging functions, should not be used directly
 */

#include <syslog.h> /* logging levels: LOG_ERR, LOG_INFO, LOG_DEBUG */

#define SYSLOGLEVEL_NONE    0
#define SYSLOGLEVEL_ERRORS  1
#define SYSLOGLEVEL_INFO    2
#define SYSLOGLEVEL_DEBUG   3
#define SYSLOGLEVEL_VERBOSE 4

#if defined(esyslog)
#  define x_syslog(l,m,x...) syslog_with_tid(l, m x)
#else

#  ifdef __cplusplus
  extern "C" {
#  endif

  /* from xine_frontend.c or vdr/tools.c: */
  extern int SysLogLevel; /* errors, info, debug */

  /* from logdefs.c: */
  extern int LogToSysLog;

  void x_syslog(int level, const char *module, const char *fmt, ...)
       __attribute__((format (printf, 3, 4)))
       __attribute__((visibility("default")));

#  ifdef __cplusplus
  } /* extern "C" { */
#  endif

#endif /* VDR */

#ifdef NEED_x_syslog
# error NEED_x_syslog is deprecated
#endif


/*
 * Macros used for logging
 */

#include <errno.h>

#define LOG_ERRNO                                                     \
        x_syslog(LOG_ERR, LOG_MODULENAME, "   (ERROR (%s,%d): %s)",   \
                 __FILE__, __LINE__, strerror(errno))

#define LOGERR(x...)                                \
        do {                                        \
          if (LIKELY(SysLogLevel >= SYSLOGLEVEL_ERRORS)) {      \
            x_syslog(LOG_ERR, LOG_MODULENAME, x);   \
            if (errno)                              \
              LOG_ERRNO;                            \
          }                                         \
        } while(0)
#define LOGMSG(x...)                                \
        do {                                        \
          if (LIKELY(SysLogLevel >= SYSLOGLEVEL_INFO))  \
            x_syslog(LOG_INFO, LOG_MODULENAME, x);  \
        } while(0)
#define LOGDBG(x...)                                \
        do {                                        \
          if (UNLIKELY(SysLogLevel >= SYSLOGLEVEL_DEBUG))       \
            x_syslog(LOG_DEBUG, LOG_MODULENAME, x); \
        } while(0)
#define LOGVERBOSE(x...)                            \
        do {                                        \
          if (UNLIKELY(SysLogLevel >= SYSLOGLEVEL_VERBOSE))     \
            x_syslog(LOG_DEBUG, LOG_MODULENAME, x); \
        } while(0)

#define TRACELINE LOGDBG("at %s:%d %s", __FILE__, __LINE__, __FUNCTION__)



/*
 * ASSERT
 */

#ifdef NDEBUG
#  define ASSERT(expr)
#else
#  define ASSERT(expr,fatal) \
      do { \
        if(UNLIKELY(!(expr))) {                         \
          LOGERR("Asseretion failed: %s at %s:%d (%s)", \
                 #expr, __FILE__, __LINE__, __FUNCTION__); \
          if(fatal) \
            abort(); \
        } \
      } while(0)
#endif

#define ASSERT_RET(expr,ret) \
      do { \
        if(UNLIKELY(!(expr))) {                         \
          LOGMSG("Asseretion failed: %s at %s:%d (%s)", \
                 #expr, __FILE__, __LINE__, __FUNCTION__); \
          ret; \
        } \
      } while(0)


/*
 * Plugin (call)trace
 */

#ifdef XINELIBOUTPUT_DEBUG
# ifdef __cplusplus
#
#  include <fstream>
#  include <iostream>
#  include <stdio.h>
#
#  ifndef TRACE_IDENT
#    define TRACE_IDENT ""
#  endif
#  if defined(XINELIBOUTPUT_DEBUG_STDOUT)
#    define TRACE(x) do {std::cout << TRACE_IDENT << x << "\n"; fflush(stdout);}while(0)
#  elif defined(XINELIBOUTPUT_DEBUG_STDERR)
#    define TRACE(x) do {std::cerr << TRACE_IDENT << x << "\n"; fflush(stderr);}while(0)
#  else
#    error No trace target !
#  endif
#  define TRACEF(x) cTraceFunctionCall _x_cTraceFunctionCall(x);
   class cTraceFunctionCall {
     public:
       const char *m_name;
       cTraceFunctionCall(const char *name) : m_name(name)
         { TRACE(m_name << " - Enter"); }
       ~cTraceFunctionCall()
         { TRACE(m_name << " - Leave "); }
   };
# endif
#else
#  define TRACE(x)
#  define TRACEF(x)
#endif


/*
 * Execution time tracker: 
 * log a message when function execution takes longer than expected
 */

#ifdef __cplusplus
#  ifdef TRACK_EXEC_TIME
     class cTimeTracker 
     {
       private:
         const char    *m_Message;
	 const char    *m_Where;
	 uint64_t m_Start;
	 uint64_t m_Trigger;
       public:
	 cTimeTracker(const char *Message, int TriggerMs, const char *Where) {
	   m_Message = Message;
	   m_Where   = Where;
	   m_Trigger = TriggerMs;
	   m_Start   = cTimeMs::Now();      
	 }
	 ~cTimeTracker() {
	   if(cTimeMs::Now() - m_Start > m_Trigger)
	     LOGMSG("********** TimeTracker hit in %s: %d ms %s", 
		    m_Where, (int)(cTimeMs::Now() - m_Start),
		    m_Message?m_Message:"");
	 }
     };
#    define TRACK_TIME(limit) cTimeTracker _timetracker(NULL,limit,__PRETTY_FUNCTION__)
#    define TRACK_TIME_EXT(limit,msg) cTrimeTracker __timetracker(msg,limit,__PRETTY_FUNCTION__)
#  else
#    define TRACK_TIME(limit)
#    define TRACK_TIME_EXT(limit,msg)
#  endif
# endif


#endif /* _LOGDEFS_H_ */
