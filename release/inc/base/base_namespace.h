/********************************************************
 * Description : macro of namespace
 * Data        : 2013-05-18 18:28:57
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_NAMESPACE_H
#define COMMON_BASE_NAMESPACE_H


namespace yanrk
{
    namespace common
    {

    }
}


#define NAMESPACE_COMMON_BEGIN   \
namespace yanrk                  \
{                                \
    namespace common             \
    {

#define NAMESPACE_COMMON_END     \
    }                            \
}

#define USING_NAMESPACE_COMMON   \
using namespace yanrk::common;

namespace YRK_CMN = yanrk::common;


#endif // COMMON_BASE_NAMESPACE_H
