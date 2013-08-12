#include <cstdio>
#include <string>

#include "test_base.h"
#include "base_utility.h"

USING_NAMESPACE_COMMON

void test_base_log_config(void)
{
    std::string file("./log.ini");
    LOG_CONFIG config;
    if (!base_load_log_config(file, config))
    {
        printf("load log config failed\n");
    }
    else
    {
        printf("load log config success\n");
    }
}
