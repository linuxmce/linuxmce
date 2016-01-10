//
// Author : Remus C.
//
// Changed by : ...
//
// Description : other macros
//
// Notes :
//     currently tested only for gcc 3.4
//

#ifndef _DEFINE_OTHER_H_
#define _DEFINE_OTHER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "define_other.h"
#endif

// log an error and seg-fault
#define _SEG_FAULT(...) \
    do \
    { \
        _LOG_ERR("seg-fault here : " __VA_ARGS__); \
        int *pSegFault = NULL; \
        *pSegFault = 0; \
    } while (0)

// case CONST : return "CONST"
#define CASE_const_ret_str(x) case x: return #x

// there should be no more any compilers needing the "#else" version
#if 1
#    define UNUSED_ARG(X) /* X */
#else
#    define UNUSED_ARG(X) X
#endif

// use it for generated code to get rid of "unused variable" warning
#ifdef __cplusplus
#    ifdef __BORLANDC__
#        define UNUSED_VAR(X) X
#    else
template <class T> inline void UNUSED_VAR(const T& UNUSED_ARG(t)) { }
#    endif
#endif

#endif
// _DEFINE_OTHER_H_
