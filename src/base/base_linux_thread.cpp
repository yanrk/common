/********************************************************
 * Description : thread class in linux
 * Data        : 2013-05-23 17:03:46
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef WIN32


#include <cassert>

#include "base_linux_thread.h"
#include "base_exception.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

LinuxThread::LinuxThread
(
    thread_func func,
    void * argument, 
    const char * name
)
    : m_func(func)
    , m_argu(argument)
    , m_name(nullptr != name ? name : "")
    , m_thread()
    , m_running(false)
    , m_locker("thread locker")
{

}

LinuxThread::~LinuxThread()
{
    release();
}

bool LinuxThread::acquire()
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);
    if (m_running || nullptr == m_func)
    {
        return(false);
    }
    m_running = true;
    thread_guard.release();

    if (0 != pthread_create(&m_thread, nullptr, m_func, m_argu))
    {
        m_running = false;
        return(false);
    }
    else
    {
        return(true);
    }
}

void LinuxThread::release()
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);
    if (!m_running)
    {
        return;
    }
    m_running = false;
    thread_guard.release();

    if (0 != pthread_join(m_thread, nullptr))
    {
        assert(false);
        BASE_THROW("release thread(\"%s\") failed", m_name.c_str());
    }
}

void LinuxThread::set_thread_args
(
    thread_func func,
    void * argument, 
    const char * name
)
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);

    if (!m_running)
    {
        m_func = func;
        m_argu = argument;
        m_name = (nullptr != name ? name : "");
    }
}

bool LinuxThread::running() const
{
    return(m_running);
}

size_t LinuxThread::thread_id() const
{
    return(static_cast<size_t>(pthread_self()));
}

std::string LinuxThread::thread_name()
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);

    return(m_name);
}

NAMESPACE_COMMON_END


#endif // WIN32
