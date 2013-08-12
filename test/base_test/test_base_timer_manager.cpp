#include <iostream>

#include "test_base.h"
#include "base_time.h"
#include "base_timer_manager.h"

class Test
{
public:
    void test() const
    {
        std::cout << "void Test::test(void) const" << std::endl;
    }

    void test(int) volatile
    {
        std::cout << "void Test::test(int)" << std::endl;
    }
};

void test(void)
{
    std::cout << "void test(void)" << std::endl;
}

void test(int)
{
    std::cout << "void test(int)" << std::endl;
}

struct TEST
{
    void operator () (void) const
    {
        std::cout << "void TEST::operator () (void) const" << std::endl;
    }

    void operator () (int)
    {
        std::cout << "void TEST::operator () (int)" << std::endl;
    }
};

USING_NAMESPACE_COMMON

void test_base_timer_manager(void)
{
    TimerManager timer_manager;

    if (!timer_manager.init())
    {
        printf("timer manager init failed\n");
        return;
    }

    void (*ptr1) (void) = test;
    void (*ptr2) (int)  = test;
    void (Test::*mem1) (void) const   = &Test::test;
    void (Test::*mem2) (int) volatile = &Test::test;

    Test t;
    TEST f;

    size_t id1 = timer_manager.create_timer_ptr(ptr1);
    size_t id2 = timer_manager.create_timer_ptr(ptr2, 0);
    size_t id3 = timer_manager.create_timer_mem(t, mem1);
    size_t id4 = timer_manager.create_timer_mem(t, mem2, 0);
    size_t id5 = timer_manager.create_timer_ptr(f);
    size_t id6 = timer_manager.create_timer_ptr(f, 0);

    timer_manager.start_timer(id1, false,  200);
    timer_manager.start_timer(id2, false,  300);
    timer_manager.start_timer(id3, false,  400);
    timer_manager.start_timer(id4, false,  600);
    timer_manager.start_timer(id5, false, 1200);
    timer_manager.start_timer(id6, false, 2400);

    printf("sleep 5000ms begin...\n");
    base_millisecond_sleep(5000);
    printf("sleep 5000ms end...\n");

    timer_manager.stop_timer(id1);
    timer_manager.stop_timer(id2);
    timer_manager.stop_timer(id3);
    timer_manager.stop_timer(id4);
    timer_manager.stop_timer(id5);
    timer_manager.stop_timer(id6);

    printf("sleep 1000ms begin...\n");
    base_millisecond_sleep(1000);
    printf("sleep 1000ms end...\n");

    timer_manager.exit();
}
