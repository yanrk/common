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

#ifndef COMMON_BASE_WINDOWS_THREAD_H
#define COMMON_BASE_WINDOWS_THREAD_H


#ifdef WIN32


#include <windows.h>

#include <string>

#include "base_common.h"
#include "base_uncopy.h"
#include "base_locker.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL WindowsThread : private BaseUncopy
{
public:
    WindowsThread
    (
        thread_func func = nullptr,
        void * argument = nullptr, 
        const char * name = nullptr
    );

    ~WindowsThread();

public:
    bool acquire();
    void release();

public:
    void set_thread_args
    (
        thread_func func,
        void * argument = nullptr, 
        const char * name = nullptr
    );

public:
    bool running() const;
    std::string thread_name();
    size_t thread_id() const;

private:
    thread_func    m_func;
    void         * m_argu;
    std::string    m_name;
    HANDLE         m_thread;
    bool           m_running;
    ThreadLocker   m_locker;
};

NAMESPACE_COMMON_END


#endif // WIN32


#endif // COMMON_BASE_WINDOWS_THREAD_H
