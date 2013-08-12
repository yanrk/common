/********************************************************
 * Description : macro of common base
 * Data        : 2013-05-24 08:04:05
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_MACRO_H
#define COMMON_BASE_MACRO_H


#include <cassert>

#include "base_exception.h"

#define IF_VAR_EQUAL_VALUE_RETURN_RET(var, value, ret)      \
    if ((value) == (var))                                   \
    {                                                       \
        assert((value) != (var));                           \
        BASE_THROW(#var " is " #value);                     \
        return ret;                                         \
    }

#define IF_VAR_EQUAL_VALUE_RETURN_VOID(var, value)          \
    IF_VAR_EQUAL_VALUE_RETURN_RET(var, value, )

#define IF_VAR_NOT_EQUAL_VALUE_RETURN_RET(var, value, ret)  \
    if ((value) != (var))                                   \
    {                                                       \
        assert((value) == (var));                           \
        BASE_THROW(#var " is not " #value);                 \
        return ret;                                         \
    }

#define IF_VAR_NOT_EQUAL_VALUE_RETURN_VOID(var, value)      \
    IF_VAR_NOT_EQUAL_VALUE_RETURN_RET(var, value, )

#if defined _MSC_VER || defined __BORLANDC__
    #define BASE_BIG_INT(n)   n##I64
    #define BASE_BIG_UINT(n)  n##UI64
#else
    #define BASE_BIG_INT(n)   n##LL
    #define BASE_BIG_UINT(n)  n##ULL
#endif // _MSC_VER || __BORLANDC__

#ifdef __cplusplus
    #define BASE_INLINE               inline
    #define BASE_DEFAULT_PARAM(var)   = var
#else
    #define BASE_INLINE               static
    #define BASE_DEFAULT_PARAM(var)
#endif // __cplusplus


#endif // COMMON_BASE_MACRO_H
