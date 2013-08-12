#include "test_base.h"
#include "base_locker.h"
#include "base_guard.hpp"

USING_NAMESPACE_COMMON

void test_base_guard(void)
{
    ThreadLocker thread_locker;
    {
        BaseGuard<ThreadLocker> guard(thread_locker);
    }

    ProcessLocker process_locker;
    {
        BaseGuard<ProcessLocker> guard(process_locker);
    }
}
