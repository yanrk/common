/********************************************************
 * Description : thread class in windows
 * Data        : 2013-05-23 15:30:53
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifdef WIN32


#include <process.h>

#include <cassert>

#include "base_windows_thread.h"
#include "base_exception.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

WindowsThread::WindowsThread
(
    thread_func func,
    void * argument, 
    const char * name
)
    : m_func(func)
    , m_argu(argument)
    , m_name(nullptr != name ? name : "")
    , m_thread(nullptr)
    , m_running(false)
    , m_locker("thread locker")
{

}

WindowsThread::~WindowsThread()
{
    release();
}

bool WindowsThread::acquire()
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);
    if (m_running || nullptr == m_func)
    {
        return(false);
    }
    m_running = true;
    thread_guard.release();

    m_thread = reinterpret_cast<HANDLE>(
        _beginthreadex(nullptr, 0, m_func, m_argu, 0, nullptr));
    if (nullptr == m_thread)
    {
        m_running = false;
        return(false);
    }
    else
    {
        return(true);
    }
}

void WindowsThread::release()
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);
    if (!m_running || nullptr == m_thread)
    {
        return;
    }
    m_running = false;
    thread_guard.release();

    DWORD ret = WaitForSingleObject(m_thread, INFINITE);

    if (WAIT_OBJECT_0 != ret)
    {
        assert(WAIT_OBJECT_0 == ret);
        BASE_THROW("release thread(\"%s\") failed", m_name.c_str());
    }

    CloseHandle(m_thread);
}

void WindowsThread::set_thread_args
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

bool WindowsThread::running() const
{
    return(m_running);
}

size_t WindowsThread::thread_id() const
{
    return(static_cast<size_t>(GetCurrentThreadId()));
}

std::string WindowsThread::thread_name()
{
    BaseGuard<ThreadLocker> thread_guard(m_locker);

    return(m_name);
}

NAMESPACE_COMMON_END


#endif // WIN32
