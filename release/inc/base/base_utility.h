/********************************************************
 * Description : assistant functions
 * Data        : 2013-08-04 16:35:12
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include <string>

#include "base_common.h"
#include "base_log_types.h"

#ifndef COMMON_BASE_UTILITY_H
#define COMMON_BASE_UTILITY_H


NAMESPACE_COMMON_BEGIN

YRK_CXX_API(bool) base_load_log_config(
    const std::string & file, 
    LOG_CONFIG & log_config
);

NAMESPACE_COMMON_END


#endif // COMMON_BASE_UTILITY_H
