/********************************************************
 * Description : windows tcp connection class
 * Data        : 2013-06-10 22:38:15
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_WINDOWS_TCP_CONNECTION_H
#define COMMON_NET_WINDOWS_TCP_CONNECTION_H


#ifdef WIN32


#include <winsock2.h>
#include <mswsock.h>

#include <list>
#include <map>

#include "base_locker.h"
#include "base_object_pool.hpp"
#include "net_connection_info.h"
#include "net_tcp_manager_base.h"

NAMESPACE_COMMON_BEGIN

struct COMPLETION_INFO
{
    socket_t                     sockfd;
    bool                         server;
    sockaddr_in_t                remote_address;
    std::list<CONNECTION_INFO *> overlapped_info_list;

    COMPLETION_INFO();

    void close_socket();
};

class YRK_EXPORT_DLL WindowsTcpConnection
{
public:
    WindowsTcpConnection();
    virtual ~WindowsTcpConnection();

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
    bool post_send(CONNECTION_INFO * over_info);
    bool stop_send(CONNECTION_INFO * over_info);

public:
    void thread_run(int thread_index);

private:
    void do_accept(CONNECTION_INFO * over_info, int accept_size);
    void do_send(CONNECTION_INFO * over_info, int send_size);
    void do_recv(CONNECTION_INFO * over_info, int recv_size);
    void do_close(COMPLETION_INFO * comp_info);

private:
    bool on_accept(socket_t sockfd, CONNECTION_INFO * send_over_info);
    bool on_connect(socket_t sockfd, CONNECTION_INFO * send_over_info);
    void on_send(socket_t sockfd);
    void on_recv(socket_t sockfd, const char * data, int size);
    void on_close(socket_t sockfd);

private:
    bool running() const;
    bool try_do_accept(CONNECTION_INFO * over_info, int accept_size);
    void handle_error(COMPLETION_INFO * comp_info, OVERLAPPED * overlapped);
    bool post_accept(CONNECTION_INFO * over_info);
    bool post_recv(CONNECTION_INFO * over_info);
    bool post_exit();
    bool create_completion_port();
    bool bind_to_completion_port(COMPLETION_INFO * comp_info);
    bool get_extern_wsa_functions();
    bool create_listen_socket();
    bool create_work_threads();
    socket_t create_sockfd(sockaddr_in_t & server_address);
    bool acquire_completion(socket_t sockfd, 
                            COMPLETION_INFO *& comp_info, 
                            CONNECTION_INFO *& send_over_info, 
                            CONNECTION_INFO *& recv_over_info);
    bool register_connection(COMPLETION_INFO * comp_info);
    bool unregister_connection(socket_t sockfd);

private:
    bool                                        m_running;
    unsigned short                              m_port;
    HANDLE                                      m_iocp;
    HANDLE                                    * m_thread;
    int                                         m_thread_count;

    ObjectPool<COMPLETION_INFO, ThreadLocker>   m_comp_info_pool;
    ObjectPool<CONNECTION_INFO, ThreadLocker>   m_over_info_pool;

    COMPLETION_INFO                           * m_listen_comp_info;

    std::map<socket_t, COMPLETION_INFO *>       m_comp_info_map;
    ThreadLocker                                m_comp_info_locker;

    LPFN_ACCEPTEX                               m_acceptex_func;
    LPFN_GETACCEPTEXSOCKADDRS                   m_get_acceptex_sock_addrs_func;

    ITcpManager                               * m_manager;
};

NAMESPACE_COMMON_END


#endif // WIN32


#endif // COMMON_NET_WINDOWS_TCP_CONNECTION_H
