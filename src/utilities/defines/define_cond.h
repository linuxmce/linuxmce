//
// Author : Remus C.
//
// Changed by : ...
//
// Description : conditionals at runtime
//
// Notes :
//     currently tested only for gcc 3.4
//     for other compilers, replace ... and __VA_ARGS__
//

#ifndef _DEFINE_COND_H_
#define _DEFINE_COND_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "define_cond.h"
#endif

// check condition, log on error
#define _COND(x) \
    do \
    { \
        if (!(x)) \
        { \
            _LOG_ERR("_COND(" #x ")"); \
        } \
    } while (0)

// check condition, log and return on error
#define _COND_RET(x, ...) \
    do \
    { \
        if (!(x)) \
        { \
            _LOG_ERR("_COND_RET(" #x ", " #__VA_ARGS__ ")"); \
            return __VA_ARGS__; \
        } \
    } while (0)

// check condition, log and break on error
#define _COND_BREAK(x, ...) \
    do \
    { \
        if (!(x)) \
        { \
            _LOG_ERR("_COND_RET(" #x ", " #__VA_ARGS__ ")"); \
            break; \
        } \
    } while (0)

// check (var == value_old), log on error, assign (var = value_new) in any case
#define _COND_ASSIGN(var, value_old, value_new) \
    do \
    { \
        if (!(var == value_old)) \
        { \
            _LOG_ERR("_COND_ASSIGN(" #var ", " #value_old ", " #value_new ")"); \
        } \
        var = value_new; \
    } while (0)

#define _LOG_ERR_BREAK(...) \
    if (true) \
    { \
        _LOG_ERR(__VA_ARGS__); \
        break; \
    } \
    else do {} while (0)

#endif
// _DEFINE_COND_H_
