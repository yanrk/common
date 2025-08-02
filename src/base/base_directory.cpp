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

#include "base_directory.h"

NAMESPACE_COMMON_BEGIN

bool base_opendir(const char * dirname, BASE_DIR & base_dir)
{
    if (nullptr == dirname || '\0' == dirname[0])
    {
        return false;
    }

    base_dir.dir_name = dirname;
    if ('/' != *base_dir.dir_name.rbegin())
    {
        base_dir.dir_name += "/";
    }

#ifdef WIN32
    base_dir.file_name = base_dir.dir_name + "*";
    base_dir.dir = 
        FindFirstFileA(base_dir.file_name.c_str(), &base_dir.file);
    if (INVALID_HANDLE_VALUE == base_dir.dir)
    {
        return false;
    }
    base_dir.eof = false;
#else
    base_dir.dir = opendir(dirname);
    if (nullptr == base_dir.dir)
    {
        return false;
    }
    base_dir.file = readdir(base_dir.dir);
#endif // WIN32

    return true;
}

bool base_is_opendir(const BASE_DIR & base_dir)
{
#ifdef WIN32
    return INVALID_HANDLE_VALUE != base_dir.dir;
#else
    return nullptr != base_dir.dir;
#endif // WIN32
}

void base_closedir(BASE_DIR & base_dir)
{
    if (!base_is_opendir(base_dir))
    {
        return;
    }

#ifdef WIN32
    FindClose(base_dir.dir);
    base_dir.dir = INVALID_HANDLE_VALUE;
#else
    closedir(base_dir.dir);
    base_dir.dir = nullptr;
#endif // WIN32
}

bool base_readdir(BASE_DIR & base_dir)
{
    if (!base_is_opendir(base_dir))
    {
        return false;
    }

#ifdef WIN32
    while (!base_dir.eof)
    {
        WIN32_FIND_DATAA file = base_dir.file;
        base_dir.eof = !FindNextFileA(base_dir.dir, &base_dir.file);

        if (0 != strcmp(file.cFileName, ".") && 
            0 != strcmp(file.cFileName, ".."))
        {
            base_dir.file_name = base_dir.dir_name + file.cFileName;
            base_dir.file_is_dir = 
                (0 != (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));

            return true;
        }
    }
#else
    while (nullptr != base_dir.file)
    {
        struct dirent * file = base_dir.file;
        base_dir.file = readdir(base_dir.dir);

        if (0 != strcmp(file->d_name, ".") && 
            0 != strcmp(file->d_name, ".."))
        {
            base_dir.file_name = base_dir.dir_name + file->d_name;
            base_dir.file_is_dir = false;

            struct stat stat_buf;
            if (-1 != lstat(base_dir.file_name.c_str(), &stat_buf) && 
                0 != (S_IFDIR & stat_buf.st_mode))
            {
                base_dir.file_is_dir = true;
            }

            return true;
        }
    }
#endif // WIN32

    return false;
}

NAMESPACE_COMMON_END
