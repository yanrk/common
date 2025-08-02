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

#ifndef WIN32


#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <signal.h>

#include <cstring>

#include "net_linux_tcp_connection.h"
#include "base_memory.h"
#include "base_log.h"
#include "base_guard.hpp"
#include "base_time.h"

NAMESPACE_COMMON_BEGIN

struct EPOLL_WORK_THREAD_PARAM
{
    LinuxTcpConnection & tcp_connection;
    int                  thread_index;

    EPOLL_WORK_THREAD_PARAM(LinuxTcpConnection & connection, int index);
};

EPOLL_WORK_THREAD_PARAM::EPOLL_WORK_THREAD_PARAM(LinuxTcpConnection & connection, 
                                                 int index)
    : tcp_connection(connection), thread_index(index)
{

}

void * main_work_thread_run(void * argu)
{
    if (nullptr != argu)
    {
        EPOLL_WORK_THREAD_PARAM * thread_argu = 
            reinterpret_cast<EPOLL_WORK_THREAD_PARAM *>(argu);
        thread_argu->tcp_connection.main_thread_run();
        BASE_DELETE(thread_argu);
    }
    return nullptr;
}

void * sub_work_thread_run(void * argu)
{
    if (nullptr != argu)
    {
        EPOLL_WORK_THREAD_PARAM * thread_argu = 
            reinterpret_cast<EPOLL_WORK_THREAD_PARAM *>(argu);
        thread_argu->tcp_connection.sub_thread_run(thread_argu->thread_index);
        BASE_DELETE(thread_argu);
    }
    return nullptr;
}

LinuxTcpConnection::LinuxTcpConnection()
    : m_running(false)
    , m_port(0)
    , m_epoll(-1)
    , m_thread(nullptr)
    , m_thread_count(0)
    , m_epoll_data_pool("epoll data pool")
    , m_listen_epoll_event()
    , m_epoll_data_map()
    , m_epoll_data_locker("epoll data map locker")
    , m_active_epoll_events()
    , m_active_epoll_events_locker("active epoll events locker")
    , m_manager(nullptr)
{
    m_listen_epoll_event.data.ptr = nullptr;
    signal(SIGPIPE, SIG_IGN);
}

LinuxTcpConnection::~LinuxTcpConnection()
{
    exit();
}

bool LinuxTcpConnection::init()
{
    exit();

    m_running = true;

    if (!create_epoll())
    {
        return false;
    }
    else
    {
        RUN_LOG_DBG("create epoll success");
    }

    if (!create_listen_socket())
    {
        return false;
    }
    else
    {
        RUN_LOG_DBG("create listen socket success");
    }

    if (!create_work_threads())
    {
        return false;
    }
    else
    {
        RUN_LOG_DBG("create work threads success");
    }

    RUN_LOG_DBG("init epoll server success");

    return true;
}

bool LinuxTcpConnection::exit()
{
    if (!m_running)
    {
        return true;
    }

    m_running = false;

    if (-1 != m_epoll)
    {
        int epoll_dup = m_epoll;
        m_epoll = -1;
        close(epoll_dup);
    }

    for (int index = 0; index < m_thread_count; ++index)
    {
        if (0 != pthread_join(m_thread[index], nullptr))
        {
            return false;
        }
    }

    if (nullptr != m_listen_epoll_event.data.ptr)
    {
        CONNECTION_INFO * epoll_data = 
            reinterpret_cast<CONNECTION_INFO *>(m_listen_epoll_event.data.ptr);
        epoll_data->close_socket();
        m_epoll_data_pool.release(epoll_data);
    }

    m_active_epoll_events.clear();

    std::map<socket_t, CONNECTION_INFO *>::iterator iter = 
        m_epoll_data_map.begin();
    while (m_epoll_data_map.end() != iter)
    {
        iter->second->close_socket();
        m_epoll_data_pool.release(iter->second);
        ++iter;
    }
    m_epoll_data_map.clear();

    m_epoll_data_pool.clear();

    RUN_LOG_DBG("exit epoll server success");

    return true;
}

bool LinuxTcpConnection::running() const
{
    return m_running;
}

void LinuxTcpConnection::main_thread_run()
{
    RUN_LOG_DBG("epoll main work thread begin");

    CONNECTION_INFO * listen_epoll_data = 
        reinterpret_cast<CONNECTION_INFO *>(m_listen_epoll_event.data.ptr);
    const socket_t listenfd = listen_epoll_data->sockfd;
    const int sub_thread_count = m_thread_count - 1;
    std::vector< std::list<struct epoll_event> > 
        active_epoll_events(sub_thread_count);
    const int PER_EVENT_SIZE = 256;
    struct epoll_event events[PER_EVENT_SIZE];

    while (running())
    {
        int event_count = epoll_wait(m_epoll, events, PER_EVENT_SIZE, 1000);
        if (-1 == event_count)
        {
            if (-1 == m_epoll)
            {
                break;
            }

            if (EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno)
            {
                continue;
            }
            else
            {
                RUN_LOG_CRI("epoll wait failed: %d", errno);
                break;
            }
        }
        else if (0 == event_count)
        {
            continue;
        }

        bool accept_posted = false;

        for (int index = 0; index < event_count; ++index)
        {
            CONNECTION_INFO * epoll_data = 
                reinterpret_cast<CONNECTION_INFO *>(events[index].data.ptr);
            if (listenfd == epoll_data->sockfd)
            {
                accept_posted = true;
            }
            else
            {
                std::list<struct epoll_event> & epoll_event_list = 
                    active_epoll_events[epoll_data->sockfd % sub_thread_count];
                epoll_event_list.push_back(events[index]);
            }
        }

        BaseGuard<ThreadLocker> epoll_events_guard(m_active_epoll_events_locker);
        for (int index = 0; index < sub_thread_count; ++index)
        {
            std::list<struct epoll_event> & epoll_event_list = 
                active_epoll_events[index];
            if (epoll_event_list.empty())
            {
                continue;
            }

            std::list<struct epoll_event> & epoll_event_storage = 
                m_active_epoll_events[index];

            epoll_event_storage.splice(epoll_event_storage.end(), 
                                       epoll_event_list);
        }
        epoll_events_guard.release();

        if (accept_posted)
        {
            do_accept(listenfd);
        }
    }

    RUN_LOG_DBG("epoll main work thread finish");
}

void LinuxTcpConnection::sub_thread_run(int thread_index)
{
    RUN_LOG_DBG("epoll sub work thread[%d] begin", thread_index);

    std::list<struct epoll_event> epoll_event_list;

    while (running())
    {
        BaseGuard<ThreadLocker> epoll_events_guard(m_active_epoll_events_locker);
        std::list<struct epoll_event> & active_epoll_events = 
            m_active_epoll_events[thread_index - 1];
        epoll_event_list.splice(epoll_event_list.end(), active_epoll_events);
        epoll_events_guard.release();

        if (epoll_event_list.empty())
        {
            base_nanosecond_sleep(BASE_BIG_UINT(1000000));
            continue;
        }

        std::list<struct epoll_event>::iterator iter = epoll_event_list.begin();
        while (epoll_event_list.end() != iter)
        {
            struct epoll_event event = *iter;
            ++iter;

            CONNECTION_INFO * epoll_data = 
                reinterpret_cast<CONNECTION_INFO *>(event.data.ptr);

            bool badly = false;

            if (event.events & EPOLLIN)
            {
                if (!do_recv(epoll_data))
                {
                    badly = true;
                }
            }

            if (event.events & EPOLLOUT)
            {
                if (!do_send(epoll_data))
                {
                    badly = true;
                }
            }

            if (badly || (event.events & EPOLLERR))
            {
                do_close(epoll_data, true);
            }
        }
        epoll_event_list.clear();
    }

    RUN_LOG_DBG("epoll sub work thread[%d] finish", thread_index);
}

void LinuxTcpConnection::set_local_server_port(unsigned short host_port)
{
    m_port = (0 == host_port ? DEFAULT_TCP_PORT : host_port);
}

void LinuxTcpConnection::set_manager(ITcpManager * manager)
{
    m_manager = manager;
}

bool LinuxTcpConnection::create_work_threads()
{
    int thread_count = 1 + 10;

    m_active_epoll_events.resize(thread_count - 1);

    BASE_NEW_A(m_thread, pthread_t, thread_count);
    if (nullptr == m_thread)
    {
        RUN_LOG_CRI("create thread handle failed");
        return false;
    }

    m_thread_count = 0;

    EPOLL_WORK_THREAD_PARAM * main_thread_argu = nullptr;
    BASE_NEW(main_thread_argu, EPOLL_WORK_THREAD_PARAM(*this, 0));
    if (nullptr == main_thread_argu)
    {
        RUN_LOG_CRI("create main work thread param failed");
        return false;
    }

    if (0 != pthread_create(&m_thread[0], nullptr, 
                            main_work_thread_run, main_thread_argu))
    {
        BASE_DELETE(main_thread_argu);
        RUN_LOG_CRI("create main work thread failed: %d", errno);
        return false;
    }

    for (m_thread_count = 1; m_thread_count < thread_count; ++m_thread_count)
    {
        EPOLL_WORK_THREAD_PARAM * sub_thread_argu = nullptr;
        BASE_NEW(sub_thread_argu, EPOLL_WORK_THREAD_PARAM(*this, m_thread_count));
        if (nullptr == sub_thread_argu)
        {
            RUN_LOG_CRI("create sub work thread param failed");
            return false;
        }

        if (0 != pthread_create(&m_thread[m_thread_count], nullptr, 
                                sub_work_thread_run, sub_thread_argu))
        {
            BASE_DELETE(sub_thread_argu);
            RUN_LOG_CRI("pthread_create failed: %d", errno);
            return false;
        }
    }

    return true;
}

bool LinuxTcpConnection::create_epoll()
{
    const int max_connect_count = 10000; /* maybe it is disable */
    m_epoll = epoll_create(max_connect_count);
    if (-1 == m_epoll)
    {
        RUN_LOG_CRI("create epoll failed: %d", errno);
        return false;
    }
    return true;
}

bool LinuxTcpConnection::bind_epoll_event(CONNECTION_INFO * epoll_data)
{
    struct epoll_event event;
    event.data.ptr = epoll_data;
    event.events = EPOLLIN | EPOLLET;
    if (-1 == epoll_ctl(m_epoll, EPOLL_CTL_ADD, epoll_data->sockfd, &event))
    {
        RUN_LOG_CRI("add epoll event failed: %d", errno);
        return false;
    }
    return true;
}

bool LinuxTcpConnection::delete_epoll_event(CONNECTION_INFO * epoll_data)
{
    struct epoll_event event;
    event.data.ptr = epoll_data;
    if (-1 == epoll_ctl(m_epoll, EPOLL_CTL_DEL, epoll_data->sockfd, &event))
    {
        RUN_LOG_CRI("delete epoll event failed: %d", errno);
        return false;
    }
    return true;
}

bool LinuxTcpConnection::modify_epoll_event(CONNECTION_INFO * epoll_data, 
                                            bool send, bool recv)
{
    struct epoll_event event;
    event.data.ptr = epoll_data;
    event.events = EPOLLET;
    if (send)
    {
        event.events |= EPOLLOUT;
    }
    if (recv)
    {
        event.events |= EPOLLIN;
    }

    if (-1 == epoll_ctl(m_epoll, EPOLL_CTL_MOD, epoll_data->sockfd, &event))
    {
        RUN_LOG_CRI("modify epoll event failed: %d", errno);
        return false;
    }

    return true;
}

bool LinuxTcpConnection::post_send(CONNECTION_INFO * epoll_data)
{
    return modify_epoll_event(epoll_data, true, true);
}

bool LinuxTcpConnection::stop_send(CONNECTION_INFO * epoll_data)
{
    return modify_epoll_event(epoll_data, false, true);
}

bool LinuxTcpConnection::create_listen_socket()
{
    CONNECTION_INFO * epoll_data = m_epoll_data_pool.acquire();
    if (nullptr == epoll_data)
    {
        RUN_LOG_CRI("acquire epoll_data failed: %d", errno);
        return false;
    }
    m_listen_epoll_event.data.ptr = epoll_data;

    epoll_data->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == epoll_data->sockfd)
    {
        RUN_LOG_CRI("create listen socket failed: %d", errno);
        return false;
    }

    int on = 1;
    setsockopt(epoll_data->sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  

    sockaddr_in_t server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(m_port);

    if (-1 == bind(epoll_data->sockfd, 
                   reinterpret_cast<sockaddr_t *>(&server_addr), 
                   sizeof(server_addr)))
    {
        RUN_LOG_CRI("bind failed: %d", errno);
        return false;
    }

    const int LISTENQ = 1024;
    if (-1 == listen(epoll_data->sockfd, LISTENQ))
    {
        RUN_LOG_CRI("listen failed: %d", errno);
        return false;
    }

    if (!set_nonblock(epoll_data->sockfd))
    {
        RUN_LOG_ERR("set listen nonblock failed: %d", errno);
    }

    if (!bind_epoll_event(epoll_data))
    {
        return false;
    }

    return true;
}

bool LinuxTcpConnection::set_nonblock(socket_t sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (-1 == flag)
    {
        RUN_LOG_CRI("fcntl(F_GETFL) failed: %d", errno);
        return false;
    }
    if (-1 == fcntl(sockfd, F_SETFL, flag | O_NONBLOCK))
    {
        RUN_LOG_CRI("fcntl(F_SETFL) failed: %d", errno);
        return false;
    }
    return true;
}

void LinuxTcpConnection::do_accept(socket_t listenfd)
{
    sockaddr_in_t client_addr;

    while (true)
    {
        CONNECTION_INFO * epoll_data = m_epoll_data_pool.acquire();
        if (nullptr == epoll_data)
        {
            RUN_LOG_ERR("acquire epoll_data failed: %d", errno);
            break;
        }
        epoll_data->reset();

        socklen_t addr_len = sizeof(client_addr);
        epoll_data->sockfd = 
            accept(listenfd, 
                   reinterpret_cast<sockaddr_t *>(&client_addr), 
                   &addr_len);
        if (-1 == epoll_data->sockfd)
        {
            m_epoll_data_pool.release(epoll_data);
            if (EAGAIN != errno && EWOULDBLOCK != errno && EINTR != errno)
            {
                RUN_LOG_ERR("accept failed: %d", errno);
            }
            break;
        }

        epoll_data->server = true;
        memcpy(&epoll_data->remote_address, &client_addr, sizeof(client_addr));

        RUN_LOG_DBG("client %s:%d connect", 
                    inet_ntoa(client_addr.sin_addr), 
                    ntohs(client_addr.sin_port));

        if (!register_connection(epoll_data))
        {
            RUN_LOG_ERR("register connection failed");
            break;
        }

        if (!on_accept(epoll_data->sockfd, epoll_data))
        {
            RUN_LOG_ERR("on accept failed");
            do_close(epoll_data, true);
            break;
        }
    }
}

bool LinuxTcpConnection::do_send(CONNECTION_INFO * epoll_data)
{
    if (-1 == epoll_data->sockfd)
    {
        return true;
    }

    int send_count = 
        send(epoll_data->sockfd, 
             epoll_data->send_buffer + epoll_data->send_len, 
             epoll_data->send_left, 0);
    if (-1 == send_count)
    {
        return EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno;
    }
    else if (0 < send_count)
    {
        epoll_data->send_len  += send_count;
        epoll_data->send_left -= send_count;

        if (0 == epoll_data->send_left)
        {
            on_send(epoll_data->sockfd);
        }
    }

    return true;
}

bool LinuxTcpConnection::do_recv(CONNECTION_INFO * epoll_data)
{
    if (-1 == epoll_data->sockfd)
    {
        return true;
    }

    while (true)
    {
        int recv_count = 
            recv(epoll_data->sockfd, epoll_data->recv_buffer, 
                 epoll_data->buffer_size, 0);
        if (-1 == recv_count)
        {
            return EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno;
        }
        else if (0 < recv_count)
        {
            on_recv(epoll_data->sockfd, epoll_data->recv_buffer, recv_count);
        }
        else
        {
            return false;
        }

        if (recv_count < epoll_data->buffer_size)
        {
            break;
        }
    }

    return true;
}

void LinuxTcpConnection::do_close(CONNECTION_INFO * epoll_data, bool invoke)
{
    if (-1 == epoll_data->sockfd)
    {
        return;
    }

    const sockaddr_in_t & remote_address = epoll_data->remote_address;

    if (epoll_data->server)
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

    socket_t sockfd = epoll_data->sockfd;

    delete_epoll_event(epoll_data);

    BaseGuard<ThreadLocker> map_remove_guard(m_epoll_data_locker);
    m_epoll_data_map.erase(epoll_data->sockfd);
    map_remove_guard.release();

    epoll_data->close_socket();
    m_epoll_data_pool.release(epoll_data);

    if (invoke)
    {
        on_close(sockfd);
    }
}

bool LinuxTcpConnection::on_accept(socket_t sockfd, CONNECTION_INFO * epoll_data)
{
    if (nullptr == m_manager)
    {
        return false;
    }
    return m_manager->when_accept(sockfd, epoll_data);
}

bool LinuxTcpConnection::on_connect(socket_t sockfd, CONNECTION_INFO * epoll_data)
{
    if (nullptr == m_manager)
    {
        return false;
    }
    return m_manager->when_connect(sockfd, epoll_data);
}

void LinuxTcpConnection::on_send(socket_t sockfd)
{
    if (nullptr != m_manager)
    {
        m_manager->when_send(sockfd);
    }
}

void LinuxTcpConnection::on_recv(socket_t sockfd, const char * data, int size)
{
    if (nullptr != m_manager)
    {
        m_manager->when_recv(sockfd, data, size);
    }
}

void LinuxTcpConnection::on_close(socket_t sockfd)
{
    if (nullptr != m_manager)
    {
        m_manager->when_close(sockfd);
    }
}

bool LinuxTcpConnection::register_connection(CONNECTION_INFO * epoll_data)
{
    if (!set_nonblock(epoll_data->sockfd))
    {
        RUN_LOG_ERR("set socket nonblock failed: %d", errno);
    }

    do
    {
        BaseGuard<ThreadLocker> map_insert_guard(m_epoll_data_locker);
        if (!m_epoll_data_map.insert(std::make_pair(epoll_data->sockfd, 
                                                    epoll_data)).second)
        {
            RUN_LOG_CRI("insert epoll_data to map failed: %d", errno);
            break;
        }
        map_insert_guard.release();

        if (!bind_epoll_event(epoll_data))
        {
            RUN_LOG_CRI("bind epoll event failed: %d", errno);
            break;
        }

        return true;
    } while (false);

    do_close(epoll_data, false);

    return false;
}

bool LinuxTcpConnection::unregister_connection(socket_t sockfd)
{
    CONNECTION_INFO * epoll_data = nullptr;
    BaseGuard<ThreadLocker> map_search_guard(m_epoll_data_locker);
    std::map<socket_t, CONNECTION_INFO *>::iterator iter = 
        m_epoll_data_map.find(sockfd);
    if (m_epoll_data_map.end() == iter)
    {
        RUN_LOG_DBG("search socket %d from map failed", sockfd);
        return false;
    }
    epoll_data = iter->second;
    map_search_guard.release();

    do_close(epoll_data, true);

    return true;
}

bool LinuxTcpConnection::create_connection(sockaddr_in_t & server_address)
{
    socket_t sockfd = create_sockfd(server_address);
    if (BAD_SOCKET == sockfd)
    {
        return false;
    }

    CONNECTION_INFO * epoll_data = m_epoll_data_pool.acquire();
    if (nullptr == epoll_data)
    {
        RUN_LOG_ERR("acquire epoll_data failed: %d", errno);
        close(sockfd);
        return false;
    }
    epoll_data->reset();
    epoll_data->sockfd = sockfd;

    epoll_data->server = false;
    memcpy(&epoll_data->remote_address, &server_address, sizeof(server_address));

    RUN_LOG_DBG("connect to %s:%d", 
                inet_ntoa(server_address.sin_addr), 
                ntohs(server_address.sin_port));

    if (!register_connection(epoll_data))
    {
        RUN_LOG_ERR("register connection failed");
        return false;
    }

    if (!on_connect(epoll_data->sockfd, epoll_data))
    {
        RUN_LOG_ERR("on connect failed");
        do_close(epoll_data, true);
        return false;
    }

    return true;
}

bool LinuxTcpConnection::create_connection(const char * host_name, 
                                           unsigned short host_port)
{
    sockaddr_in_t server_address;
    if (!transform_address(host_name, host_port, server_address))
    {
        return false;
    }
    return create_connection(server_address);
}

bool LinuxTcpConnection::destroy_connection(socket_t sockfd)
{
    return unregister_connection(sockfd);
}

bool LinuxTcpConnection::transform_address(const char * host_name, 
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
    return 0 != inet_aton(host_name, &server_address.sin_addr);
}

socket_t LinuxTcpConnection::create_sockfd(sockaddr_in_t & server_address)
{
    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (BAD_SOCKET == sockfd)
    {
        RUN_LOG_ERR("socket failed: %d", errno);
        return BAD_SOCKET;
    }

    if (0 > connect(sockfd, 
                    reinterpret_cast<sockaddr_t *>(&server_address), 
                    sizeof(server_address)))
    {
        RUN_LOG_ERR("connect failed: %d", errno);
        close(sockfd);
        return BAD_SOCKET;
    }

    return sockfd;
}

NAMESPACE_COMMON_END


#endif // WIN32
