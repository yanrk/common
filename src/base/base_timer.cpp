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

#include "base_timer.h"
#include "base_time.h"

NAMESPACE_COMMON_BEGIN

static YRK_THR_FUNC_RET_T YRK_STDCALL timer_run(void * argument)
{
    BaseTimer * timer = reinterpret_cast<BaseTimer *>(argument);
    if (nullptr != timer)
    {
        BaseTimer::run(timer);
    }
    return(THREAD_DEFAULT_RET);
}

ITimerSink::~ITimerSink()
{

}

BaseTimer::BaseTimer()
    : m_sink(nullptr)
    , m_period(0)
    , m_index(0)
    , m_first_time(true)
    , m_running(false)
    , m_thread()
{
    m_thread.set_thread_args(timer_run, this, "base timer");
}

BaseTimer::~BaseTimer()
{
    exit();
}

bool BaseTimer::init(ITimerSink * sink, size_t period)
{
    exit();

    if (nullptr == sink)
    {
        return(false);
    }

    m_sink = sink;
    m_period = period;
    m_index = 0;
    m_first_time = true;
    m_running = true;

    if (!m_thread.acquire())
    {
        m_running = false;
    }

    return(m_running);
}

void BaseTimer::exit()
{
    if (m_running)
    {
        m_running = false;
        m_thread.release();
    }
}

bool BaseTimer::running() const
{
    return(m_running);
}

void BaseTimer::run(BaseTimer * timer)
{
    if (nullptr == timer)
    {
        return;
    }

    while (timer->running())
    {
        timer->m_sink->on_timer(timer->m_first_time, timer->m_index);
        base_millisecond_sleep(timer->m_period);
        ++timer->m_index;
        timer->m_first_time = false;
    }
}

NAMESPACE_COMMON_END
