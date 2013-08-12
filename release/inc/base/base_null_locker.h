/********************************************************
 * Description : null locker class
 * Data        : 2013-05-30 16:45:57
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_NULL_LOCKER_H
#define COMMON_BASE_NULL_LOCKER_H


#include "base_common.h"
#include "base_uncopy.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL NullLocker : private BaseUncopy
{
public:
    NullLocker(const char * name = nullptr);
    ~NullLocker();

public:
    void acquire();
    void release();
    bool try_acquire();
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_NULL_LOCKER_H
