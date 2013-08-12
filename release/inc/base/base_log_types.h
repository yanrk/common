/********************************************************
 * Description : type definition of log module
 * Data        : 2013-05-22 22:06:21
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_LOG_TYPES_H
#define COMMON_BASE_LOG_TYPES_H


#include <string>

#include "base_namespace.h"

NAMESPACE_COMMON_BEGIN

enum LOG_TYPE
{
    LOG_TYPE_MIN, 
    LOG_TYPE_RUN = LOG_TYPE_MIN, 
    LOG_TYPE_DBG, 
    LOG_TYPE_MAX
};

const char * const LOG_TYPE_INFO[LOG_TYPE_MAX] = 
{
    "run", "debug"
};

enum LOG_WRITE_MODE
{
    SYNC_WRITE_MODE, 
    ASYN_WRITE_MODE, 
    LAZY_WRITE_MODE
};

enum LOG_LEVEL
{
    LOG_LEVEL_MIN, 
    CRI_LEVEL = LOG_LEVEL_MIN, 
    ERR_LEVEL, 
    WAR_LEVEL, 
    DBG_LEVEL, 
    TRK_LEVEL, 
    LOG_LEVEL_MAX
};

const char * const LOG_LEVEL_INFO[LOG_LEVEL_MAX] = 
{
    "Critical", 
    "   Error", 
    " Warning", 
    "   Debug", 
    "   Track"  
};

struct LOG_CONFIG
{
    std::string log_file_path;

    struct
    {
        LOG_WRITE_MODE  write_mode;
        LOG_LEVEL       log_min_level;
        size_t          log_file_size;
        size_t          buffer_count;
    } log_file[LOG_TYPE_MAX];
};

enum
{
    LOG_RECORD_SIZE = 2048
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_LOG_TYPES_H
