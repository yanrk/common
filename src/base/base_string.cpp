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

#include <cstring>

#include "base_string.h"

NAMESPACE_COMMON_BEGIN

size_t base_vsnprintf(char * buffer, size_t bufsiz, const char * fmt, va_list args)
{
    if (nullptr == buffer || 0 == bufsiz || nullptr == fmt || nullptr == args)
    {
        return(0);
    }

#ifdef WIN32
    size_t ret_siz = _vsnprintf(buffer, bufsiz, fmt, args);
#else
    size_t ret_siz = vsnprintf(buffer, bufsiz, fmt, args);
#endif

    if (bufsiz <= ret_siz)
    {
        ret_siz = bufsiz - 1;
        buffer[ret_siz] = '\0';
    }

    return(ret_siz);
}

size_t base_snprintf(char * buffer, size_t bufsiz, const char * fmt, ...)
{
    if (nullptr == fmt)
    {
        return(0);
    }

    va_list args = nullptr;

    va_start(args, fmt);

    size_t ret_siz = base_vsnprintf(buffer, bufsiz, fmt, args);

    va_end(args);

    return(ret_siz);
}

void base_trim_head(std::string & str, char trim)
{
    std::string::size_type h_pos = str.find_first_not_of(trim);
    if (std::string::npos == h_pos)
    {
        str.clear();
    }
    else
    {
        std::string(str.begin() + h_pos, str.end()).swap(str);
    }
}

void base_trim_tail(std::string & str, char trim)
{
    std::string::size_type t_pos = str.find_last_not_of(trim);
    if (std::string::npos == t_pos)
    {
        str.clear();
    }
    else
    {
        std::string(str.begin(), str.begin() + t_pos + 1).swap(str);
    }
}

void base_trim(std::string & str, char trim)
{
    std::string::size_type h_pos = str.find_first_not_of(trim);
    if (std::string::npos == h_pos)
    {
        str.clear();
    }
    else
    {
        std::string::size_type t_pos = str.find_last_not_of(trim);
        std::string(str.begin() + h_pos, str.begin() + t_pos + 1).swap(str);
    }
}

int base_strcmp_ignore_case(const char * str1, const char * str2)
{
#ifdef WIN32
    return(stricmp(str1, str2));
#else
    return(strcasecmp(str1, str2));
#endif // WIN32
}

int base_strncmp_ignore_case(const char * str1, const char * str2, int count)
{
#ifdef WIN32
    return(strnicmp(str1, str2, count));
#else
    return(strncasecmp(str1, str2, count));
#endif // WIN32
}

#define CSTRING_2_TYPE(type, fmt)                              \
bool base_string2##type(const char * str, type & val)          \
{                                                              \
    if (nullptr == str)                                        \
    {                                                          \
        return(false);                                         \
    }                                                          \
    return(sscanf(str, fmt, &val) > 0);                        \
}

#define STRING_2_TYPE(type, fmt)                               \
bool base_string2##type(const std::string & str, type & val)   \
{                                                              \
    return(sscanf(str.c_str(), fmt, &val) > 0);                \
}

#define TYPE_2_CSTRING(type, fmt)                              \
bool base_##type##2string(type val, char * str, int size)      \
{                                                              \
    return(base_snprintf(str, size, fmt, val) > 0);            \
}

#define TYPE_2_STRING(type, fmt) \
bool base_##type##2string(type val, std::string & str)         \
{                                                              \
    const int bufsiz = 64;                                     \
    char buffer[bufsiz];                                       \
    base_##type##2string(val, buffer, bufsiz);                 \
    str = buffer;                                              \
    return(true);                                              \
}

CSTRING_2_TYPE(  char,  "%c")
CSTRING_2_TYPE( short, "%hd")
CSTRING_2_TYPE(   int,  "%d")
CSTRING_2_TYPE(  long, "%ld")
CSTRING_2_TYPE( float,  "%f")
CSTRING_2_TYPE(double, "%lf")

STRING_2_TYPE(   char,  "%c")
STRING_2_TYPE(  short, "%hd")
STRING_2_TYPE(    int,  "%d")
STRING_2_TYPE(   long, "%ld")
STRING_2_TYPE(  float,  "%f")
STRING_2_TYPE( double, "%lf")

TYPE_2_CSTRING(  char,  "%c")
TYPE_2_CSTRING( short, "%hd")
TYPE_2_CSTRING(   int,  "%d")
TYPE_2_CSTRING(  long, "%ld")
TYPE_2_CSTRING( float,  "%f")
TYPE_2_CSTRING(double,  "%f")

TYPE_2_STRING(   char,  "%c")
TYPE_2_STRING(  short, "%hd")
TYPE_2_STRING(    int,  "%d")
TYPE_2_STRING(   long, "%ld")
TYPE_2_STRING(  float,  "%f")
TYPE_2_STRING( double,  "%f")

NAMESPACE_COMMON_END
