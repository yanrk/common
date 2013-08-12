#include "test_base.h"
#include "base_locker.h"

USING_NAMESPACE_COMMON

void test_base_locker(void)
{
    {
        ThreadLocker thread_locker;
        thread_locker.acquire();
        thread_locker.release();
        if (thread_locker.try_acquire())
        {
            printf("thread-locker lock\n");
            thread_locker.release();
        }
    }

    {
        ProcessLocker process_locker;
        process_locker.acquire();
        process_locker.release();
        if (process_locker.try_acquire())
        {
            printf("process-locker lock\n");
            process_locker.release();
        }
    }
}
