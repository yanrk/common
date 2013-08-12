/********************************************************
 * Description : log class of sync mode
 * Data        : 2013-05-23 14:02:13
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include "base_sync_log.h"

NAMESPACE_COMMON_BEGIN

SyncLog::SyncLog
(
    const std::string & path, 
    const std::string & log_type, 
    LOG_LEVEL min_log_level, 
    size_t max_file_size
)
    : BaseLog(path, log_type, min_log_level, max_file_size)
{

}

SyncLog::~SyncLog()
{

}

void SyncLog::save_record(LOG_LEVEL log_level, const char * data, size_t size)
{
    log_level = LOG_LEVEL_MAX; /* prevent warning */
    write(data, size);
}

NAMESPACE_COMMON_END
