/********************************************************
 * Description : log class of lazy mode
 * Data        : 2013-05-23 13:56:44
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_LAZY_LOG_H
#define COMMON_BASE_LAZY_LOG_H


#include "base_base_log.h"
#include "base_locker.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL LazyLog : public BaseLog
{
public:
    LazyLog
    (
        const std::string & path, 
        const std::string & log_type, 
        LOG_LEVEL min_log_level, 
        size_t max_file_size, 
        size_t max_record_count
    );

    virtual ~LazyLog();

protected:
    virtual void save_record
    (
        LOG_LEVEL log_level, 
        const char * data, 
        size_t size
    );

private:
    void flush();

private:
    struct LOG_RECORD
    {
        char   data[LOG_RECORD_SIZE];
        size_t size;
    };

private:
    LOG_RECORD   * m_record;
    size_t         m_reserve;
    size_t         m_begin;
    size_t         m_count;
    ThreadLocker   m_record_locker;
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_LAZY_LOG_H
