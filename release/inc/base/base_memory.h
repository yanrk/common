/********************************************************
 * Description : alloc and free memory macro
 * Data        : 2013-05-18 18:00:25
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_MEMORY_H
#define COMMON_BASE_MEMORY_H


#include "base_exception.h"

#define BASE_NEW(var, type)           \
do                                    \
{                                     \
    try                               \
    {                                 \
        (var) = new type;             \
    }                                 \
    catch (YRK_CMN::BaseException &)  \
    {                                 \
        (var) = nullptr;              \
        throw;                        \
    }                                 \
    catch (...)                       \
    {                                 \
        (var) = nullptr;              \
    }                                 \
} while (false)

#define BASE_NEW_A(var, type, size)   \
do                                    \
{                                     \
    try                               \
    {                                 \
        (var) = new type[size];       \
    }                                 \
    catch (YRK_CMN::BaseException &)  \
    {                                 \
        (var) = nullptr;              \
        throw;                        \
    }                                 \
    catch (...)                       \
    {                                 \
        (var) = nullptr;              \
    }                                 \
} while (false)

#define BASE_DELETE(var)              \
do                                    \
{                                     \
    try                               \
    {                                 \
        delete (var);                 \
    }                                 \
    catch (YRK_CMN::BaseException &)  \
    {                                 \
        (var) = nullptr;              \
        throw;                        \
    }                                 \
    catch (...)                       \
    {                                 \
                                      \
    }                                 \
                                      \
    (var) = nullptr;                  \
} while (false)

#define BASE_DELETE_A(var)            \
do                                    \
{                                     \
    try                               \
    {                                 \
        delete [] (var);              \
    }                                 \
    catch (YRK_CMN::BaseException &)  \
    {                                 \
        (var) = nullptr;              \
        throw;                        \
    }                                 \
    catch (...)                       \
    {                                 \
                                      \
    }                                 \
                                      \
    (var) = nullptr;                  \
} while (false)


#endif // COMMON_BASE_MEMORY_H
