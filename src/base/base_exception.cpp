/********************************************************
 * Description : exception classes
 * Data        : 2013-05-20 06:56:52
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include "base_exception.h"
#include "base_string.h"

NAMESPACE_COMMON_BEGIN

BaseException::BaseException(const char * file, int line, 
                             const char * func, const char * msg)
    : m_message()
{
    if (nullptr == file)
    {
        file = "unknown_file";
    }

    if (0 > line)
    {
        line = -1;
    }

    const size_t INT_LENGH_BOUND = 16;
    char line_info[INT_LENGH_BOUND] = { 0 };
    base_snprintf(line_info, INT_LENGH_BOUND, "%d", line);

    if (nullptr == func)
    {
        func = "unknown_func";
    }

    if (nullptr == msg)
    {
        msg = "";
    }

    m_message += file;
    m_message += ":";
    m_message += line_info;
    m_message += " ";
    m_message += func;
    m_message += " throw: ";
    m_message += msg;
}

const std::string & BaseException::message() const
{
    return(m_message);
}

NAMESPACE_COMMON_END
