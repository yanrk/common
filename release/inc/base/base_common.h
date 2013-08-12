/********************************************************
 * Description : include some common files
 * Data        : 2013-05-18 18:38:27
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_COMMON_H
#define COMMON_BASE_COMMON_H


#include <cstddef>
#include <cstdint>

#include "base_export.h"
#include "base_namespace.h"
#include "base_macro.h"

#ifdef WIN32
    typedef unsigned int        YRK_THR_FUNC_RET_T;
    #define THREAD_DEFAULT_RET  0UL
#else
    typedef void *              YRK_THR_FUNC_RET_T;
    #define THREAD_DEFAULT_RET  nullptr
#endif // WIN32

typedef YRK_THR_FUNC_RET_T (YRK_STDCALL * thread_func) (void *);

#endif // COMMON_BASE_COMMON_H
