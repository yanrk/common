/********************************************************
 * Description : functions of directory traversal
 * Data        : 2013-06-06 14:46:16
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_DIRECTORY_H
#define COMMON_BASE_DIRECTORY_H


#ifdef WIN32
    #include <windows.h>
#else
    #include <dirent.h>
    #include <sys/stat.h>
#endif // WIN32

#include <cstring>
#include <string>

#include "base_common.h"

NAMESPACE_COMMON_BEGIN

struct BASE_DIR
{
#ifdef WIN32
    HANDLE               dir;
    WIN32_FIND_DATAA     file;
    bool                 eof;
#else
    DIR                * dir;
    struct dirent      * file;
#endif // WIN32

    /* we can use the below members by read-only */
    bool                 file_is_dir;
    std::string          dir_name;
    std::string          file_name;
};

YRK_C_API(bool) base_opendir(const char * dirname, BASE_DIR & base_dir);

YRK_C_API(bool) base_is_opendir(const BASE_DIR & base_dir);

YRK_C_API(void) base_closedir(BASE_DIR & base_dir);

/* this function will ignore return-directory "." and ".." while traversing */
YRK_C_API(bool) base_readdir(BASE_DIR & base_dir);

NAMESPACE_COMMON_END


#endif // COMMON_BASE_DIRECTORY_H
