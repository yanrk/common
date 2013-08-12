/********************************************************
 * Description : tcp connection class
 * Data        : 2013-06-16 01:23:54
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_NET_TCP_CONNECTION_H
#define COMMON_NET_TCP_CONNECTION_H


#ifdef WIN32
    #include "net_windows_tcp_connection.h"
#else
    #include "net_linux_tcp_connection.h"
#endif // WIN32

NAMESPACE_COMMON_BEGIN

#ifdef WIN32
    typedef WindowsTcpConnection     TcpConnection;
#else
    typedef LinuxTcpConnection       TcpConnection;
#endif // WIN32

NAMESPACE_COMMON_END


#endif // COMMON_NET_TCP_CONNECTION_H
