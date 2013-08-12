/********************************************************
 * Description : linux tcp connection class
 * Data        : 2013-06-12 21:21:35
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_LINUX_TCP_CONNECTION_H
#define COMMON_NET_LINUX_TCP_CONNECTION_H


#ifndef WIN32


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <map>
#include <list>
#include <vector>

#include "base_locker.h"
#include "base_object_pool.hpp"
#include "net_connection_info.h"
#include "net_tcp_manager_base.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL LinuxTcpConnection
{
public:
    LinuxTcpConnection();
    ~LinuxTcpConnection();

public:
    bool init();
    bool exit();

public:
    void set_manager(ITcpManager * manager);
    void set_local_server_port(unsigned short host_port);
    bool create_connection(const char * host_name, unsigned short host_port);
    bool create_connection(sockaddr_in_t & server_address);
    bool destroy_connection(socket_t sockfd);
    bool transform_address(const char * host_name, 
                           unsigned short host_port, 
                           sockaddr_in_t & server_address);

public:
    bool post_send(CONNECTION_INFO * epoll_data);
    bool stop_send(CONNECTION_INFO * epoll_data);

public:
    void main_thread_run();
    void sub_thread_run(int thread_index);

private:
    void do_accept(socket_t listenfd);
    bool do_send(CONNECTION_INFO * epoll_data);
    bool do_recv(CONNECTION_INFO * epoll_data);
    void do_close(CONNECTION_INFO * epoll_data, bool invoke);

private:
    bool on_accept(socket_t sockfd, CONNECTION_INFO * epoll_data);
    bool on_connect(socket_t sockfd, CONNECTION_INFO * epoll_data);
    void on_send(socket_t sockfd);
    void on_recv(socket_t sockfd, const char * data, int size);
    void on_close(socket_t sockfd);

private:
    bool running() const;
    bool create_epoll();
    bool bind_epoll_event(CONNECTION_INFO * epoll_data);
    bool delete_epoll_event(CONNECTION_INFO * epoll_data);
    bool modify_epoll_event(CONNECTION_INFO * epoll_data, bool send, bool recv);
    bool create_listen_socket();
    bool create_work_threads();
    socket_t create_sockfd(sockaddr_in_t & server_address);
    bool register_connection(CONNECTION_INFO * epoll_data);
    bool unregister_connection(socket_t sockfd);
    bool set_nonblock(socket_t sockfd);

private:
    bool                                           m_running;
    unsigned short                                 m_port;
    int                                            m_epoll;
    pthread_t                                    * m_thread;
    int                                            m_thread_count;

    ObjectPool<CONNECTION_INFO, ThreadLocker>      m_epoll_data_pool;

    struct epoll_event                             m_listen_epoll_event;

    std::map<socket_t, CONNECTION_INFO *>          m_epoll_data_map;
    ThreadLocker                                   m_epoll_data_locker;

    std::vector< std::list<struct epoll_event> >   m_active_epoll_events;
    ThreadLocker                                   m_active_epoll_events_locker;

    ITcpManager                                  * m_manager;
};

NAMESPACE_COMMON_END


#endif // WIN32


#endif // COMMON_NET_LINUX_TCP_CONNECTION_H
