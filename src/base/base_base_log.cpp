/********************************************************
 * Description : base class of log classes
 * Data        : 2013-05-23 09:24:56
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include <fstream>

#include "base_base_log.h"
#include "base_io.h"
#include "base_string.h"
#include "base_time.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

BaseLog::BaseLog
(
    const std::string & path, 
    const std::string & log_type, 
    LOG_LEVEL min_log_level, 
    size_t max_file_size
)
    : m_path(path + "/" + log_type + "/")
    , m_log_type(log_type)
    , m_fullname(m_path + m_log_type + ".log")
    , m_file(-1)
    , m_min_log_level(min_log_level)
    , m_cur_file_size(0)
    , m_max_file_size(max_file_size * 1024 * 1024)
    , m_file_locker("file locker of base log")
{
    create_path();
    open();
}


BaseLog::~BaseLog()
{
    close();
}

void BaseLog::create_path()
{
    std::string::size_type pos = 0;
    while (std::string::npos != (pos = m_path.find_first_of("/\\", pos)))
    {
        pos += 1;
        std::string dir(m_path, 0, pos);
        base_mkdir(dir.c_str());
    }
    base_mkdir(m_path.c_str());
}

void BaseLog::open()
{
    close();
    base_umask();
    m_file = base_open(m_fullname.c_str(), O_WRONLY | O_CREAT | O_APPEND);
    update_file_size();
}

void BaseLog::close()
{
    if (-1 != m_file)
    {
        base_close(m_file);
        m_file = -1;
    }
}

void BaseLog::rename()
{
    std::ifstream ifs(m_fullname);
    if (!ifs.is_open())
    {
        return;
    }

    const size_t LINE_MAX_SIZE = 2048;

    ifs.seekg(0, std::ios::end);
    size_t len = static_cast<size_t>(ifs.tellg());

    size_t offset = 0;
    if (LINE_MAX_SIZE < len)
    {
        offset = len - LINE_MAX_SIZE;
    }
    ifs.seekg(offset, std::ios::beg);

    std::string date;
    char line[LINE_MAX_SIZE];
    while(!ifs.eof())
    {
        ifs.getline(line, LINE_MAX_SIZE);
        size_t size = static_cast<size_t>(ifs.gcount());
        line[size] = '\0';
        parse_date(line, size + 1, date);
    }

    ifs.close();

    if (date.empty())
    {
        char buffer[32] = { 0 };
        tm tm_now = base_get_localtime();
        size_t buf_len = base_snprintf
        (
            buffer, sizeof(buffer) / sizeof(buffer[0]), 
            "%4d-%02d-%02d %02d:%02d:%02d", 
            tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, 
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec
        );
        std::string(buffer, buffer + buf_len).swap(date);
    }

    std::string new_name(m_path + m_log_type + "_" + date + ".log");
    if (0 != base_rename(m_fullname.c_str(), new_name.c_str()))
    {
        std::ofstream(new_name) << ifs.rdbuf();
        std::ofstream(m_fullname, std::ios::trunc);
    }
}

void BaseLog::update_file_size()
{
    std::ifstream ifs(m_fullname);
    if (!ifs.is_open())
    {
        return;
    }
    ifs.seekg(0, std::ios::end);
    m_cur_file_size = static_cast<size_t>(ifs.tellg());
    ifs.close();
}

void BaseLog::parse_date(const char * record, size_t size, std::string & date)
{
    if (nullptr == record || 20 > size)
    {
        return;
    }

    int year   = 0;
    int month  = 0;
    int day    = 0;
    int hour   = 0;
    int minute = 0;
    int second = 0;

    int ret = sscanf(record, "%d-%d-%d %d:%d:%d", 
                     &year, &month, &day, &hour, &minute, &second);

    if (6 != ret || 1900 > year || 0 >= month || 12 < month || 
        0 >= day || 31 < day || 0 > hour || 24 <= hour || 
        0 > minute || 60 <= minute || 0 > second || 60 <= second)
    {
        return;
    }

    char date_temp[19] = { 0 };
    size_t temp_size = 0;
    for (size_t index = 0; index < 19; ++index)
    {
        if ('0' <= record[index] && '9' >= record[index])
        {
            date_temp[temp_size] = record[index];
            ++temp_size;
        }
    }
    std::string(date_temp, date_temp + temp_size).swap(date);
}

void BaseLog::push_record
(
    LOG_LEVEL level, const char * file, const char * func, 
    size_t line, const char * format, va_list args
)
{
    IF_VAR_EQUAL_VALUE_RETURN_VOID(file, nullptr);
    IF_VAR_EQUAL_VALUE_RETURN_VOID(func, nullptr);
    IF_VAR_EQUAL_VALUE_RETURN_VOID(format, nullptr);
    IF_VAR_EQUAL_VALUE_RETURN_VOID(args, nullptr);

    if (level > m_min_log_level)
    {
        return;
    }

    static const char * const WEEK[7] = 
    {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    char record[LOG_RECORD_SIZE];
    size_t record_size = 0;

    tm tm_now = base_get_localtime();
    struct timeval tv_now = base_gettimeofday();
    int millisecond = static_cast<int>(tv_now.tv_usec / 1000);
    int time_zone = base_get_timezone();
    int day_of_week = base_get_day_of_week();

    record_size += base_snprintf
    (
        record + record_size, LOG_RECORD_SIZE - record_size, 
        "%4d-%02d-%02d %02d:%02d:%02d.%03d GMT%+03d:00 %s", 
        tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, 
        tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, millisecond, 
        time_zone, WEEK[day_of_week]
    );

    record_size += base_snprintf
    (
        record + record_size, LOG_RECORD_SIZE - record_size, 
        " | %s | %s:%s:%05d | ", 
        LOG_LEVEL_INFO[level], file, func, line
    );

    record_size += base_vsnprintf
    (
        record + record_size, LOG_RECORD_SIZE - record_size, 
        format, args
    );

    record_size += 1; /* for '\n' */
    if (LOG_RECORD_SIZE < record_size)
    {
        record_size = LOG_RECORD_SIZE;
    }
    record[record_size - 1] = '\n';

    save_record(level, record, record_size);
}

void BaseLog::push_record
(
    LOG_LEVEL level, const char * file, const char * func, 
    size_t line, const char * format, ...
)
{
    va_list args = nullptr;

    va_start(args, format);

    push_record(level, file, func, line, format, args);

    va_end(args);
}

void BaseLog::set_min_log_level(LOG_LEVEL level)
{
    m_min_log_level = level;
}

void BaseLog::write(const char * data, size_t size)
{
    BaseGuard<ThreadLocker> file_guard(m_file_locker);

    if (-1 == m_file || -1 == base_write(m_file, data, size))
    {
        return;
    }

    m_cur_file_size += size;
    if (m_cur_file_size < m_max_file_size)
    {
        return;
    }

    close();
    rename();
    open();
}

NAMESPACE_COMMON_END
