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

#include "base_time.h"
#include "base_macro.h"

NAMESPACE_COMMON_BEGIN

int64_t base_get_time()
{
    return(static_cast<int64_t>(time(NULL)));
}

tm base_get_localtime()
{
    time_t time_now = time(nullptr);
    tm     tm_now;

#ifdef WIN32
    localtime_s(&tm_now, &time_now);
#else
    localtime_r(&time_now, &tm_now);
#endif // WIN32

    return(tm_now);
}

tm base_get_gmtime()
{
    time_t time_now = time(nullptr);
    tm     tm_now;

#ifdef WIN32
    gmtime_s(&tm_now, &time_now);
#else
    gmtime_r(&time_now, &tm_now);
#endif // WIN32

    return(tm_now);
}

struct timeval base_gettimeofday()
{
    struct timeval tv_now;

#ifdef WIN32
    SYSTEMTIME sys_now;
    GetLocalTime(&sys_now);
    tv_now.tv_sec = static_cast<long>(time(nullptr));
    tv_now.tv_usec = sys_now.wMilliseconds * 1000L;
#else
    gettimeofday(&tv_now, nullptr);
#endif // WIN32

    return(tv_now);
}

int base_get_timezone()
{
    static long time_diff = 0;
    static bool first_time = true;

    if (first_time)
    {
#ifdef WIN32
        _get_timezone(&time_diff);
#else
        time_diff = timezone;
#endif // WIN32

        time_diff /= -3600;

        first_time = false;
    }

    return(time_diff);
}

int base_get_day_of_week()
{
    tm tm_now = base_get_localtime();
    return(tm_now.tm_wday);
}

void base_millisecond_sleep(size_t milliseconds)
{
    int64_t nanoseconds = BASE_BIG_INT(1000000) * milliseconds;
    base_nanosecond_sleep(nanoseconds);
}

void base_nanosecond_sleep(int64_t nanoseconds)
{
#ifdef WIN32
    Sleep(static_cast<DWORD>(nanoseconds / 1000000));
#else
    struct timespec sleep_time;
    sleep_time.tv_sec = static_cast<time_t>(nanoseconds / 1000000000);
    sleep_time.tv_nsec = static_cast<long>(nanoseconds % 1000000000);

    while (0 != nanosleep(&sleep_time, &sleep_time) && EINTR == errno)
    {
        /* continue */
    }
#endif // WIN32
}

NAMESPACE_COMMON_END
