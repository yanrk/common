/********************************************************
 * Description : c interface of log module
 * Data        : 2013-05-22 22:34:24
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_LOG_H
#define COMMON_BASE_LOG_H


#include <string>

#include "base_common.h"
#include "base_log_types.h"
#include "base_base_log.h"

NAMESPACE_COMMON_BEGIN

YRK_CXX_API(bool) log_init(const LOG_CONFIG & log_config);

YRK_CXX_API(void) log_exit();

YRK_C_API(void) set_log_level(LOG_TYPE log_type, LOG_LEVEL log_level);

YRK_C_API(void) run_log
(
    LOG_LEVEL level, const char * file, const char * func, 
    size_t line, const char * format, ...
);

YRK_C_API(void) debug_log
(
    LOG_LEVEL level, const char * file, const char * func, 
    size_t line, const char * format, ...
);

#define RUN_LOG(level, fmt, ...)                                     \
run_log((level), __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define RUN_LOG_CRI(fmt, ...) RUN_LOG(CRI_LEVEL, (fmt), ##__VA_ARGS__)
#define RUN_LOG_ERR(fmt, ...) RUN_LOG(ERR_LEVEL, (fmt), ##__VA_ARGS__)
#define RUN_LOG_WAR(fmt, ...) RUN_LOG(WAR_LEVEL, (fmt), ##__VA_ARGS__)
#define RUN_LOG_DBG(fmt, ...) RUN_LOG(DBG_LEVEL, (fmt), ##__VA_ARGS__)
#define RUN_LOG_TRK(fmt, ...) RUN_LOG(TRK_LEVEL, (fmt), ##__VA_ARGS__)

#define DBG_LOG(fmt, ...)                                                \
debug_log(DBG_LEVEL, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

NAMESPACE_COMMON_END


#endif // COMMON_BASE_LOG_H
