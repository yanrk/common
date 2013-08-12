/********************************************************
 * Description : net connection infomation struction
 * Data        : 2013-06-28 11:26:16
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_CONNECTION_INFOMATION_H
#define COMMON_NET_CONNECTION_INFOMATION_H


#include "net_common_types.h"

NAMESPACE_COMMON_BEGIN

#ifdef WIN32

typedef struct CONNECTION_INFO
{
public:
    OVERLAPPED     overlapped;
    socket_t       sockfd;
    WSABUF         wsa_buffer;
    char           buffer[MAX_TCP_PKG_SIZE];
    int            buffer_size;
    POST_IO_TYPE   post_type;

    void accept_reset();
    void send_reset();
    void recv_reset();

    bool empty();
    bool write_data(const char * data, int size);
    void close_socket();

private:
    void reset(enum POST_IO_TYPE post_io_type);
} OVERLAPPED_INFO;

#else // WIN32

typedef struct CONNECTION_INFO
{
    socket_t        sockfd;
    bool            server;
    sockaddr_in_t   remote_address;
    char            send_buffer[MAX_TCP_PKG_SIZE];
    char            recv_buffer[MAX_TCP_PKG_SIZE];
    int             buffer_size;
    int             send_len;
    int             send_left;

    void reset();
    bool empty();
    bool write_data(const char * data, int size);
    void close_socket();
} EPOLL_DATA;

#endif // WIN32

NAMESPACE_COMMON_END


#endif // COMMON_NET_CONNECTION_INFOMATION_H
