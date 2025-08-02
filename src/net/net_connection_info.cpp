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

#include <cstring>

#include "net_connection_info.h"

NAMESPACE_COMMON_BEGIN

#ifdef WIN32

void CONNECTION_INFO::reset(enum POST_IO_TYPE post_io_type)
{
    memset(&overlapped, 0, sizeof(OVERLAPPED));
    sockfd = INVALID_SOCKET;
    wsa_buffer.buf = buffer;
    buffer_size = MAX_TCP_PKG_SIZE;
    post_type = post_io_type;
}

void CONNECTION_INFO::accept_reset()
{
    reset(POST_ACCEPT);
    wsa_buffer.len = buffer_size;
}

void CONNECTION_INFO::send_reset()
{
    reset(POST_SEND);
    wsa_buffer.len = 0;
}

void CONNECTION_INFO::recv_reset()
{
    reset(POST_RECV);
    wsa_buffer.len = buffer_size;
}

bool CONNECTION_INFO::empty()
{
    return 0 == wsa_buffer.len;
}

bool CONNECTION_INFO::write_data(const char * data, int size)
{
    if (0 == size)
    {
        return true;
    }

    if (NULL == data || size > buffer_size || 
        0 > size || !empty())
    {
        return false;
    }

    memcpy(wsa_buffer.buf, data, size);
    wsa_buffer.len = size;

    return true;
}

void CONNECTION_INFO::close_socket()
{
    if (INVALID_SOCKET != sockfd)
    {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
}

#else // WIN32

void CONNECTION_INFO::reset()
{
    sockfd = -1;
    buffer_size = MAX_TCP_PKG_SIZE;
    send_len = 0;
    send_left = 0;
}

bool CONNECTION_INFO::empty()
{
    return 0 == send_left;
}

bool CONNECTION_INFO::write_data(const char * data, int size)
{
    if (0 == size)
    {
        return true;
    }

    if (NULL == data || size > buffer_size || 
        0 > size || !empty())
    {
        return false;
    }

    memcpy(send_buffer, data, size);
    send_len = 0;
    send_left = size;

    return true;
}

void CONNECTION_INFO::close_socket()
{
    if (-1 != sockfd)
    {
        close(sockfd);
        sockfd = -1;
    }
}

#endif // WIN32

NAMESPACE_COMMON_END
