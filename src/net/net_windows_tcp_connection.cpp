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

#ifdef WIN32


#include <process.h>

#include "net_windows_tcp_connection.h"
#include "base_memory.h"
#include "base_log.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

COMPLETION_INFO::COMPLETION_INFO()
    : sockfd(INVALID_SOCKET)
{

}

void COMPLETION_INFO::close_socket()
{
    if (INVALID_SOCKET != sockfd)
    {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
    }
}

struct IOCP_WORK_THREAD_PARAM
{
    WindowsTcpConnection & tcp_connection;
    int                    thread_index;

    IOCP_WORK_THREAD_PARAM(WindowsTcpConnection & connection, int index);
};

IOCP_WORK_THREAD_PARAM::IOCP_WORK_THREAD_PARAM(WindowsTcpConnection & connection, 
                                               int index)
    : tcp_connection(connection), thread_index(index)
{

}

unsigned int __stdcall work_thread_run(void * argu)
{
    IOCP_WORK_THREAD_PARAM * thread_argu = 
        reinterpret_cast<IOCP_WORK_THREAD_PARAM *>(argu);
    if (nullptr == thread_argu)
    {
        return 1;
    }

    thread_argu->tcp_connection.thread_run(thread_argu->thread_index);

    BASE_DELETE(thread_argu);

    return 0;
}

WindowsTcpConnection::WindowsTcpConnection()
    : m_running(false)
    , m_port(0)
    , m_iocp(nullptr)
    , m_thread(nullptr)
    , m_thread_count(0)
    , m_comp_info_pool("comp info pool")
    , m_over_info_pool("over info pool")
    , m_listen_comp_info(nullptr)
    , m_comp_info_map()
    , m_comp_info_locker("comp info map locker")
    , m_acceptex_func(nullptr)
    , m_get_acceptex_sock_addrs_func(nullptr)
    , m_manager(nullptr)
{

}

WindowsTcpConnection::~WindowsTcpConnection()
{
    exit();
}

bool WindowsTcpConnection::init()
{
    exit();

    m_running = true;

    WSADATA wsa_data;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
    {
        RUN_LOG_CRI("WSAStartup failed: %d", WSAGetLastError());
        return false;
    }

    if (!create_completion_port())
    {
        return false;
    }
    else
    {
        RUN_LOG_DBG("create completion port success");
    }

    if (!create_work_threads())
    {
        return false;
    }
    else
    {
        RUN_LOG_DBG("create work threads success");
    }

    if (!create_listen_socket())
    {
        return false;
    }
    else
    {
        RUN_LOG_DBG("create listen socket success");
    }

    RUN_LOG_DBG("init iocp server success");

    return true;
}

bool WindowsTcpConnection::exit()
{
    if (!m_running)
    {
        return true;
    }

    m_running = false;

    for (int index = 0; index < m_thread_count; ++index)
    {
        if (!post_exit())
        {
            return false;
        }
    }

    if (nullptr != m_thread)
    {
        WaitForMultipleObjects(m_thread_count, m_thread, TRUE, INFINITE);

        for (int index = 0; index < m_thread_count; ++index)
        {
            CloseHandle(m_thread[index]);
        }
        BASE_DELETE_A(m_thread);
    }

    if (nullptr != m_iocp)
    {
        CloseHandle(m_iocp);
        m_iocp = nullptr;
    }

    if (nullptr != m_listen_comp_info)
    {
        m_listen_comp_info->close_socket();
        std::list<CONNECTION_INFO *>::iterator iter = 
            m_listen_comp_info->overlapped_info_list.begin();
        while (m_listen_comp_info->overlapped_info_list.end() != iter)
        {
            (*iter)->close_socket();
            ++iter;
        }
        m_over_info_pool << m_listen_comp_info->overlapped_info_list;
        m_comp_info_pool.release(m_listen_comp_info);
    }

    std::map<socket_t, COMPLETION_INFO *>::iterator iter =
        m_comp_info_map.begin();
    while (m_comp_info_map.end() != iter)
    {
        iter->second->close_socket();
        m_over_info_pool << iter->second->overlapped_info_list;
        m_comp_info_pool.release(iter->second);
        ++iter;
    }
    m_comp_info_map.clear();

    m_comp_info_pool.clear();
    m_over_info_pool.clear();

    m_acceptex_func = nullptr;
    m_get_acceptex_sock_addrs_func = nullptr;

    WSACleanup();

    RUN_LOG_DBG("exit iocp server success");

    return true;
}

void WindowsTcpConnection::thread_run(int thread_index)
{
    RUN_LOG_DBG("iocp work thread[%d] begin", thread_index);

    while (running())
    {
        DWORD transferred_size = 0;

        OVERLAPPED * overlapped = nullptr;
        COMPLETION_INFO * comp_info = nullptr;

        if (!GetQueuedCompletionStatus(
                m_iocp, &transferred_size, 
                reinterpret_cast<PULONG_PTR>(&comp_info), 
                &overlapped, INFINITE))
        {
            RUN_LOG_ERR("GetQueuedCompletionStatus failed %d", GetLastError());
            handle_error(comp_info, overlapped);
            continue;
        }

        if (POST_EXIT == reinterpret_cast<ULONG_PTR>(comp_info))
        {
            RUN_LOG_DBG("get exit request and break from thread[%d]", thread_index);
            break;
        }

        CONNECTION_INFO * over_info = 
            CONTAINING_RECORD(overlapped, CONNECTION_INFO, overlapped);

        if ((0 == transferred_size) && 
            (POST_SEND == over_info->post_type || 
             POST_RECV == over_info->post_type))
        {
            do_close(comp_info);
            continue;
        }

        switch (over_info->post_type)
        {
            case POST_ACCEPT:
            {
                do_accept(over_info, transferred_size);
                break;
            }
            case POST_SEND:
            {
                do_send(over_info, transferred_size);
                break;
            }
            case POST_RECV:
            {
                do_recv(over_info, transferred_size);
                break;
            }
            default:
            {
                RUN_LOG_ERR("unknown post type: %d", over_info->post_type);
                break;
            }
        }
    }

    RUN_LOG_DBG("iocp work thread[%d] finish", thread_index);
}

bool WindowsTcpConnection::running() const
{
    return m_running;
}

void WindowsTcpConnection::set_local_server_port(unsigned short host_port)
{
    m_port = (0 == host_port ? DEFAULT_TCP_PORT : host_port);
}

void WindowsTcpConnection::set_manager(ITcpManager * manager)
{
    m_manager = manager;
}

bool WindowsTcpConnection::try_do_accept(CONNECTION_INFO * over_info, 
                                         int accept_size)
{
    CONNECTION_INFO * send_over_info = nullptr;
    CONNECTION_INFO * recv_over_info = nullptr;
    COMPLETION_INFO * comp_info = nullptr;
    if (!acquire_completion(over_info->sockfd, comp_info, 
                            send_over_info, recv_over_info))
    {
        RUN_LOG_ERR("acquire completiont failed");
        return false;
    }

    DWORD addr_len = sizeof(sockaddr_in_t) + 16;
    sockaddr_in_t * local_addr = nullptr;
    sockaddr_in_t * remote_addr = nullptr;
    int local_addr_len = 0;
    int remote_addr_len = 0;

    m_get_acceptex_sock_addrs_func(over_info->wsa_buffer.buf, 
                                   over_info->wsa_buffer.len - 2 * addr_len, 
                                   addr_len, addr_len, 
                                   reinterpret_cast<sockaddr_t **>(&local_addr), 
                                   &local_addr_len, 
                                   reinterpret_cast<sockaddr_t **>(&remote_addr), 
                                   &remote_addr_len);

    comp_info->server = true;
    memcpy(&comp_info->remote_address, remote_addr, sizeof(sockaddr_in_t));

    RUN_LOG_DBG("client %s:%d connect", 
                inet_ntoa(remote_addr->sin_addr), 
                ntohs(remote_addr->sin_port));

    do
    {
        if (!register_connection(comp_info))
        {
            RUN_LOG_ERR("register connection failed");
            break;
        }

        if (!on_accept(comp_info->sockfd, send_over_info))
        {
            RUN_LOG_ERR("on accept failed");
            break;
        }

        if (0 < accept_size)
        {
            on_recv(over_info->sockfd, over_info->buffer, accept_size);
        }

        if (!post_recv(recv_over_info))
        {
            break;
        }

        comp_info = nullptr;
    } while (false);

    if (nullptr != comp_info)
    {
        do_close(comp_info);
    }

    return true;
}

void WindowsTcpConnection::do_accept(CONNECTION_INFO * over_info, int accept_size)
{
    if (!try_do_accept(over_info, accept_size))
    {
        over_info->close_socket();
    }
    post_accept(over_info);
}

void WindowsTcpConnection::do_send(CONNECTION_INFO * over_info, int send_size)
{
    if (send_size == static_cast<int>(over_info->wsa_buffer.len))
    {
        over_info->wsa_buffer.buf = over_info->buffer;
        over_info->wsa_buffer.len = 0;
        on_send(over_info->sockfd);
    }
    else if (send_size < static_cast<int>(over_info->wsa_buffer.len))
    {
        over_info->wsa_buffer.buf += send_size;
        over_info->wsa_buffer.len -= send_size;
        post_send(over_info);
    }
    else
    {
        RUN_LOG_ERR("do_send failed");
    }
}

void WindowsTcpConnection::do_recv(CONNECTION_INFO * over_info, int recv_size)
{
    if (0 < recv_size)
    {
        on_recv(over_info->sockfd, over_info->buffer, recv_size);
    }
    post_recv(over_info);
}

void WindowsTcpConnection::do_close(COMPLETION_INFO * comp_info)
{
    if (nullptr == comp_info || 
        m_listen_comp_info == comp_info)
    {
        return;
    }

    const sockaddr_in_t & remote_address = comp_info->remote_address;

    if (comp_info->server)
    {
        RUN_LOG_DBG("client %s:%d disconnect", 
                    inet_ntoa(remote_address.sin_addr), 
                    ntohs(remote_address.sin_port));
    }
    else
    {
        RUN_LOG_DBG("disconnect from %s:%d", 
                    inet_ntoa(remote_address.sin_addr), 
                    ntohs(remote_address.sin_port));
    }

    socket_t sockfd = comp_info->sockfd;

    BaseGuard<ThreadLocker> map_erase_guard(m_comp_info_locker);
    m_comp_info_map.erase(comp_info->sockfd);
    map_erase_guard.release();

    comp_info->close_socket();

    m_over_info_pool << comp_info->overlapped_info_list;
    m_comp_info_pool.release(comp_info);

    on_close(sockfd);
}

bool WindowsTcpConnection::on_accept(socket_t sockfd, CONNECTION_INFO * send_over_info)
{
    if (nullptr == m_manager)
    {
        return false;
    }
    return m_manager->when_accept(sockfd, send_over_info);
}

bool WindowsTcpConnection::on_connect(socket_t sockfd, CONNECTION_INFO * send_over_info)
{
    if (nullptr == m_manager)
    {
        return false;
    }
    return m_manager->when_connect(sockfd, send_over_info);
}

void WindowsTcpConnection::on_send(socket_t sockfd)
{
    if (nullptr != m_manager)
    {
        m_manager->when_send(sockfd);
    }
}

void WindowsTcpConnection::on_recv(socket_t sockfd, const char * data, int size)
{
    if (nullptr != m_manager)
    {
        m_manager->when_recv(sockfd, data, size);
    }
}

void WindowsTcpConnection::on_close(socket_t sockfd)
{
    if (nullptr != m_manager)
    {
        m_manager->when_close(sockfd);
    }
}

bool WindowsTcpConnection::create_completion_port()
{
    m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (nullptr == m_iocp)
    {
        RUN_LOG_CRI("create completion port failed: %d", WSAGetLastError());
        return false;
    }
    return true;
}

bool WindowsTcpConnection::bind_to_completion_port(COMPLETION_INFO * comp_info)
{
    if (nullptr == CreateIoCompletionPort(
                       reinterpret_cast<HANDLE>(comp_info->sockfd), m_iocp, 
                       reinterpret_cast<ULONG_PTR>(comp_info), 0))
    {
        RUN_LOG_CRI("bind to completion port failed: %d", WSAGetLastError());
        return false;
    }
    return true;
}

bool WindowsTcpConnection::create_work_threads()
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    int thread_count = system_info.dwNumberOfProcessors * 2;
    
    BASE_NEW_A(m_thread, HANDLE, thread_count);
    if (nullptr == m_thread)
    {
        RUN_LOG_CRI("create thread handle failed");
        return false;
    }

    for (m_thread_count = 0; m_thread_count < thread_count; ++m_thread_count)
    {
        IOCP_WORK_THREAD_PARAM * thread_argu = nullptr;
        BASE_NEW(thread_argu, IOCP_WORK_THREAD_PARAM(*this, m_thread_count + 1));
        if (nullptr == thread_argu)
        {
            RUN_LOG_CRI("create work thread param failed");
            return false;
        }

        m_thread[m_thread_count] = reinterpret_cast<HANDLE>(
            _beginthreadex(nullptr, 0, work_thread_run, thread_argu, 0, nullptr));
        if (nullptr == m_thread[m_thread_count])
        {
            BASE_DELETE(thread_argu);
            RUN_LOG_CRI("_beginthreadex failed: %d", GetLastError());
            return false;
        }
    }

    return true;
}

bool WindowsTcpConnection::get_extern_wsa_functions()
{
    DWORD bytes = 0;
    GUID acceptex_guid = WSAID_ACCEPTEX;
    if (0 != WSAIoctl(
                 m_listen_comp_info->sockfd, 
                 SIO_GET_EXTENSION_FUNCTION_POINTER, 
                 &acceptex_guid, 
                 sizeof(acceptex_guid), 
                 &m_acceptex_func, 
                 sizeof(m_acceptex_func), 
                 &bytes, 
                 nullptr, 
                 nullptr))
    {
        RUN_LOG_CRI("get AcceptEx failed: %d", WSAGetLastError());
        return false;
    }

    bytes = 0;
    GUID get_acceptex_sock_addrs_guid = WSAID_GETACCEPTEXSOCKADDRS;
    if (0 != WSAIoctl(
                 m_listen_comp_info->sockfd, 
                 SIO_GET_EXTENSION_FUNCTION_POINTER, 
                 &get_acceptex_sock_addrs_guid, 
                 sizeof(get_acceptex_sock_addrs_guid), 
                 &m_get_acceptex_sock_addrs_func, 
                 sizeof(m_get_acceptex_sock_addrs_func), 
                 &bytes, 
                 nullptr, 
                 nullptr))
    {
        RUN_LOG_CRI("get GetAcceptExSockaddrs failed: %d", WSAGetLastError());
        return false;
    }

    return true;
}

bool WindowsTcpConnection::create_listen_socket()
{
    m_listen_comp_info = m_comp_info_pool.acquire();
    if (nullptr == m_listen_comp_info)
    {
        RUN_LOG_ERR("acquire listen comp_info failed: %d", GetLastError());
        return false;
    }

    m_listen_comp_info->sockfd = 
        WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == m_listen_comp_info->sockfd)
    {
        RUN_LOG_ERR("WSASocket listen failed: %d", WSAGetLastError());
        return false;
    }

    if (!bind_to_completion_port(m_listen_comp_info))
    {
        return false;
    }

    sockaddr_in_t server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in_t));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(m_port);

    if (0 != bind(m_listen_comp_info->sockfd, 
                  reinterpret_cast<sockaddr_t *>(&server_addr), 
                  sizeof(server_addr)))
    {
        RUN_LOG_ERR("bind failed: %d", WSAGetLastError());
        return false;
    }

    if (0 != listen(m_listen_comp_info->sockfd, SOMAXCONN))
    {
        RUN_LOG_ERR("listen failed: %d", WSAGetLastError());
        return false;
    }

    if (!get_extern_wsa_functions())
    {
        return false;
    }

    for (int index = 0; index < m_thread_count; ++index)
    {
        CONNECTION_INFO * over_info = m_over_info_pool.acquire();
        if (nullptr == over_info)
        {
            RUN_LOG_ERR("acquire over_info failed: %d", GetLastError());
            return false;
        }

        m_listen_comp_info->overlapped_info_list.push_back(over_info);

        if (!post_accept(over_info))
        {
            RUN_LOG_ERR("post accept failed: %d", GetLastError());
            return false;
        }
    }

    return true;
}

void WindowsTcpConnection::handle_error(COMPLETION_INFO * comp_info, 
                                        OVERLAPPED * overlapped)
{
    if (WAIT_TIMEOUT == GetLastError())
    {
        return;
    }

    CONNECTION_INFO * over_info = 
        CONTAINING_RECORD(overlapped, CONNECTION_INFO, overlapped);

    if (POST_ACCEPT == over_info->post_type)
    {
        post_accept(over_info);
        return;
    }

    if (0 == send(comp_info->sockfd, nullptr, 0, 0))
    {
        return;
    }

    RUN_LOG_ERR("handle error: %d", GetLastError());

    do_close(comp_info);
}

bool WindowsTcpConnection::post_accept(CONNECTION_INFO * over_info)
{
    over_info->accept_reset();

    over_info->sockfd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 
                                  nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == over_info->sockfd)
    {
        RUN_LOG_ERR("WSASocket failed: %d", WSAGetLastError());
        return false;
    }

    DWORD addr_len = sizeof(sockaddr_in_t) + 16;
    DWORD recv_size = 0;
    if (!m_acceptex_func(m_listen_comp_info->sockfd, over_info->sockfd, 
            over_info->wsa_buffer.buf, 0, /* not recv data here */
            addr_len, addr_len, &recv_size, &over_info->overlapped))
    {
        if (WSA_IO_PENDING != WSAGetLastError())
        {
            RUN_LOG_ERR("AcceptEx failed: %d", WSAGetLastError());
            return false;
        }
    }

    if (0 != recv_size)
    {
        RUN_LOG_ERR("AcceptEx error: %d", recv_size);
    }

    return true;
}

bool WindowsTcpConnection::post_send(CONNECTION_INFO * over_info)
{
    WSABUF wsa_buffer[1] = { over_info->wsa_buffer };
    DWORD send_size = 0;
    DWORD flags = 0;

    if (SOCKET_ERROR == WSASend(over_info->sockfd, wsa_buffer, 1, &send_size, 
                                flags, &over_info->overlapped, nullptr))
    {
        if (WSA_IO_PENDING != WSAGetLastError())
        {
            RUN_LOG_ERR("WSASend failed: %d", WSAGetLastError());
            return false;
        }
    }

    /*
    if (0 < send_size)
    {
        do_send(over_info, send_size);
    }
    */

    return true;
}

bool WindowsTcpConnection::stop_send(CONNECTION_INFO * over_info)
{
    return true;
}

bool WindowsTcpConnection::post_recv(CONNECTION_INFO * over_info)
{
    WSABUF wsa_buffer[1] = { over_info->wsa_buffer };
    DWORD recv_size = 0;
    DWORD flags = 0;

    if (SOCKET_ERROR == WSARecv(over_info->sockfd, wsa_buffer, 1, &recv_size, 
                                &flags, &over_info->overlapped, nullptr))
    {
        if (WSA_IO_PENDING != WSAGetLastError())
        {
            RUN_LOG_ERR("WSARecv failed: %d", WSAGetLastError());
            return false;
        }
    }

    /*
    if (0 < recv_size)
    {
        on_recv(over_info->sockfd, over_info->buffer, recv_size);
    }
    */

    return true;
}

bool WindowsTcpConnection::post_exit()
{
    if (!PostQueuedCompletionStatus(m_iocp, 0, 
            static_cast<ULONG_PTR>(POST_EXIT), nullptr))
    {
        RUN_LOG_CRI("PostQueuedCompletionStatus failed: %d", WSAGetLastError());
        return false;
    }

    return true;
}

bool WindowsTcpConnection::acquire_completion(socket_t sockfd, 
                                              COMPLETION_INFO *& comp_info, 
                                              CONNECTION_INFO *& send_over_info, 
                                              CONNECTION_INFO *& recv_over_info)
{
    comp_info = nullptr;
    send_over_info = nullptr;
    recv_over_info = nullptr;

    send_over_info = m_over_info_pool.acquire();
    if (nullptr == send_over_info)
    {
        RUN_LOG_ERR("acquire over_info failed: %d", GetLastError());
        return false;
    }
    send_over_info->send_reset();

    recv_over_info = m_over_info_pool.acquire();
    if (nullptr == recv_over_info)
    {
        m_over_info_pool.release(send_over_info);
        RUN_LOG_ERR("acquire over_info failed: %d", GetLastError());
        return false;
    }
    recv_over_info->recv_reset();

    comp_info = m_comp_info_pool.acquire();
    if (nullptr == comp_info)
    {
        m_over_info_pool.release(send_over_info);
        m_over_info_pool.release(recv_over_info);
        RUN_LOG_ERR("acquire comp_info failed: %d", GetLastError());
        return false;
    }

    send_over_info->sockfd = sockfd;
    recv_over_info->sockfd = sockfd;
    comp_info->sockfd = sockfd;
    comp_info->overlapped_info_list.push_back(send_over_info);
    comp_info->overlapped_info_list.push_back(recv_over_info);

    return true;
}

bool WindowsTcpConnection::register_connection(COMPLETION_INFO * comp_info)
{
    if (!bind_to_completion_port(comp_info))
    {
        RUN_LOG_CRI("bind to completion port failed: %d", WSAGetLastError());
        return false;
    }

    BaseGuard<ThreadLocker> map_insert_guard(m_comp_info_locker);
    if (!m_comp_info_map.insert(std::make_pair(comp_info->sockfd, 
                                               comp_info)).second)
    {
        RUN_LOG_CRI("insert comp_info to map failed: %d", GetLastError());
        return false;
    }
    map_insert_guard.release();

    return true;
}

bool WindowsTcpConnection::unregister_connection(socket_t sockfd)
{
    COMPLETION_INFO * comp_info = nullptr;
    BaseGuard<ThreadLocker> map_search_guard(m_comp_info_locker);
    std::map<socket_t, COMPLETION_INFO *>::iterator iter = 
        m_comp_info_map.find(sockfd);
    if (m_comp_info_map.end() == iter)
    {
        RUN_LOG_DBG("search socket %d from map failed", sockfd);
        return false;
    }
    comp_info = iter->second;
    map_search_guard.release();

    do_close(comp_info);

    return true;
}

bool WindowsTcpConnection::create_connection(sockaddr_in_t & server_address)
{
    socket_t sockfd = create_sockfd(server_address);
    if (BAD_SOCKET == sockfd)
    {
        return false;
    }

    CONNECTION_INFO * send_over_info = nullptr;
    CONNECTION_INFO * recv_over_info = nullptr;
    COMPLETION_INFO * comp_info = nullptr;

    if (!acquire_completion(sockfd, comp_info, send_over_info, recv_over_info))
    {
        RUN_LOG_ERR("acquire connection failed");
        closesocket(sockfd);
        return false;
    }

    comp_info->server = false;
    memcpy(&comp_info->remote_address, &server_address, sizeof(server_address));

    RUN_LOG_DBG("connect to %s:%d", 
                inet_ntoa(server_address.sin_addr), 
                ntohs(server_address.sin_port));

    do
    {
        if (!register_connection(comp_info))
        {
            RUN_LOG_ERR("register connection failed");
            break;
        }

        if (!on_connect(comp_info->sockfd, send_over_info))
        {
            RUN_LOG_ERR("on connect failed");
            break;
        }

        if (!post_recv(recv_over_info))
        {
            break;
        }

        return true;
    } while (false);

    do_close(comp_info);

    return false;
}

bool WindowsTcpConnection::create_connection(const char * host_name, 
                                             unsigned short host_port)
{
    sockaddr_in_t server_address;
    if (!transform_address(host_name, host_port, server_address))
    {
        return false;
    }
    return create_connection(server_address);
}

bool WindowsTcpConnection::destroy_connection(socket_t sockfd)
{
    return unregister_connection(sockfd);
}

bool WindowsTcpConnection::transform_address(const char * host_name, 
                                             unsigned short host_port, 
                                             sockaddr_in_t & server_address)
{
    if (nullptr == host_name || 0 == host_port)
    {
        return false;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(host_port);
    server_address.sin_addr.s_addr = inet_addr(host_name);

    return true;
}

socket_t WindowsTcpConnection::create_sockfd(sockaddr_in_t & server_address)
{
    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (BAD_SOCKET == sockfd)
    {
        RUN_LOG_ERR("socket failed: %d", WSAGetLastError());
        return BAD_SOCKET;
    }

    if (0 > connect(sockfd, 
                    reinterpret_cast<sockaddr_t *>(&server_address), 
                    sizeof(server_address)))
    {
        RUN_LOG_ERR("connect failed: %d", WSAGetLastError());
        closesocket(sockfd);
        return BAD_SOCKET;
    }

    u_long mode = 1;
    if (SOCKET_ERROR == ioctlsocket(sockfd, FIONBIO, &mode))
    {
        RUN_LOG_ERR("ioctlsocket(FIONBIO) failed: %d", WSAGetLastError());
    }

    return sockfd;
}

NAMESPACE_COMMON_END


#endif // WIN32
