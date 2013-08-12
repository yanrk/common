/********************************************************
 * Description : net common types
 * Data        : 2013-06-28 10:00:03
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_COMMON_TYPES_H
#define COMMON_NET_COMMON_TYPES_H


#ifdef WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif // WIN32

#include "base_common.h"

NAMESPACE_COMMON_BEGIN

enum POST_IO_TYPE
{
    POST_EXIT, 
    POST_ACCEPT, 
    POST_SEND, 
    POST_RECV
};

typedef uint32_t connection_t;

const connection_t BAD_CONNECTION = ~((connection_t)0);

#ifdef WIN32
    typedef SOCKET               socket_t;
    typedef SOCKADDR             sockaddr_t;
    typedef SOCKADDR_IN          sockaddr_in_t;
    #define BAD_SOCKET           (INVALID_SOCKET)
#else
    typedef int                  socket_t;
    typedef struct sockaddr      sockaddr_t;
    typedef struct sockaddr_in   sockaddr_in_t;
    #define BAD_SOCKET           (-1)
#endif // WIN32

const int            MAX_TCP_PKG_SIZE = 8192;
const unsigned short DEFAULT_TCP_PORT = 13579;
const char * const   DEFAULT_TCP_ADDR = "127.0.0.1";

NAMESPACE_COMMON_END


#endif // COMMON_NET_COMMON_TYPES_H
