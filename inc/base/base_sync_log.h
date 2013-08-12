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

#ifndef COMMON_BASE_SYNC_LOG_H
#define COMMON_BASE_SYNC_LOG_H


#include "base_base_log.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL SyncLog : public BaseLog
{
public:
    SyncLog
    (
        const std::string & path, 
        const std::string & log_type, 
        LOG_LEVEL min_log_level, 
        size_t max_file_size
    );

    virtual ~SyncLog();

protected:
    virtual void save_record
    (
        LOG_LEVEL log_level, 
        const char * data, 
        size_t size
    );
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_SYNC_LOG_H
