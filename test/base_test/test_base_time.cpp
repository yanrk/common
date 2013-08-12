#include "test_base.h"
#include "base_time.h"
#include "base_macro.h"

USING_NAMESPACE_COMMON

void test_base_time(void)
{
    size_t time_now = static_cast<size_t>(base_get_time());
    printf("seconds from 1900-01-01 00:00:00 is %u\n", time_now);

    tm tm_utc_now = base_get_gmtime();
    printf
    (
        "UTC: %4d-%02d-%02d %02d:%02d:%02d\n", 
        tm_utc_now.tm_year + 1900, tm_utc_now.tm_mon + 1, tm_utc_now.tm_mday, 
        tm_utc_now.tm_hour, tm_utc_now.tm_min, tm_utc_now.tm_sec
    );

    static const char * const WEEK[7] = 
    {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    tm tm_now = base_get_localtime();
    struct timeval tv_now = base_gettimeofday();
    int millisecond = static_cast<int>(tv_now.tv_usec / 1000);
    int time_zone = base_get_timezone();
    int day_of_week = base_get_day_of_week();

    printf
    (
        "Local Time: %4d-%02d-%02d %02d:%02d:%02d.%03d GMT%+03d:00 %s\n", 
        tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, 
        tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, millisecond, 
        time_zone, WEEK[day_of_week]
    );

    base_millisecond_sleep(1000);

    int64_t t_start = base_get_time();
    base_nanosecond_sleep(BASE_BIG_INT(5000000000));
    int64_t t_end = base_get_time();

    int sleep_seconds = static_cast<int>(t_end - t_start);
    if (sleep_seconds < 5 - 1 || sleep_seconds > 5 + 1)
    {
        printf("base_nanosecond_sleep maybe not accurate\n");
    }
    else
    {
        printf("base_nanosecond_sleep works well\n");
    }
}
