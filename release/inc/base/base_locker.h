/********************************************************
 * Description : locker classes
 * Data        : 2013-05-20 08:03:27
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_LOCKER_H
#define COMMON_BASE_LOCKER_H


#include "base_common.h"
#include "base_null_locker.h"

#ifdef WIN32
    #include "base_windows_locker.h"
#else
    #include "base_linux_locker.hpp"
#endif // WIN32

NAMESPACE_COMMON_BEGIN

#ifdef WIN32
    typedef WindowsThreadLocker      ThreadLocker;
    typedef WindowsProcessLocker     ProcessLocker;
#else
    typedef LinuxMutexLocker<true>   ThreadLocker;
    typedef LinuxMutexLocker<false>  ProcessLocker;
#endif // WIN32

NAMESPACE_COMMON_END


#endif // COMMON_BASE_LOCKER_H
