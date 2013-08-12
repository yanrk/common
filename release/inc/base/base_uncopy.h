/********************************************************
 * Description : base class which not support copy
 * Data        : 2013-05-20 12:24:45
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_UNCOPY_H
#define COMMON_BASE_UNCOPY_H


#include "base_common.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL BaseUncopy
{
public:
    BaseUncopy();
    ~BaseUncopy();

private:
    BaseUncopy(const BaseUncopy &);
    BaseUncopy & operator = (const BaseUncopy &);
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_UNCOPY_H
