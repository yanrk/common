#include "base_log.h"

USING_NAMESPACE_COMMON

static void test_log_with_write_mode
(
    LOG_WRITE_MODE write_mode, 
    const char * mode
)
{
    LOG_CONFIG log_config;
    log_config.log_file_path = "./test_base_log/log/";
    if (nullptr != mode)
    {
        log_config.log_file_path += mode;
    }
    for (size_t type = LOG_TYPE_MIN; type < LOG_TYPE_MAX; ++type)
    {
        log_config.log_file[type].write_mode = write_mode;
        log_config.log_file[type].log_min_level = TRK_LEVEL;
        log_config.log_file[type].log_file_size = 10;
        log_config.log_file[type].buffer_count = 50;
    }

    log_init(log_config);

    for (size_t index = 0; index < 10000; ++index)
    {
        RUN_LOG_CRI("%s %d", "test RUN_LOG_CRI", 1);
        RUN_LOG_ERR("%s %d", "test RUN_LOG_ERR", 2);
        RUN_LOG_WAR("%s %d", "test RUN_LOG_WAR", 3);
        RUN_LOG_DBG("%s %d", "test RUN_LOG_DBG", 4);
        RUN_LOG_TRK("%s %d", "test RUN_LOG_TRK", 5);
        DBG_LOG("%s %d", "test DBG_LOG", 6);
    }

    log_exit();
}

void test_base_log(void)
{
    test_log_with_write_mode(SYNC_WRITE_MODE, "sync");
    test_log_with_write_mode(ASYN_WRITE_MODE, "asyn");
    test_log_with_write_mode(LAZY_WRITE_MODE, "lazy");
}
