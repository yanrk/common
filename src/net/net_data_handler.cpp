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

#include <cstring>

#include "net_data_handler.h"
#include "base_guard.hpp"
#include "base_log.h"
#include "base_byte_stream.h"
#include "base_time.h"
#include "base_macro.h"

NAMESPACE_COMMON_BEGIN

struct HANDLE_MESSAGE_PARAM
{
    NetDataHandler & data_handler;
    int              thread_index;

    HANDLE_MESSAGE_PARAM(NetDataHandler & handler, int index);
};

HANDLE_MESSAGE_PARAM::HANDLE_MESSAGE_PARAM(NetDataHandler & handler, int index)
    : data_handler(handler), thread_index(index)
{

}

static YRK_THR_FUNC_RET_T YRK_STDCALL handle_msg_run(void * argument)
{
    if (nullptr != argument)
    {
        HANDLE_MESSAGE_PARAM * handle_msg_param = 
            reinterpret_cast<HANDLE_MESSAGE_PARAM *>(argument);
        NetDataHandler & data_handler = handle_msg_param->data_handler;
        int thread_index = handle_msg_param->thread_index;
        data_handler.handle_thread_run(thread_index);
        BASE_DELETE(handle_msg_param);
    }
    return THREAD_DEFAULT_RET;
}

NetDataHandler::RecvNode::RecvNode()
    : temp_length(0)
    , memory_node(nullptr)
{

}

NetDataHandler::SendNode::SendNode() 
    : capacity(sizeof(buffer) / sizeof(buffer[0]))
    , length(0)
    , send_buffer(nullptr)
{

}

NetDataHandler::NetDataHandler()
    : m_init(false)
    , m_handle_thread_count(1)
    , m_memory_pool()
    , m_recv_node_pool()
    , m_send_node_pool()
    , m_recv_node()
    , m_recv_data()
    , m_send_data()
    , m_recv_node_locker("recv node locker")
    , m_recv_data_locker(nullptr)
    , m_send_data_locker("send data locker")
    , m_handle_thread(nullptr)
{

}

NetDataHandler::~NetDataHandler()
{
    exit();
}

bool NetDataHandler::set_thread_count(int thread_count)
{
    if (m_init)
    {
        return false;
    }

    m_handle_thread_count = (0 >= thread_count ? 1 : thread_count);

    return true;
}

bool NetDataHandler::init()
{
    exit();

    m_init = true;

    BASE_NEW_A(m_recv_data_locker, ThreadLocker, m_handle_thread_count);
    if (nullptr == m_recv_data_locker)
    {
        RUN_LOG_CRI("create recv_data_locker failed");
        return false;
    }

    m_recv_data.resize(m_handle_thread_count);

    if (!create_handle_thread())
    {
        return false;
    }

    return true;
}

bool NetDataHandler::exit()
{
    if (!m_init)
    {
        return true;
    }

    m_init = false;

    if (nullptr != m_handle_thread)
    {
        for (int index = 0; index < m_handle_thread_count; ++index)
        {
            BaseThread & handle_thread = m_handle_thread[index];
            handle_thread.release();
        }
    }

    BASE_DELETE_A(m_recv_data_locker);

    if (nullptr == m_handle_thread)
    {
        return true;
    }

    BASE_DELETE_A(m_handle_thread);

    std::list<RecvNode *> recv_node_list;
    std::map< socket_t, RecvNode * >::iterator recv_node_iter = 
        m_recv_node.begin();
    while (m_recv_node.end() != recv_node_iter)
    {
        recv_node_list.push_back(recv_node_iter->second);
        ++recv_node_iter;
    }
    m_recv_node.clear();
    m_recv_node_pool << recv_node_list;

    for (int index = 0; index < m_handle_thread_count; ++index)
    {
        std::list<RecvData> & recv_data_list = m_recv_data[index];
        std::list<RecvData>::iterator recv_data_iter = recv_data_list.begin();
        while (recv_data_list.end() != recv_data_iter)
        {
            m_memory_pool.release(recv_data_iter->memory_node);
            ++recv_data_iter;
        }
        recv_data_list.clear();
    }
    m_recv_data.clear();

    std::list<SendNode *> send_node_list;
    std::map< socket_t, std::list<SendNode *> >::iterator send_node_iter = 
        m_send_data.begin();
    while (m_send_data.end() != send_node_iter)
    {
        send_node_list.splice(send_node_list.end(), send_node_iter->second);
        ++send_node_iter;
    }
    m_send_data.clear();
    m_send_node_pool << send_node_list;

    m_memory_pool.clear();
    m_recv_node_pool.clear();
    m_send_node_pool.clear();

    return true;
}

void NetDataHandler::handle_connect(socket_t sockfd, 
                                    CONNECTION_INFO * send_buffer)
{
    if (BAD_SOCKET == sockfd || nullptr == send_buffer)
    {
        RUN_LOG_ERR("invalid arguments, sockfd:%d%s", sockfd, 
            nullptr == send_buffer ? ", send_buffer:nullptr" : "");
        return;
    }

    RecvNode * recv_node = m_recv_node_pool.acquire();
    if (nullptr != recv_node)
    {
        recv_node->temp_length = 0;
        recv_node->memory_size = 0;
        recv_node->memory_node = nullptr;
    }
    else
    {
        RUN_LOG_ERR("acquire recv_node failed");
    }

    BaseGuard<ThreadLocker> recv_node_guard(m_recv_node_locker);
    bool insert = m_recv_node.insert(std::make_pair(sockfd, recv_node)).second;
    recv_node_guard.release();

    if (!insert)
    {
        RUN_LOG_ERR("insert recv_node failed");
        m_recv_node_pool.release(recv_node);
    }

    SendNode * send_node = m_send_node_pool.acquire();
    if (nullptr != send_node)
    {
        send_node->length = 0;
        send_node->send_buffer = send_buffer;
    }
    else
    {
        RUN_LOG_ERR("acquire send_node failed");
    }

    std::list<SendNode *> send_node_list;
    send_node_list.push_back(send_node);

    BaseGuard<ThreadLocker> send_data_guard(m_send_data_locker);
    insert = m_send_data.insert(std::make_pair(sockfd, send_node_list)).second;
    send_data_guard.release();

    if (!insert)
    {
        RUN_LOG_ERR("insert send_node_list failed");
        m_send_node_pool << send_node_list;
    }
}

void NetDataHandler::handle_send(socket_t sockfd)
{
    BaseGuard<ThreadLocker> send_data_guard(m_send_data_locker);

    std::map< socket_t, std::list<SendNode *> >::iterator iter = 
        m_send_data.find(sockfd);
    if (m_send_data.end() == iter)
    {
        return;
    }

    std::list<SendNode *> & send_node_list = iter->second;
    if (send_node_list.empty())
    {
        return;
    }

    try_send(send_node_list);

    send_data_guard.release();
}

void NetDataHandler::handle_recv(socket_t sockfd, const char * data, int size)
{
    std::list<RecvData> a_recv_data_list;

    BaseGuard<ThreadLocker> recv_node_guard(m_recv_node_locker);

    std::map< socket_t, RecvNode * >::iterator iter = 
        m_recv_node.find(sockfd);
    if (m_recv_node.end() == iter)
    {
        return;
    }

    RecvNode * recv_node = iter->second;

    do
    {
        if (nullptr == recv_node->memory_node)
        {
            int copy_size = sizeof(uint32_t) - recv_node->temp_length;
            if (copy_size > size)
            {
                copy_size = size;
            }

            memcpy(recv_node->temp_buffer + recv_node->temp_length, 
                   data, copy_size);

            data += copy_size;
            size -= copy_size;
            recv_node->temp_length += copy_size;

            if (sizeof(uint32_t) == recv_node->temp_length)
            {
                uint32_t msg_size = 0;
                memcpy(&msg_size, recv_node->temp_buffer, sizeof(msg_size));
                ByteStream::net_to_host(&msg_size, sizeof(msg_size));
                recv_node->memory_size = msg_size;
                recv_node->memory_node = 
                    m_memory_pool.acquire(recv_node->memory_size);
                if (nullptr == recv_node->memory_node)
                {
                    RUN_LOG_ERR("acquire recv_node failed");
                    break;
                }

                recv_node->memory_node->size = 0;
            }

            if (0 == size)
            {
                break;
            }
        }

        int copy_size = recv_node->memory_size - recv_node->memory_node->size;
        if (copy_size > size)
        {
            copy_size = size;
        }

        MEMORY_NODE * memory_node = recv_node->memory_node;
        memcpy(memory_node->buffer + memory_node->size, data, copy_size);
        memory_node->size += copy_size;
        data += copy_size;
        size -= copy_size;

        if (memory_node->size == recv_node->memory_size)
        {
            recv_node->temp_length = 0;
            recv_node->memory_size = 0;
            recv_node->memory_node = nullptr;

            RecvData recv_data = { sockfd, memory_node };
            a_recv_data_list.push_back(recv_data);
        }

        if (0 == size)
        {
            break;
        }
    } while (true);

    recv_node_guard.release();

    if (!a_recv_data_list.empty())
    {
        int index = sockfd % m_handle_thread_count;
        BaseGuard<ThreadLocker> recv_data_guard(m_recv_data_locker[index]);
        std::list<RecvData> & recv_data_list = m_recv_data[index];
        recv_data_list.splice(recv_data_list.end(), a_recv_data_list);
        recv_data_guard.release();
    }
}

void NetDataHandler::handle_close(socket_t sockfd)
{
    RecvNode * recv_node = nullptr;
    BaseGuard<ThreadLocker> recv_node_guard(m_recv_node_locker);
    std::map< socket_t, RecvNode * >::iterator recv_iter = 
        m_recv_node.find(sockfd);
    if (m_recv_node.end() != recv_iter)
    {
        recv_node = recv_iter->second;
        m_recv_node.erase(recv_iter);
    }
    recv_node_guard.release();
    m_recv_node_pool.release(recv_node);

    std::list<MEMORY_NODE *> memory_node_list;
    int index = sockfd % m_handle_thread_count;
    BaseGuard<ThreadLocker> recv_data_guard(m_recv_data_locker[index]);
    std::list<RecvData> & recv_data_list = m_recv_data[index];
    std::list<RecvData>::iterator recv_data_iter = recv_data_list.begin();
    while (recv_data_list.end() != recv_data_iter)
    {
        std::list<RecvData>::iterator iter_check = recv_data_iter;
        ++recv_data_iter;
        if (sockfd == iter_check->sockfd)
        {
            memory_node_list.push_back(iter_check->memory_node);
            recv_data_list.erase(iter_check);
        }
    }
    recv_data_guard.release();
    std::list<MEMORY_NODE *>::iterator memory_iter = memory_node_list.begin();
    while (memory_node_list.end() != memory_iter)
    {
        m_memory_pool.release(*memory_iter);
        ++memory_iter;
    }
    memory_node_list.clear();

    std::list<SendNode *> send_node_list;
    BaseGuard<ThreadLocker> send_data_guard(m_send_data_locker);
    std::map< socket_t, std::list<SendNode *> >::iterator send_iter = 
        m_send_data.find(sockfd);
    if (m_send_data.end() != send_iter)
    {
        send_node_list.splice(send_node_list.end(), send_iter->second);
        m_send_data.erase(send_iter);
    }
    send_data_guard.release();
    m_send_node_pool << send_node_list;
}

bool NetDataHandler::save_send_data(socket_t sockfd, const char * data, int size)
{
    BaseGuard<ThreadLocker> send_data_guard(m_send_data_locker);

    std::map< socket_t, std::list<SendNode *> >::iterator iter = 
        m_send_data.find(sockfd);
    if (m_send_data.end() == iter)
    {
        return false;
    }

    SendNode * send_node = nullptr;
    std::list<SendNode *> & send_node_list = iter->second;
    if (send_node_list.empty())
    {
        RUN_LOG_ERR("send node list is empty");
        return false;
    }
    else
    {
        send_node = send_node_list.back();
    }

    do
    {
        int copy_size = send_node->capacity - send_node->length;
        if (copy_size > size)
        {
            copy_size = size;
        }

        memcpy(send_node->buffer + send_node->length, data, copy_size);

        send_node->length += copy_size;
        data += copy_size;
        size -= copy_size;

        if (0 == size)
        {
            break;
        }

        if (send_node->capacity == send_node->length)
        {
            CONNECTION_INFO * send_buffer = send_node->send_buffer;
            send_node = m_send_node_pool.acquire();
            if (nullptr == send_node)
            {
                RUN_LOG_ERR("acquire send_node failed");
                return false;
            }
            send_node->send_buffer = send_buffer;
            send_node->length = 0;
            send_node_list.push_back(send_node);
        }
    } while (true);

    try_send(send_node_list);

    send_data_guard.release();

    return true;
}

bool NetDataHandler::create_handle_thread()
{
    BASE_NEW_A(m_handle_thread, BaseThread, m_handle_thread_count);
    if (nullptr == m_handle_thread)
    {
        RUN_LOG_CRI("create handle_thread failed");
        return false;
    }

    for (int index = 0; index < m_handle_thread_count; ++index)
    {
        HANDLE_MESSAGE_PARAM * handle_msg_param = nullptr;
        BASE_NEW(handle_msg_param, HANDLE_MESSAGE_PARAM(*this, index));
        if (nullptr == handle_msg_param)
        {
            RUN_LOG_CRI("create handle_msg_param failed");
            return false;
        }

        BaseThread & handle_thread = m_handle_thread[index];
        handle_thread.set_thread_args(handle_msg_run, handle_msg_param);
        if (!handle_thread.acquire())
        {
            BASE_DELETE(handle_msg_param);
            RUN_LOG_CRI("the %dth handle thread acquire failed", index + 1);
            return false;
        }
    }

    return true;
}

void NetDataHandler::handle_thread_run(int index)
{
    RUN_LOG_DBG("tcp handler handle message thread[%d] begin", index + 1);

    while (m_init)
    {
        std::list<RecvData> recv_data_list;
        BaseGuard<ThreadLocker> recv_data_guard(m_recv_data_locker[index]);
        recv_data_list.swap(m_recv_data[index]);
        recv_data_guard.release();

        if (recv_data_list.empty())
        {
            base_nanosecond_sleep(BASE_BIG_INT(50000000));
            continue;
        }

        std::list<RecvData>::iterator recv_data_iter = recv_data_list.begin();
        while (recv_data_list.end() != recv_data_iter)
        {
            MEMORY_NODE * memory_node = recv_data_iter->memory_node;
            handle_data(recv_data_iter->sockfd, 
                memory_node->buffer, memory_node->size);
            m_memory_pool.release(memory_node);
            ++recv_data_iter;
        }
        recv_data_list.clear();
    }

    RUN_LOG_DBG("tcp handler handle message thread[%d] finish", index + 1);
}

bool NetDataHandler::send_data(socket_t sockfd, const char * data, int size)
{
    uint32_t data_len = static_cast<uint32_t>(size);
    ByteStream::host_to_net(&data_len, sizeof(data_len));

    MEMORY_NODE * memory_node = m_memory_pool.acquire(sizeof(data_len) + size);
    if (nullptr == memory_node)
    {
        RUN_LOG_ERR("acquire memory_node failed");
        return false;
    }

    memory_node->size = 0;
    memcpy(memory_node->buffer, &data_len, sizeof(data_len));
    memory_node->size += sizeof(data_len);
    memcpy(memory_node->buffer + memory_node->size, data, size);
    memory_node->size += size;

    bool save_ret = save_send_data(sockfd, memory_node->buffer, 
        memory_node->size);

    m_memory_pool.release(memory_node);

    return save_ret;
}

bool NetDataHandler::try_send(std::list<SendNode *> & send_node_list)
{
    if (send_node_list.empty())
    {
        return false;
    }

    SendNode * send_node = send_node_list.front();
    CONNECTION_INFO * send_buffer = send_node->send_buffer;
    if (send_buffer->empty() && 0 < send_node->length)
    {
        send_buffer->write_data(send_node->buffer, 
            send_node->length);
        if (send_node_list.back() != send_node)
        {
            send_node_list.pop_front();
            m_send_node_pool.release(send_node);
        }
        else
        {
            send_node->length = 0;
        }
    }

    if (send_buffer->empty())
    {
        suspend_send(send_buffer);
        return false;
    }
    else
    {
        continue_send(send_buffer);
        return true;
    }
}

NAMESPACE_COMMON_END
