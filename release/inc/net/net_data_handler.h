/********************************************************
 * Description : data handler class
 * Data        : 2013-06-28 08:17:52
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_DATA_HANDLER_H
#define COMMON_NET_DATA_HANDLER_H


#include <vector>
#include <list>
#include <map>

#include "net_common_types.h"
#include "net_connection_info.h"
#include "base_locker.h"
#include "base_thread.h"
#include "base_object_pool.hpp"
#include "base_memory_pool.hpp"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL NetDataHandler
{
public:
    NetDataHandler();
    virtual ~NetDataHandler();

public:
    void handle_thread_run(int index);

protected:
    bool init();
    bool exit();

protected:
    bool set_thread_count(int thread_count);

protected:
    void handle_connect(socket_t sockfd, CONNECTION_INFO * send_buffer);
    void handle_send(socket_t sockfd);
    void handle_recv(socket_t sockfd, const char * data, int size);
    void handle_close(socket_t sockfd);

protected:
    bool send_data(socket_t sockfd, const char * data, int size);

private:
    virtual bool handle_data(socket_t sockfd, const char * data, int size) = 0;

private:
    virtual bool continue_send(CONNECTION_INFO * send_buffer) = 0;
    virtual bool suspend_send(CONNECTION_INFO * send_buffer) = 0;

private:
    bool create_handle_thread();
    bool save_send_data(socket_t sockfd, const char * data, int size);

private:
    struct RecvNode
    {
        char          temp_length;
        char          temp_buffer[sizeof(uint32_t)];
        int           memory_size;
        MEMORY_NODE * memory_node;

        RecvNode();
    };

    struct RecvData
    {
        socket_t      sockfd;
        MEMORY_NODE * memory_node;
    };

    struct SendNode
    {
        const int         capacity;
        int               length;
        CONNECTION_INFO * send_buffer;
        char              buffer[MAX_TCP_PKG_SIZE];

        SendNode();
    };

private:
    bool try_send(std::list<SendNode *> & send_node_list);

private:
    bool                                          m_init;
    int                                           m_handle_thread_count;
    MemoryPool<ThreadLocker>                      m_memory_pool;
    ObjectPool<RecvNode, ThreadLocker>            m_recv_node_pool;
    ObjectPool<SendNode, ThreadLocker>            m_send_node_pool;
    std::map< socket_t, RecvNode * >              m_recv_node;
    std::vector< std::list< RecvData > >          m_recv_data;
    std::map< socket_t, std::list<SendNode *> >   m_send_data;
    ThreadLocker                                  m_recv_node_locker;
    ThreadLocker                                * m_recv_data_locker;
    ThreadLocker                                  m_send_data_locker;
    BaseThread                                  * m_handle_thread;
};

NAMESPACE_COMMON_END


#endif // COMMON_NET_DATA_HANDLER_H
