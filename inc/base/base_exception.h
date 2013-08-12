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

#ifndef COMMON_BASE_EXCEPTION_H
#define COMMON_BASE_EXCEPTION_H


#include <string>

#include "base_common.h"
#include "base_string.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL BaseException
{
public:
    BaseException(const char * file, int line, const char * func, const char * msg);

public:
    const std::string & message() const;

protected:
    std::string m_message;
};


#define BASE_THROW(fmt, ...)                                           \
do                                                                     \
{                                                                      \
    const size_t MAX_EXCEPTION_MSG_LENGTH = 1024;                      \
    char msg[MAX_EXCEPTION_MSG_LENGTH] = { 0 };                        \
    base_snprintf(msg, MAX_EXCEPTION_MSG_LENGTH, fmt, ##__VA_ARGS__);  \
    throw BaseException(__FILE__, __LINE__, __FUNCTION__, msg);        \
} while (false)

NAMESPACE_COMMON_END


#endif // COMMON_BASE_EXCEPTION_H
