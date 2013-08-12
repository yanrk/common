/********************************************************
 * Description : time functions
 * Data        : 2013-05-24 12:15:59
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_TIME_H
#define COMMON_BASE_TIME_H


#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
    #include <errno.h>
#endif // WIN32

#include <ctime>
#include <cstdint>

#include "base_common.h"

NAMESPACE_COMMON_BEGIN

YRK_C_API(int64_t) base_get_time();

YRK_C_API(tm) base_get_localtime();

YRK_C_API(tm) base_get_gmtime();

YRK_C_API(struct timeval) base_gettimeofday();

YRK_C_API(int) base_get_timezone();

YRK_C_API(int) base_get_day_of_week();

YRK_C_API(void) base_millisecond_sleep(size_t milliseconds);

YRK_C_API(void) base_nanosecond_sleep(int64_t nanoseconds);

NAMESPACE_COMMON_END


#endif // COMMON_BASE_TIME_H
