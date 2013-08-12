/********************************************************
 * Description : thread class
 * Data        : 2013-05-23 18:02:15
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_THREAD_H
#define COMMON_BASE_THREAD_H


#include "base_common.h"

#ifdef WIN32
    #include "base_windows_thread.h"
#else
    #include "base_linux_thread.h"
#endif // WIN32

NAMESPACE_COMMON_BEGIN

#ifdef WIN32
    typedef WindowsThread  BaseThread;
#else
    typedef LinuxThread    BaseThread;
#endif // WIN32

NAMESPACE_COMMON_END


#endif // COMMON_BASE_THREAD_H
