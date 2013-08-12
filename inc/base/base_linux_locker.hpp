/********************************************************
 * Description : locker classes in linux
 * Data        : 2013-05-20 10:46:27
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_LINUX_LOCKER_H
#define COMMON_BASE_LINUX_LOCKER_H


#ifndef WIN32


#include <pthread.h>

#include <cassert>
#include <string>

#include "base_common.h"
#include "base_uncopy.h"
#include "base_exception.h"

NAMESPACE_COMMON_BEGIN

template <bool b_thread_lock>
class YRK_EXPORT_DLL LinuxMutexLocker : private BaseUncopy
{
public:
    LinuxMutexLocker(const char * name = nullptr);
    ~LinuxMutexLocker();

public:
    void acquire();
    void release();
    bool try_acquire();

private:
    std::string     m_name;
    pthread_mutex_t m_locker;
};

template <bool b_thread_lock>
LinuxMutexLocker<b_thread_lock>::LinuxMutexLocker(const char * name)
    : m_name(nullptr != name ? name : "")
    , m_locker()
{
    pthread_mutexattr_t locker_attr;

    if (0 != pthread_mutexattr_init(&locker_attr))
    {
        assert(false);
        BASE_THROW("init locker(\"%s\") attr failed", m_name.c_str());
    }

    int shared_state = b_thread_lock 
                     ? PTHREAD_PROCESS_PRIVATE 
                     : PTHREAD_PROCESS_SHARED;


    if (0 != pthread_mutexattr_setpshared(&locker_attr, shared_state))
    {
        assert(false);
        BASE_THROW("set locker(\"%s\") attr failed", m_name.c_str());
    }

    if (0 != pthread_mutex_init(&m_locker, &locker_attr))
    {
        assert(false);
        BASE_THROW("create locker(\"%s\") failed", m_name.c_str());
    }

    if (0 != pthread_mutexattr_destroy(&locker_attr))
    {
        assert(false);
        BASE_THROW("destroy locker(\"%s\") attr failed", m_name.c_str());
    }
}

template <bool b_thread_lock>
LinuxMutexLocker<b_thread_lock>::~LinuxMutexLocker()
{
    if (0 != pthread_mutex_destroy(&m_locker))
    {
        assert(false);
        BASE_THROW("destroy locker(\"%s\") failed", m_name.c_str());
    }
}

template <bool b_thread_lock>
void LinuxMutexLocker<b_thread_lock>::acquire()
{
    if (0 != pthread_mutex_lock(&m_locker))
    {
        assert(false);
        BASE_THROW("lock locker(\"%s\") failed", m_name.c_str());
    }
}

template <bool b_thread_lock>
void LinuxMutexLocker<b_thread_lock>::release()
{
    if (0 != pthread_mutex_unlock(&m_locker))
    {
        assert(false);
        BASE_THROW("unlock locker(\"%s\") failed", m_name.c_str());
    }
}

template <bool b_thread_lock>
bool LinuxMutexLocker<b_thread_lock>::try_acquire()
{
    return(0 == pthread_mutex_trylock(&m_locker));
}

NAMESPACE_COMMON_END


#endif // WIN32


#endif // COMMON_BASE_LINUX_LOCKER_H
