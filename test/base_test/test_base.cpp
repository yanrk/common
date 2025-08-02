#include "test_base.h"

int main(int argc, char * argv[])
{
    test_base_string();
    test_base_hex();
    test_base_base64();
    test_base_exception();
    test_base_byte_stream();
    test_base_memory();
    test_base_locker();
    test_base_guard();
    test_base_thread();
    test_base_object_pool();
    test_base_macro();
    test_base_time();
    test_base_log();
    test_base_directory();
    test_base_ini();
    test_base_convert();
    test_base_log_config();
    test_base_timer_manager();
    test_base_timer();
    test_base_handle_manager();
    return 0;
}
