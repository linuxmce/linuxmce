/*
 * logdefs.h: Logging and debug output
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: logdefs.h,v 1.8 2008/02/05 01:18:17 phintuka Exp $
 *
 */

#ifndef _LOGDEFS_H_

/*
 * Default module name (goes to every log line)
 */

#ifndef LOG_MODULENAME
#  define LOG_MODULENAME "[xine..put] "
#endif

/*
 * Logging functions, should not be used directly
 */

#if defined(esyslog) || (defined(VDRVERSNUM) && VDRVERSNUM >= 10343)
#  define x_syslog(l,x...) syslog_with_tid(l, LOG_MODULENAME x)
#else

#  ifndef __APPLE__
#    include <linux/unistd.h> /* syscall(__NR_gettid) */
#  endif

   /* from xine_frontend.c or vdr tools.c: */
   extern int SysLogLevel; /* errors, info, debug */

#  ifndef LogToSysLog
   extern int LogToSysLog; /* xine_frontend_c, log to syslog instead of console */
#  endif

#  if defined(NEED_x_syslog)
#    include <syslog.h>
#    include <sys/syscall.h>
#    include <stdarg.h>
     static void x_syslog(int level, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
     static void x_syslog(int level, const char *fmt, ...)
     { 
       va_list argp;
       char buf[512];
       va_start(argp, fmt);
       vsnprintf(buf, 512, fmt, argp);
       buf[sizeof(buf)-1] = 0;
#    ifndef __APPLE__
       if(!LogToSysLog) {
	 fprintf(stderr,"[%ld] " LOG_MODULENAME "%s\n", (long int)syscall(__NR_gettid), buf);
       } else {
	 syslog(level, "[%ld] " LOG_MODULENAME "%s", (long int)syscall(__NR_gettid), buf);
       }
#    else
       if(!LogToSysLog) {
	 fprintf(stderr, LOG_MODULENAME "%s\n", buf);
       } else {
	 syslog(level, LOG_MODULENAME "%s", buf);
       }
#    endif
       va_end(argp);
     }
#  endif /* NEED_x_syslog */
#endif /* VDR */


/*
 * Macros used for logging
 */

#include <errno.h>

#define LOG_ERRNO  x_syslog(LOG_ERR, "   (ERROR (%s,%d): %s)", \
                   __FILE__, __LINE__, strerror(errno))

#define LOGERR(x...) do { \
                       if(SysLogLevel > 0) { \
                          x_syslog(LOG_ERR, x); \
                          if(errno) \
                            LOG_ERRNO; \
                       } \
                     } while(0)
#define LOGMSG(x...) do{ if(SysLogLevel > 1) x_syslog(LOG_INFO,  x); } while(0)
#define LOGDBG(x...) do{ if(SysLogLevel > 2) x_syslog(LOG_DEBUG, x); } while(0)

#define TRACELINE LOGDBG("at %s:%d %s", __FILE__, __LINE__, __FUNCTION__)



/*
 * ASSERT
 */

#ifdef NDEBUG
#  define ASSERT(expr)
#else
#  define ASSERT(expr,fatal) \
      do { \
        if(!(expr)) { \
          LOGERR("Asseretion failed: %s at %s:%d (%s)", \
                 #expr, __FILE__, __LINE__, __FUNCTION__); \
          if(fatal) \
            abort(); \
        } \
      } while(0)
#endif


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
