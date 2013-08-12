/********************************************************
 * Description : timer manager
 * Data        : 2013-08-09 00:28:45
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_TIMER_MANAGER_H
#define COMMON_BASE_TIMER_MANAGER_H


#include <list>

#include "base_common.h"
#include "base_uncopy.h"
#include "base_locker.h"
#include "base_thread.h"

NAMESPACE_COMMON_BEGIN

struct ITimer
{
    ITimer()
        : m_once(false)
        , m_id(0)
        , m_period(0)
        , m_start_time(0)
    {

    }

    virtual ~ITimer()
    {

    }

    virtual void doit() = 0;

    bool       m_once;
    size_t     m_id;
    size_t     m_period;
    uint64_t   m_start_time;
};

template <typename T, typename F, typename P>
struct OneParamMemberFuncTimer : ITimer
{
    OneParamMemberFuncTimer(T & t, F f, const P & p)
        : m_t(t)
        , m_f(f)
        , m_p(p)
    {

    }

    virtual void doit()
    {
        (m_t.*m_f)(m_p);
    }

    T & m_t;
    F   m_f;
    P   m_p;
};

template <typename T, typename F>
struct NoneParamMemberFuncTimer : ITimer
{
    NoneParamMemberFuncTimer(T & t, F f)
        : m_t(t)
        , m_f(f)
    {

    }

    virtual void doit()
    {
        (m_t.*m_f)();
    }

    T & m_t;
    F   m_f;
};

template <typename F, typename P>
struct OneParamGlobalFuncTimer : ITimer
{
    OneParamGlobalFuncTimer(F f, const P & p)
        : m_f(f)
        , m_p(p)
    {

    }

    virtual void doit()
    {
        m_f(m_p);
    }

    F   m_f;
    P   m_p;
};

template <typename F>
struct NoneParamGlobalFuncTimer : ITimer
{
    NoneParamGlobalFuncTimer(F f)
        : m_f(f)
    {

    }

    virtual void doit()
    {
        m_f();
    }

    F   m_f;
};

const size_t BAD_TIMER_ID = 0;

class YRK_EXPORT_DLL TimerManager : public BaseUncopy
{
public:
    TimerManager();
    ~TimerManager();

public:
    bool init();
    void exit();

public:
    template <typename T, typename F, typename P>
    size_t create_timer_mem(T & t, F f, const P & p);

    template <typename T, typename F>
    size_t create_timer_mem(T & t, F f);

    template <typename F, typename P>
    size_t create_timer_ptr(F f, P p);

    template <typename F>
    size_t create_timer_ptr(F f);

    void destroy_timer(size_t id);

    bool start_timer(size_t id, bool once, size_t period);
    bool stop_timer(size_t id);

public:
    void thread_run();

private:
    bool running() const;
    size_t acquire_timer_id();
    void add_pending_timer(ITimer * timer);
    void add_running_timer(ITimer * timer);
    void add_running_timer_unlock(ITimer * timer);
    ITimer * del_pending_timer(size_t id);
    ITimer * del_running_timer(size_t id);

    bool                       m_running;
    size_t                     m_timer_id;
    uint64_t                   m_current_time;
    std::list<ITimer *>        m_pending_timer;
    std::list<ITimer *>        m_running_timer;
    ThreadLocker               m_id_locker;
    ThreadLocker               m_pending_locker;
    ThreadLocker               m_running_locker;
    BaseThread                 m_thread;
};

template <typename T, typename F, typename P>
size_t TimerManager::create_timer_mem(T & t, F f, const P & p)
{
    size_t id = acquire_timer_id();
    if (BAD_TIMER_ID == id)
    {
        return(BAD_TIMER_ID);
    }

    ITimer * timer = new OneParamMemberFuncTimer<T, F, P>(t, f, p);
    if (nullptr == timer)
    {
        return(BAD_TIMER_ID);
    }

    timer->m_id = id;

    add_pending_timer(timer);

    return(id);
}

template <typename T, typename F>
size_t TimerManager::create_timer_mem(T & t, F f)
{
    size_t id = acquire_timer_id();
    if (BAD_TIMER_ID == id)
    {
        return(BAD_TIMER_ID);
    }

    ITimer * timer = new NoneParamMemberFuncTimer<T, F>(t, f);
    if (nullptr == timer)
    {
        return(BAD_TIMER_ID);
    }

    timer->m_id = id;

    add_pending_timer(timer);

    return(id);
}

template <typename F, typename P>
size_t TimerManager::create_timer_ptr(F f, P p)
{
    size_t id = acquire_timer_id();
    if (BAD_TIMER_ID == id)
    {
        return(BAD_TIMER_ID);
    }

    ITimer * timer = new OneParamGlobalFuncTimer<F, P>(f, p);
    if (nullptr == timer)
    {
        return(BAD_TIMER_ID);
    }

    timer->m_id = id;

    add_pending_timer(timer);

    return(id);
}

template <typename F>
size_t TimerManager::create_timer_ptr(F f)
{
    size_t id = acquire_timer_id();
    if (BAD_TIMER_ID == id)
    {
        return(BAD_TIMER_ID);
    }

    ITimer * timer = new NoneParamGlobalFuncTimer<F>(f);
    if (nullptr == timer)
    {
        return(BAD_TIMER_ID);
    }

    timer->m_id = id;

    add_pending_timer(timer);

    return(id);
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_TIMER_MANAGER_H
