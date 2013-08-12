/********************************************************
 * Description : string operator functions
 * Data        : 2013-05-18 20:41:33
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_STRING_H
#define COMMON_BASE_STRING_H


#include <cstdio>
#include <cstdarg>

#include "base_common.h"

NAMESPACE_COMMON_BEGIN

YRK_C_API(size_t) base_vsnprintf(
    char * buffer, 
    size_t bufsiz, 
    const char * fmt, 
    va_list args
);

YRK_C_API(size_t) base_snprintf(
    char * buffer, 
    size_t bufsiz, 
    const char * fmt, 
    ...
);

YRK_CXX_API(void) base_trim_head(
    std::string & str, 
    char trim = ' '
);

YRK_CXX_API(void) base_trim_tail(
    std::string & str, 
    char trim = ' '
);

YRK_CXX_API(void) base_trim(
    std::string & str, 
    char trim = ' '
);

YRK_C_API(int) base_strcmp_ignore_case(
    const char * str1, 
    const char * str2
);

YRK_C_API(int) base_strncmp_ignore_case(
    const char * str1, 
    const char * str2, 
    int count
);

YRK_C_API(bool) base_string2char  (const char * str, char   & val);
YRK_C_API(bool) base_string2short (const char * str, short  & val);
YRK_C_API(bool) base_string2int   (const char * str, int    & val);
YRK_C_API(bool) base_string2long  (const char * str, long   & val);
YRK_C_API(bool) base_string2float (const char * str, float  & val);
YRK_C_API(bool) base_string2double(const char * str, double & val);

YRK_CXX_API(bool) base_string2char  (const std::string & str, char   & val);
YRK_CXX_API(bool) base_string2short (const std::string & str, short  & val);
YRK_CXX_API(bool) base_string2int   (const std::string & str, int    & val);
YRK_CXX_API(bool) base_string2long  (const std::string & str, long   & val);
YRK_CXX_API(bool) base_string2float (const std::string & str, float  & val);
YRK_CXX_API(bool) base_string2double(const std::string & str, double & val);

YRK_C_API(bool) base_char2string  (char   val, char * str, int size);
YRK_C_API(bool) base_short2string (short  val, char * str, int size);
YRK_C_API(bool) base_int2string   (int    val, char * str, int size);
YRK_C_API(bool) base_long2string  (long   val, char * str, int size);
YRK_C_API(bool) base_float2string (float  val, char * str, int size);
YRK_C_API(bool) base_double2string(double val, char * str, int size);

YRK_CXX_API(bool) base_char2string  (char   val, std::string & str);
YRK_CXX_API(bool) base_short2string (short  val, std::string & str);
YRK_CXX_API(bool) base_int2string   (int    val, std::string & str);
YRK_CXX_API(bool) base_long2string  (long   val, std::string & str);
YRK_CXX_API(bool) base_float2string (float  val, std::string & str);
YRK_CXX_API(bool) base_double2string(double val, std::string & str);

NAMESPACE_COMMON_END


#endif // COMMON_BASE_STRING_H
