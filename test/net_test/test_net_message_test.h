#ifndef COMMON_NET_TEST_MESSAGE_TEST_H
#define COMMON_NET_TEST_MESSAGE_TEST_H


#include <cstring>
#include <map>

#include "net_common_types.h"
#include "net_message_handler_base.h"
#include "base_memory_pool.hpp"
#include "base_locker.h"

USING_NAMESPACE_COMMON

class MessageTest : public IMessageHandler
{
public:
    MessageTest();
    virtual ~MessageTest();

public:
    void set_send_data(bool send_data);

public:
    virtual bool on_accept(connection_t connection);
    virtual bool on_connect(connection_t connection);
    virtual void on_close(connection_t connection);

public:
    virtual bool handle_message(connection_t connection, const char * data, int size);

private:
    void add_connection(connection_t connection);
    void del_connection(connection_t connection);
    MEMORY_NODE * acquire_send_message(connection_t connection);
    void release_send_message(MEMORY_NODE *& memory_node);
    bool check_recv_message(connection_t connection, const char * data, int size);
    bool test_send_message(connection_t connection);

private:
    bool                                          m_send_data;
    MemoryPool<ThreadLocker>                      m_memory_pool;
    std::map<connection_t, int>                   m_connections;
    ThreadLocker                                  m_locker;
};


#endif // COMMON_NET_TEST_MESSAGE_TEST_H
