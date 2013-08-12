#include <cstdio>
#include <cstring>
#include <string>

#include "net_tcp_manager.h"
#include "base_log.h"
#include "base_time.h"
#include "base_ini.h"
#include "base_utility.h"
#include "test_net.h"
#include "test_net_message_test.h"

struct CONFIG_INFO
{
    bool                    server;
    bool                    test_data;
    int                     thread_count;
    std::string             host_name;
    unsigned short          host_port;
    int                     connect_count;
};

USING_NAMESPACE_COMMON

bool get_config(CONFIG_INFO & config_info)
{
    const char * const ini_file = "./message_test.ini";

    BaseIni ini;
    if (!ini.load(ini_file))
    {
        printf("load %s failed\n", ini_file);
        return(false);
    }

    if (!ini.get_value("", "server", config_info.server))
    {
        printf("get value %s failed\n", "server");
        return(false);
    }

    if (!ini.get_value("", "test_data", config_info.test_data))
    {
        printf("get value %s failed\n", "test_data");
        return(false);
    }

    if (!ini.get_value("", "message_handle_thread_count", config_info.thread_count))
    {
        printf("get value %s failed\n", "message_handle_thread_count");
        return(false);
    }

    if (config_info.server)
    {
        if (!ini.get_value("server", "local_host_name", config_info.host_name))
        {
            printf("get value %s/%s failed\n", "server", "local_host_name");
            return(false);
        }

        if (!ini.get_value("server", "local_host_port", config_info.host_port))
        {
            printf("get value %s/%s failed\n", "server", "local_host_port");
            return(false);
        }
    }
    else
    {
        if (!ini.get_value("client", "remote_host_name", config_info.host_name))
        {
            printf("get value %s/%s failed\n", "client", "remote_host_name");
            return(false);
        }

        if (!ini.get_value("client", "remote_host_port", config_info.host_port))
        {
            printf("get value %s/%s failed\n", "client", "remote_host_port");
            return(false);
        }

        if (!ini.get_value("client", "connect_count", config_info.connect_count))
        {
            printf("get value %s/%s failed\n", "client", "connect_count");
            return(false);
        }
    }

    ini.clear();

    return(true);
}

void test_tcp_manager(void)
{
    LOG_CONFIG log_config;
    if (!base_load_log_config("./log.ini", log_config))
    {
        printf("load log config failed\n");
        return;
    }

    log_init(log_config);

    printf("log service start...\n");

    CONFIG_INFO config_info;
    if (!get_config(config_info))
    {
        return;
    }

    MessageTest message_handler;
    message_handler.set_send_data(config_info.test_data);

    TcpManager manager;

    if (config_info.server)
    {
        manager.set_local_port(config_info.host_port);
        manager.set_data_handle_thread_count(config_info.thread_count);
        manager.set_message_handler(&message_handler);
    }
    else
    {
        manager.set_remote_address(config_info.host_name.c_str(), 
                                   config_info.host_port);
        manager.set_data_handle_thread_count(config_info.thread_count);
        manager.set_message_handler(&message_handler);
    }

    if (!manager.start())
    {
        printf("tcp %s start failed\n", (config_info.server ? "server" : "client"));
        log_exit();
        printf("log service exit...\n");
        return;
    }

    printf("tcp %s start...\n", (config_info.server ? "server" : "client"));

    if (!config_info.server)
    {
        printf("\n");
        for (int i = 1; i <= config_info.connect_count; ++i)
        {
            if (manager.create_connection())
            {
                printf("tcp client create connection %d success...\n", i);
            }
            else
            {
                printf("tcp client create connection %d failed...\n", i);
                break;
            }
        }
    }

    const int MAX_BUFF_SIZE = 4096;
    char buffer[MAX_BUFF_SIZE];
    while (true)
    {
        printf("\ninput \"stop\" / \"exit\" / \"quit\" to exit\n");
        int i = scanf("%s", buffer);
        if (1 != i)
        {
            continue;
        }
        if (0 == strcmp("stop", buffer) || 
            0 == strcmp("exit", buffer) || 
            0 == strcmp("quit", buffer))
        {
            break;
        }
    }
    printf("\n");

    manager.stop();

    printf("tcp %s exit...\n", (config_info.server ? "server" : "client"));

    log_exit();

    printf("log service exit...\n");

    base_millisecond_sleep(2000);
}
