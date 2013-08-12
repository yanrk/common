/********************************************************
 * Description : sink and slot of timer
 * Data        : 2013-08-09 22:21:48
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_TIMER_H
#define COMMON_BASE_TIMER_H


#include "base_common.h"
#include "base_uncopy.h"
#include "base_thread.h"

NAMESPACE_COMMON_BEGIN

class BaseTimer;

class YRK_EXPORT_DLL ITimerSink
{
public:
    virtual ~ITimerSink();

public:
    virtual void on_timer(bool first_time, size_t index) = 0;
};

class YRK_EXPORT_DLL BaseTimer : BaseUncopy
{
public:
    BaseTimer();
    ~BaseTimer();

public:
    bool init(ITimerSink * sink, size_t period);
    void exit();

public:
    static void run(BaseTimer * timer);

private:
    bool running() const;

private:
    ITimerSink         * m_sink;
    size_t               m_period;
    size_t               m_index;
    bool                 m_first_time;
    bool                 m_running;
    BaseThread           m_thread;
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_TIMER_H
