/********************************************************
 * Description : hook of tcp manager
 * Data        : 2013-06-16 01:17:21
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_TCP_MANAGER_BASE_H
#define COMMON_NET_TCP_MANAGER_BASE_H


#include "base_common.h"
#include "net_common_types.h"
#include "net_connection_info.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL ITcpManager
{
public:
    ITcpManager() { }
    virtual ~ITcpManager() { }

public:
    virtual bool when_accept(socket_t sockfd, CONNECTION_INFO * send_buffer) = 0;
    virtual bool when_connect(socket_t sockfd, CONNECTION_INFO * send_buffer) = 0;
    virtual void when_send(socket_t sockfd) = 0;
    virtual void when_recv(socket_t sockfd, const char * data, int size) = 0;
    virtual void when_close(socket_t sockfd) = 0;

public:
    virtual bool send_message(connection_t connection, const char * data, int size) = 0;
};

NAMESPACE_COMMON_END


#endif // COMMON_NET_TCP_MANAGER_BASE_H
