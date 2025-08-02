/********************************************************
 * Description : hook of message handler
 * Data        : 2013-06-28 14:19:52
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_MESSAGE_HANDLER_BASE_H
#define COMMON_NET_MESSAGE_HANDLER_BASE_H


#include "net_common_types.h"
#include "net_tcp_manager_base.h"

#include "base_common.h"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL IMessageHandler
{
public:
    IMessageHandler() : m_tcp_manager(nullptr)
    {

    }

    virtual ~IMessageHandler()
    {

    }

public:
    void set_tcp_manager(ITcpManager * manager)
    {
        m_tcp_manager = manager;
    }

public:
    bool send_message(connection_t connection, const char * data, int size)
    {
        if (nullptr == m_tcp_manager)
        {
            return false;
        }
        return m_tcp_manager->send_message(connection, data, size);
    }

public:
    virtual bool on_accept(connection_t connection) = 0;
    virtual bool on_connect(connection_t connection) = 0;
    virtual void on_close(connection_t connection) = 0;

public:
    virtual bool handle_message(connection_t connection, const char * data, int size) = 0;

private:
    ITcpManager                          * m_tcp_manager;
};

NAMESPACE_COMMON_END


#endif // COMMON_NET_MESSAGE_HANDLER_BASE_H
