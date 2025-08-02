#include "test_net_message_test.h"

static const char msg_block[] = "this is a message";
static const int msg_size = sizeof(msg_block) / sizeof(msg_block[0]);
static const int MAX_MSG_COUNT = 2000;

USING_NAMESPACE_COMMON

MessageTest::MessageTest()
    : m_send_data(false)
    , m_memory_pool()
    , m_connections()
    , m_locker()
{

}

MessageTest::~MessageTest()
{
    m_connections.clear();
    m_memory_pool.clear();
}

void MessageTest::set_send_data(bool send_data)
{
    m_send_data = send_data;
}

bool MessageTest::on_accept(connection_t connection)
{
    add_connection(connection);
    return true;
}

bool MessageTest::on_connect(connection_t connection)
{
    add_connection(connection);
    if (m_send_data)
    {
        test_send_message(connection);
    }
    return true;
}

void MessageTest::on_close(connection_t connection)
{
    del_connection(connection);
}

bool MessageTest::test_send_message(connection_t connection)
{
    MEMORY_NODE * memory_node = acquire_send_message(connection);
    if (nullptr == memory_node)
    {
        return false;
    }

    DBG_LOG("connection %d send %06d bytes", connection, memory_node->size);
    printf("connection %d send %06d bytes\n", connection, memory_node->size);

    send_message(connection, memory_node->buffer, memory_node->size);

    release_send_message(memory_node);

    return true;
}

bool MessageTest::handle_message(connection_t connection, const char * data, int size)
{
    if (!MessageTest::check_recv_message(connection, data, size))
    {
        return false;
    }

    DBG_LOG("connection %d recv %06d bytes", connection, size);
    printf("connection %d recv %06d bytes\n", connection, size);

    test_send_message(connection);

    return true;
}

void MessageTest::add_connection(connection_t connection)
{
    BaseGuard<ThreadLocker> guard(m_locker);
    m_connections[connection] = 0;
    guard.release();
}

void MessageTest::del_connection(connection_t connection)
{
    BaseGuard<ThreadLocker> guard(m_locker);
    m_connections.erase(connection);
    guard.release();
}

MEMORY_NODE * MessageTest::acquire_send_message(connection_t connection)
{
    BaseGuard<ThreadLocker> guard(m_locker);
    int block_count = m_connections[connection];
    guard.release();

    block_count = block_count % MAX_MSG_COUNT + 1;
    int memory_len = (msg_size - 1) * block_count + 1;
    MEMORY_NODE * memory_node = m_memory_pool.acquire(memory_len);
    if (nullptr == memory_node)
    {
        RUN_LOG_ERR("memory pool acquire %d failed on connection %d", memory_len, connection);
        return nullptr;
    }

    for (int index = 0; index < block_count; ++index)
    {
        memcpy(memory_node->buffer + memory_node->size, msg_block, msg_size - 1);
        memory_node->size += msg_size - 1;
    }
    memory_node->buffer[memory_node->size] = '\0';
    memory_node->size += 1;

    guard.acquire();
    m_connections[connection] = block_count;
    guard.release();

    return memory_node;
}

void MessageTest::release_send_message(MEMORY_NODE *& memory_node)
{
    m_memory_pool.release(memory_node);
}

bool MessageTest::check_recv_message(connection_t connection, const char * data, int size)
{
    BaseGuard<ThreadLocker> guard(m_locker);
    int block_count = m_connections[connection];
    guard.release();

    block_count = block_count % MAX_MSG_COUNT + 1;
    int guess_recv_len = (msg_size - 1) * block_count + 1;
    if (size != guess_recv_len)
    {
        RUN_LOG_ERR("connection %d msg length error: need_len %d, recv_len %d", connection, guess_recv_len, size);
        return false;
    }

    for (int index = 0; index < block_count; ++index)
    {
        if (0 != memcmp(data, msg_block, msg_size - 1))
        {
            RUN_LOG_ERR("connection %d msg content error: %s", connection, data);
        }
        data += msg_size - 1;
        size -= msg_size - 1;
    }
    if ('\0' != data[size - 1])
    {
        RUN_LOG_ERR("connection %d msg content error: %s", connection, data);
    }

    guard.acquire();
    m_connections[connection] = block_count;
    guard.release();

    return true;
}
