/********************************************************
 * Description : locker classes in windows
 * Data        : 2013-05-20 09:02:33
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifdef WIN32


#include <cassert>

#include "base_windows_locker.h"
#include "base_exception.h"

NAMESPACE_COMMON_BEGIN

WindowsThreadLocker::WindowsThreadLocker(const char * name)
    : m_name(nullptr != name ? name : "")
    , m_locker()
{
    InitializeCriticalSection(&m_locker);
}

WindowsThreadLocker::~WindowsThreadLocker()
{
    DeleteCriticalSection(&m_locker);
}

void WindowsThreadLocker::acquire()
{
    EnterCriticalSection(&m_locker);
}

void WindowsThreadLocker::release()
{
    LeaveCriticalSection(&m_locker);
}

bool WindowsThreadLocker::try_acquire()
{
    return false;
}


WindowsProcessLocker::WindowsProcessLocker(const char * name)
    : m_name(nullptr != name ? name : "")
    , m_locker(CreateMutex(nullptr, FALSE, name))
{
    if (nullptr == m_locker)
    {
        assert(nullptr != m_locker);
        BASE_THROW("create locker(\"%s\") failed", m_name.c_str());
    }
}

WindowsProcessLocker::~WindowsProcessLocker()
{
    if (!CloseHandle(m_locker))
    {
        assert(false);
        BASE_THROW("destroy locker(\"%s\") failed", m_name.c_str());
    }
}

void WindowsProcessLocker::acquire()
{
    DWORD ret = WaitForSingleObject(m_locker, INFINITE);

    if (WAIT_OBJECT_0 != ret)
    {
        assert(WAIT_OBJECT_0 == ret);
        BASE_THROW("lock locker(\"%s\") failed", m_name.c_str());
    }
}

void WindowsProcessLocker::release()
{
    if (!ReleaseMutex(m_locker))
    {
        assert(false);
        BASE_THROW("unlock locker(\"%s\") failed", m_name.c_str());
    }
}

bool WindowsProcessLocker::try_acquire()
{
    DWORD ret = WaitForSingleObject(m_locker, 0);
    return WAIT_OBJECT_0 == ret;
}

NAMESPACE_COMMON_END


#endif // WIN32
