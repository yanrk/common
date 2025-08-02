/********************************************************
 * Description : convert between string and build-in type
 * Data        : 2013-08-03 22:26:44
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_CONVERT_H
#define COMMON_BASE_CONVERT_H


#include <string>
#include <sstream>

#include "base_common.h"

NAMESPACE_COMMON_BEGIN

template <typename T>
bool base_string2type(const std::string & str, T & val)
{
    std::istringstream iss(str);
    iss.setf(std::ios::boolalpha);
    iss >> val;
    return !iss.fail();
}

template <typename T>
bool base_type2string(T val, std::string & str)
{
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss.setf(std::ios::boolalpha);
    oss << val;
    str = oss.str();
    return true;
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_CONVERT_H
