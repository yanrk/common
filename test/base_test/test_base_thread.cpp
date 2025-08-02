#include "test_base.h"
#include "base_thread.h"
#include "base_locker.h"
#include "base_guard.hpp"
#include "base_time.h"

USING_NAMESPACE_COMMON

BaseThread   g_thread_1;
BaseThread   g_thread_2;
ThreadLocker g_stdout_locker;

struct TEST_THREAD
{
    BaseThread * thread;
    int          value;
};

#ifdef WIN32
static unsigned int __stdcall thread_run(void * argument)
{
    TEST_THREAD * test = reinterpret_cast<TEST_THREAD *>(argument);
    if (nullptr == test || nullptr == test->thread)
    {
        return 0;
    }

    g_stdout_locker.acquire();
    printf("Thread[%s] start\n", test->thread->thread_name().c_str());
    g_stdout_locker.release();

    while (test->thread->running())
    {
        BaseGuard<ThreadLocker> stdout_guard(g_stdout_locker);
        printf("Thread[%s] %d\n", test->thread->thread_name().c_str(), test->value);
    }

    g_stdout_locker.acquire();
    printf("Thread[%s] end\n", test->thread->thread_name().c_str());
    g_stdout_locker.release();

    return 0;
}
#else
static void * thread_run(void * argument)
{
    TEST_THREAD * test = reinterpret_cast<TEST_THREAD *>(argument);
    if (nullptr == test || nullptr == test->thread)
    {
        return nullptr;
    }

    g_stdout_locker.acquire();
    printf("Thread[%s] start\n", test->thread->thread_name().c_str());
    g_stdout_locker.release();

    while (test->thread->running())
    {
        BaseGuard<ThreadLocker> stdout_guard(g_stdout_locker);
        printf("Thread[%s] %d\n", test->thread->thread_name().c_str(), test->value);
    }

    g_stdout_locker.acquire();
    printf("Thread[%s] end\n", test->thread->thread_name().c_str());
    g_stdout_locker.release();

    return nullptr;
}
#endif // WIN32

void test_base_thread(void)
{
    TEST_THREAD thread_1 = { &g_thread_1, 1 };
    TEST_THREAD thread_2 = { &g_thread_2, 2 };

    g_thread_1.set_thread_args(thread_run, &thread_1, "thread 1");
    g_thread_2.set_thread_args(thread_run, &thread_2, "thread 2");

    g_thread_1.acquire();
    g_thread_2.acquire();

    base_nanosecond_sleep(50000000);

    g_thread_1.release();
    g_thread_2.release();
}