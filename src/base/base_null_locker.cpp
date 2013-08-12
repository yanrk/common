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

#include "base_null_locker.h"

NAMESPACE_COMMON_BEGIN

NullLocker::NullLocker(const char * name)
{
    name = nullptr; /* prevent warning */
}

NullLocker::~NullLocker()
{

}

void NullLocker::acquire()
{

}

void NullLocker::release()
{

}

bool NullLocker::try_acquire()
{
    return(true);
}

NAMESPACE_COMMON_END
