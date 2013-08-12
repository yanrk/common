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

#ifndef COMMON_BASE_BASE_LOG_H
#define COMMON_BASE_BASE_LOG_H


#include <cstdarg>
#include <string>
#include <fstream>

#include "base_log_types.h"
#include "base_uncopy.h"
#include "base_locker.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL BaseLog : private BaseUncopy
{
public:
    BaseLog
    (
        const std::string & path, 
        const std::string & log_type, 
        LOG_LEVEL min_log_level, 
        size_t max_file_size
    );

    virtual ~BaseLog();

public:
    void push_record
    (
        LOG_LEVEL level, const char * file, const char * func, 
        size_t line, const char * format, va_list args
    );

    void push_record
    (
        LOG_LEVEL level, const char * file, const char * func, 
        size_t line, const char * format, ...
    );

    void set_min_log_level(LOG_LEVEL level);

protected:
    virtual void save_record
    (
        LOG_LEVEL log_level, 
        const char * data, 
        size_t size
    ) = 0;

    void write(const char * data, size_t size);

private:
    void create_path();
    void open();
    void close();
    void rename();
    void update_file_size();

private:
    void parse_date(const char *, size_t, std::string &);

private:
    std::string  m_path;
    std::string  m_log_type;
    std::string  m_fullname;
    int          m_file;
    LOG_LEVEL    m_min_log_level;
    size_t       m_cur_file_size;
    size_t       m_max_file_size;
    ThreadLocker m_file_locker;
};

NAMESPACE_COMMON_END


#endif // COMMON_BASE_BASE_LOG_H
