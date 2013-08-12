/********************************************************
 * Description : macro of standard I/O
 * Data        : 2013-05-23 11:46:45
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_IO_H
#define COMMON_BASE_IO_H


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
    #include <io.h>
    #include <direct.h>
#else
    #include <unistd.h>
#endif // WIN32

#ifdef WIN32
    #define base_umask()
    #define base_open(file, mode) ::_open(file, mode, S_IREAD | S_IWRITE)
    #define base_close            ::_close
    #define base_lseek            ::_lseek
    #define base_read             ::_read
    #define base_write            ::_write
    #define base_stat             ::_stat
    #define base_STAT             struct _stat
    #define base_access           ::_access
    #define base_mkdir(dir)       ::_mkdir(dir)
    #define base_rename           ::rename
    #define base_unlink           ::_unlink
    #define base_sync             ::_commit
#else
    #define base_umask()          ::umask(S_IWGRP | S_IWOTH)
    #define base_open(file, mode) ::open(file, mode, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
    #define base_close            ::close
    #define base_lseek            ::lseek
    #define base_read             ::read
    #define base_write            ::write
    #define base_stat             ::stat
    #define base_STAT             struct stat
    #define base_access           ::access
    #define base_mkdir(dir)       ::mkdir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
    #define base_rename           ::rename
    #define base_unlink           ::unlink
    #define base_sync             ::fsync
#endif


#endif // COMMON_BASE_IO_H
