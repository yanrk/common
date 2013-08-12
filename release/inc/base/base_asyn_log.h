/********************************************************
 * Description : log class of asyn mode
 * Data        : 2013-05-23 20:12:41
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_ASYN_LOG_H
#define COMMON_BASE_ASYN_LOG_H


#include <list>
#include <vector>

#include "base_thread.h"
#include "base_locker.h"
#include "base_object_pool.hpp"
#include "base_base_log.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL AsynLog : public BaseLog
{
public:
    AsynLog
    (
        const std::string & path, 
        const std::string & log_type, 
        LOG_LEVEL min_log_level, 
        size_t max_file_size
    );

    virtual ~AsynLog();

public:
    static bool acquire_write_thread();
    static void release_write_thread();

    static void push_asyn_log_object(AsynLog * asyn_log);
    static void pop_asyn_log_object(AsynLog * asyn_log);

    static void write_file();

protected:
    virtual void save_record
    (
        LOG_LEVEL log_level, 
        const char * data, 
        size_t size
    );

private:
    enum { ASYN_RECORD_SIZE = LOG_RECORD_SIZE * 8 };

    struct LOG_RECORD
    {
        char   data[ASYN_RECORD_SIZE];
        size_t size;
    };

private:
    static BaseThread                      s_write_file_thread;
    static std::list<AsynLog *>            s_asyn_log_obj_list;
    static ThreadLocker                    s_asyn_log_obj_locker;

private:
    
    ObjectPool<LOG_RECORD, ThreadLocker>   m_idle_pool;
    ObjectPool<LOG_RECORD, ThreadLocker>   m_data_pool;
    LOG_RECORD                           * m_current_record;
    ThreadLocker                           m_current_record_locker;
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_ASYN_LOG_H
