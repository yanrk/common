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

#ifndef COMMON_BASE_WINDOWS_LOCKER_H
#define COMMON_BASE_WINDOWS_LOCKER_H


#ifdef WIN32


#include <windows.h>

#include <string>

#include "base_common.h"
#include "base_uncopy.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL WindowsThreadLocker : private BaseUncopy
{
public:
    WindowsThreadLocker(const char * name = nullptr);
    ~WindowsThreadLocker();

public:
    void acquire();
    void release();
    bool try_acquire(); /* not support, alway be false */

private:
    std::string      m_name;
    CRITICAL_SECTION m_locker;
};


class YRK_EXPORT_DLL WindowsProcessLocker : private BaseUncopy
{
public:
    WindowsProcessLocker(const char * name = nullptr);
    ~WindowsProcessLocker();

public:
    void acquire();
    void release();
    bool try_acquire();

private:
    std::string m_name;
    HANDLE      m_locker;
};

NAMESPACE_COMMON_END


#endif // WIN32


#endif // COMMON_BASE_WINDOWS_LOCKER_H
