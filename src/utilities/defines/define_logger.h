//
// Author : Remus C.
//
// Changed by : ...
//
// Description : logger macros
//
// Notes :
//     currently tested only for gcc 3.4
//     for other compilers, replace __PRETTY_FUNCTION__ with
//     __func__ or __FUNCTION__
//

#ifndef _DEFINE_LOGGER_H_
#define _DEFINE_LOGGER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "define_logger.h"
#endif

/*
#if (! defined _LOG_NFO)
#  if (defined wxVERSION_NUMBER) || (defined USE_DEBUG_CODE)
#    define _LOG_NFO(x, ...) _wx_log_nfo(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_ERR(x, ...) _wx_log_err(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_WRN(x, ...) _wx_log_wrn(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_DBG(x, ...) _wx_log_dbg(x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    include "wx_logger.h"
#  elif (defined DCECommon)
*/
#    include "DCE/Logger.h"
#    define _LOG_NFO(x, ...) DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS,   x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_ERR(x, ...) DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_WRN(x, ...) DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,  x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_DBG(x, ...) DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,  x " \033[0m\033[32m// %s\033[0m" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
/*
#  else
#    define _LOG_NFO(x, ...) fprintf(stderr, "\n\033[0m\033[36mINFO:\033[1m "    x " \033[0m\033[32m// %s\033[0m\n" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_ERR(x, ...) fprintf(stderr, "\n\033[0m\033[31mERROR:\033[1m "   x " \033[0m\033[32m// %s\033[0m\n" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_WRN(x, ...) fprintf(stderr, "\n\033[0m\033[35mWARNING:\033[1m " x " \033[0m\033[32m// %s\033[0m\n" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#    define _LOG_DBG(x, ...) fprintf(stderr, "\n\033[0m\033[33mDEBUG:\033[0m "   x " \033[0m\033[32m// %s\033[0m\n" , ## __VA_ARGS__ , __PRETTY_FUNCTION__)
#  endif
#endif // (! defined _LOG_NFO)
*/

#endif
// _DEFINE_LOGGER_H_
