/********************************************************
 * Description : tcp manager
 * Data        : 2013-06-16 01:34:45
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_TCP_MANAGER_H
#define COMMON_NET_TCP_MANAGER_H


#include <deque>
#include <map>

#include "net_message_handler_base.h"
#include "net_tcp_manager_base.h"
#include "net_tcp_connection.h"
#include "net_data_handler.h"

#include "base_locker.h"
#include "base_object_pool.hpp"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL TcpManager : public ITcpManager, public NetDataHandler
{
public:
    TcpManager();
    virtual ~TcpManager();

public:
    bool start();
    void stop();

public:
    bool set_local_port(unsigned short host_port);
    bool set_remote_address(const char * host_name, unsigned short host_port);
    bool set_data_handle_thread_count(int thread_count);
    bool set_message_handler(IMessageHandler * message_handler);
    bool create_connection();
    bool create_connection(const char * host_name, unsigned short host_port);
    bool create_connection(sockaddr_in_t & server_address);
    bool destroy_connection(connection_t connection);

public:
    virtual bool send_message(connection_t connection, const char * data, int size);

private:
    virtual bool when_accept(socket_t sockfd, CONNECTION_INFO * send_buffer);
    virtual bool when_connect(socket_t sockfd, CONNECTION_INFO * send_buffer);
    virtual void when_send(socket_t sockfd);
    virtual void when_recv(socket_t sockfd, const char * data, int size);
    virtual void when_close(socket_t sockfd);

private:
    virtual bool handle_data(socket_t sockfd, const char * data, int size);

private:
    virtual bool continue_send(CONNECTION_INFO * send_buffer);
    virtual bool suspend_send(CONNECTION_INFO * send_buffer);

private:
    connection_t acquire_first_connection();
    connection_t acquire_next_connection();
    bool transform_socket(socket_t sockfd, connection_t & connection);
    bool transform_connection(connection_t connection, socket_t & sockfd);
    void add_connection(socket_t sockfd, connection_t connection);
    void del_connection(socket_t sockfd, connection_t connection);
    void del_socket(socket_t sockfd);
    void del_connection(connection_t connection);
    bool connect_notice(socket_t sockfd, CONNECTION_INFO * send_buffer, 
                        bool accept);

private:
    bool                               m_start;
    TcpConnection                      m_tcp_connection;
    sockaddr_in_t                      m_remote_server_address;

    connection_t                       m_next_connection;
    std::deque<connection_t>           m_using_connection;
    ThreadLocker                       m_deque_locker;

    std::map<socket_t, connection_t>   m_sock2connection;
    std::map<connection_t, socket_t>   m_connection2sock;
    ThreadLocker                       m_map_locker;

    IMessageHandler                  * m_message_handler;
};

NAMESPACE_COMMON_END


#endif // COMMON_NET_TCP_MANAGER_H
