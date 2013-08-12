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

#include <cassert>
#include <cstring>

#include "base_lazy_log.h"
#include "base_memory.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

LazyLog::LazyLog
(
    const std::string & path, 
    const std::string & log_type, 
    LOG_LEVEL min_log_level, 
    size_t max_file_size, 
    size_t max_record_count
)
    : BaseLog(path, log_type, min_log_level, max_file_size)
    , m_record(nullptr)
    , m_reserve(max_record_count)
    , m_begin(0)
    , m_count(0)
    , m_record_locker("record locker of lazy log")
{
    BASE_NEW_A(m_record, LOG_RECORD, max_record_count);
    assert(nullptr != m_record);
}

LazyLog::~LazyLog()
{
    BASE_DELETE_A(m_record);
}

void LazyLog::save_record(LOG_LEVEL log_level, const char * data, size_t size)
{
    BaseGuard<ThreadLocker> record_guard(m_record_locker);

    if (nullptr == m_record)
    {
        return;
    }

    if (LOG_RECORD_SIZE < size)
    {
        size = LOG_RECORD_SIZE;
    }

    size_t push_index = (m_begin + m_count) % m_reserve;
    memcpy(m_record[push_index].data, data, size);
    m_record[push_index].size = size;
    m_record[push_index].data[size - 1] = '\n';

    if (m_count < m_reserve)
    {
        ++m_count;
    }

    if (ERR_LEVEL >= log_level)
    {
        flush();
    }
}

void LazyLog::flush()
{
    for (size_t index = 0; index < m_count; ++index)
    {
        size_t record_index = (m_begin + index) % m_reserve;
        write(m_record[record_index].data, m_record[record_index].size);
    }
    m_begin = 0;
    m_count = 0;
}

NAMESPACE_COMMON_END
