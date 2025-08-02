/********************************************************
 * Description : log module
 * Data        : 2013-05-22 22:34:24
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include <cstdarg>
#include <cassert>
#include <string>

#include "base_log.h"
#include "base_memory.h"
#include "base_base_log.h"
#include "base_lazy_log.h"
#include "base_sync_log.h"
#include "base_asyn_log.h"

NAMESPACE_COMMON_BEGIN

static BaseLog * g_log[LOG_TYPE_MAX];
static bool      g_need_aysn_write_thread = false;

bool log_init(const LOG_CONFIG & log_config)
{
    for (size_t type = LOG_TYPE_MIN; type < LOG_TYPE_MAX; ++type)
    {
        switch (log_config.log_file[type].write_mode)
        {
            case SYNC_WRITE_MODE:
            {
                BASE_NEW
                (
                    g_log[type], 
                    SyncLog
                    (
                        log_config.log_file_path, 
                        LOG_TYPE_INFO[type], 
                        log_config.log_file[type].log_min_level, 
                        log_config.log_file[type].log_file_size
                    )
                );
                break;
            }
            case ASYN_WRITE_MODE:
            {
                BASE_NEW
                (
                    g_log[type], 
                    AsynLog
                    (
                        log_config.log_file_path, 
                        LOG_TYPE_INFO[type], 
                        log_config.log_file[type].log_min_level, 
                        log_config.log_file[type].log_file_size
                    )
                );
                g_need_aysn_write_thread = true;
                break;
            }
            case LAZY_WRITE_MODE:
            {
                BASE_NEW
                (
                    g_log[type], 
                    LazyLog
                    (
                        log_config.log_file_path, 
                        LOG_TYPE_INFO[type], 
                        log_config.log_file[type].log_min_level, 
                        log_config.log_file[type].log_file_size, 
                        log_config.log_file[type].buffer_count
                    )
                );
                break;
            }
            default:
            {
                assert(false);
                BASE_THROW("invalid log write mode");
                return false;
            }
        }

        if (nullptr == g_log[type])
        {
            assert(nullptr != g_log[type]);
            BASE_THROW("create log[%s] failed", LOG_TYPE_INFO[type]);
            return false;
        }
    }

    if (g_need_aysn_write_thread)
    {
        return AsynLog::acquire_write_thread();
    }

    return true;
}

void log_exit()
{
    for (size_t type = LOG_TYPE_MIN; type < LOG_TYPE_MAX; ++type)
    {
        BASE_DELETE(g_log[type]);
    }

    if (g_need_aysn_write_thread)
    {
        AsynLog::release_write_thread();
        g_need_aysn_write_thread = false;
    }
}

void set_log_level(LOG_TYPE log_type, LOG_LEVEL log_level)
{
    if (nullptr != g_log[log_type])
    {
        g_log[log_level]->set_min_log_level(log_level);
    }
}

void run_log
(
    LOG_LEVEL level, const char * file, const char * func, 
    size_t line, const char * format, ...
)
{
    va_list args = nullptr;

    va_start(args, format);

    if (nullptr != g_log[LOG_TYPE_RUN])
    {
        g_log[LOG_TYPE_RUN]->push_record
        (
            level, file, func, line, format, args
        );
    }
    if (nullptr != g_log[LOG_TYPE_DBG])
    {
        g_log[LOG_TYPE_DBG]->push_record
        (
            level, file, func, line, format, args
        );
    }

    va_end(args);
}

void debug_log
(
    LOG_LEVEL level, const char * file, const char * func, 
    size_t line, const char * format, ...
)
{
    if (nullptr == g_log[LOG_TYPE_DBG])
    {
        return;
    }

    va_list args = nullptr;

    va_start(args, format);

    g_log[LOG_TYPE_DBG]->push_record
    (
        level, file, func, line, format, args
    );

    va_end(args);
}

NAMESPACE_COMMON_END
