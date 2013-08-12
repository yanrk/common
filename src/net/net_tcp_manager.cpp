/********************************************************
 * Description : tcp manager
 * Data        : 2013-06-19 11:55:03
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include <ctime>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "net_tcp_manager.h"
#include "base_log.h"

NAMESPACE_COMMON_BEGIN

TcpManager::TcpManager()
    : ITcpManager()
    , NetDataHandler()
    , m_start(false)
    , m_tcp_connection()
    , m_remote_server_address()
    , m_next_connection(BAD_CONNECTION)
    , m_using_connection()
    , m_deque_locker()
    , m_sock2connection()
    , m_connection2sock()
    , m_map_locker()
    , m_message_handler(nullptr)
{
    m_tcp_connection.set_manager(this);
    memset(&m_remote_server_address, 0, sizeof(m_remote_server_address));
    m_next_connection = acquire_first_connection();
}

TcpManager::~TcpManager()
{
    stop();
}

bool TcpManager::set_local_port(unsigned short host_port)
{
    if (m_start)
    {
        return(false);
    }
    m_tcp_connection.set_local_server_port(host_port);
    return(true);
}

bool TcpManager::set_remote_address(const char * host_name, 
                                    unsigned short host_port)
{
    if (m_start)
    {
        return(false);
    }
    return(m_tcp_connection.transform_address(
               host_name, host_port, m_remote_server_address));
}

bool TcpManager::set_data_handle_thread_count(int thread_count)
{
    if (m_start)
    {
        return(false);
    }
    return(set_thread_count(thread_count));
}

bool TcpManager::set_message_handler(IMessageHandler * message_handler)
{
    if (m_start || nullptr == message_handler)
    {
        return(false);
    }
    m_message_handler = message_handler;
    m_message_handler->set_tcp_manager(this);
    return(true);
}

bool TcpManager::create_connection()
{
    return(m_tcp_connection.create_connection(m_remote_server_address));
}

bool TcpManager::create_connection(const char * host_name, 
                                   unsigned short host_port)
{
    return(m_tcp_connection.create_connection(host_name, host_port));
}

bool TcpManager::create_connection(sockaddr_in_t & server_address)
{
    return(m_tcp_connection.create_connection(server_address));
}

bool TcpManager::destroy_connection(connection_t connection)
{
    socket_t sockfd = BAD_SOCKET;
    if (!transform_connection(connection, sockfd))
    {
        return(false);
    }
    return(m_tcp_connection.destroy_connection(sockfd));
}

bool TcpManager::start()
{
    stop();

    m_start = true;

    do
    {
        if (!NetDataHandler::init())
        {
            break;
        }

        if (!m_tcp_connection.init())
        {
            break;
        }

        return(true);
    } while (false);

    stop();

    return(false);
}

void TcpManager::stop()
{
    if (!m_start)
    {
        return;
    }

    m_start = false;

    if (!m_tcp_connection.exit())
    {
        RUN_LOG_ERR("tcp connection exit failed");
    }

    if (!NetDataHandler::exit())
    {
        RUN_LOG_ERR("data handler exit failed");
    }

    memset(&m_remote_server_address, 0, sizeof(m_remote_server_address));

    BaseGuard<ThreadLocker> deque_guard(m_deque_locker);
    std::deque<connection_t>::iterator iter = 
        m_using_connection.begin();
    while (m_using_connection.end() != iter)
    {
        m_message_handler->on_close(*iter);
        ++iter;
    }
    m_using_connection.clear();
    deque_guard.release();

    BaseGuard<ThreadLocker> map_guard(m_map_locker);
    m_sock2connection.clear();
    m_connection2sock.clear();
    map_guard.release();
}

bool TcpManager::when_accept(socket_t sockfd, CONNECTION_INFO * send_buffer)
{
    return(connect_notice(sockfd, send_buffer, true));
}

bool TcpManager::when_connect(socket_t sockfd, CONNECTION_INFO * send_buffer)
{
    return(connect_notice(sockfd, send_buffer, false));
}

void TcpManager::when_send(socket_t sockfd)
{
    NetDataHandler::handle_send(sockfd);
}

void TcpManager::when_recv(socket_t sockfd, const char * data, int size)
{
    NetDataHandler::handle_recv(sockfd, data, size);
}

void TcpManager::when_close(socket_t sockfd)
{
    connection_t connection = BAD_CONNECTION;
    transform_socket(sockfd, connection);
    NetDataHandler::handle_close(sockfd);
    del_socket(sockfd);
    if (nullptr != m_message_handler && BAD_CONNECTION != connection)
    {
        m_message_handler->on_close(connection);
    }
}

bool TcpManager::send_message(connection_t connection, const char * data, int size)
{
    if (nullptr == m_message_handler)
    {
        return(false);
    }

    socket_t sockfd = BAD_SOCKET;
    if (!transform_connection(connection, sockfd))
    {
        return(false);
    }

    return(NetDataHandler::send_data(sockfd, data, size));
}

bool TcpManager::handle_data(socket_t sockfd, const char * data, int size)
{
    if (nullptr == m_message_handler)
    {
        return(false);
    }

    connection_t connection = BAD_CONNECTION;
    if (!transform_socket(sockfd, connection))
    {
        return(false);
    }

    return(m_message_handler->handle_message(connection, data, size));
}

bool TcpManager::continue_send(CONNECTION_INFO * send_buffer)
{
    return(m_tcp_connection.post_send(send_buffer));
}

bool TcpManager::suspend_send(CONNECTION_INFO * send_buffer)
{
    return(m_tcp_connection.stop_send(send_buffer));
}

connection_t TcpManager::acquire_first_connection()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    connection_t connection = 0;
    connection += rand() % 1000;
    connection *= 1000;
    connection += rand() % 1000;
    connection *= 1000;
    connection += rand() % 1000;
    return(connection);
}

connection_t TcpManager::acquire_next_connection()
{
    connection_t connection = BAD_CONNECTION;

    BaseGuard<ThreadLocker> deque_guard(m_deque_locker);
    if (m_using_connection.max_size() == m_using_connection.size())
    {
        return(connection);
    }

    while (true)
    {
        connection = m_next_connection++;
        if (BAD_CONNECTION == connection)
        {
            continue;
        }

        if (m_using_connection.end() == 
            std::find(m_using_connection.begin(), 
                      m_using_connection.end(), connection))
        {
            break;
        }
    }

    deque_guard.release();

    return(connection);
}

bool TcpManager::transform_socket(socket_t sockfd, connection_t & connection)
{
    BaseGuard<ThreadLocker> map_guard(m_map_locker);
    std::map<socket_t, connection_t>::iterator iter = 
        m_sock2connection.find(sockfd);
    if (m_sock2connection.end() != iter)
    {
        connection = iter->second;
        return(true);
    }
    map_guard.release();
    return(false);
}

bool TcpManager::transform_connection(connection_t connection, socket_t & sockfd)
{
    BaseGuard<ThreadLocker> map_guard(m_map_locker);
    std::map<connection_t, socket_t>::iterator iter = 
        m_connection2sock.find(connection);
    if (m_connection2sock.end() != iter)
    {
        sockfd = iter->second;
        return(true);
    }
    map_guard.release();
    return(false);
}

void TcpManager::add_connection(socket_t sockfd, connection_t connection)
{
    BaseGuard<ThreadLocker> map_guard(m_map_locker);
    m_sock2connection[sockfd] = connection;
    m_connection2sock[connection] = sockfd;
    map_guard.release();

    BaseGuard<ThreadLocker> deque_guard(m_deque_locker);
    m_using_connection.push_back(connection);
    deque_guard.release();
}

void TcpManager::del_connection(socket_t sockfd, connection_t connection)
{
    BaseGuard<ThreadLocker> map_guard(m_map_locker);
    m_sock2connection.erase(sockfd);
    m_connection2sock.erase(connection);
    map_guard.release();

    BaseGuard<ThreadLocker> deque_guard(m_deque_locker);
    std::deque<connection_t>::iterator iter = 
        std::find(m_using_connection.begin(), 
                  m_using_connection.end(), connection);
    if (m_using_connection.end() != iter)
    {
        m_using_connection.erase(iter);
    }
    deque_guard.release();
}

void TcpManager::del_socket(socket_t sockfd)
{
    connection_t connection = BAD_CONNECTION;
    if (!transform_socket(sockfd, connection))
    {
        return;
    }
    del_connection(sockfd, connection);
}

void TcpManager::del_connection(connection_t connection)
{
    socket_t sockfd = BAD_SOCKET;
    if (!transform_connection(connection, sockfd))
    {
        return;
    }
    del_connection(sockfd, connection);
}

bool TcpManager::connect_notice(socket_t sockfd, 
                                CONNECTION_INFO * send_buffer, 
                                bool accept)
{
    if (nullptr == m_message_handler)
    {
        return(false);
    }

    connection_t connection = acquire_next_connection();
    if (BAD_CONNECTION == connection)
    {
        return(false);
    }

    NetDataHandler::handle_connect(sockfd, send_buffer);
    add_connection(sockfd, connection);

    bool ret = true;

    if (accept)
    {
        ret = m_message_handler->on_accept(connection);
    }
    else
    {
        ret = m_message_handler->on_connect(connection);
    }

    if (!ret)
    {
        del_socket(sockfd);
    }

    return(ret);
}

NAMESPACE_COMMON_END
