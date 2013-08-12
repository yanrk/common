#include <cassert>
#include <iostream>

#include "test_base.h"
#include "base_handle_manager.hpp"

USING_NAMESPACE_COMMON

void test_base_handle_manager(void)
{
    int min = 0;
    int max = 20;
    HandleManager<int, NullLocker> manager(min, max);

    if (!manager.release(min))
    {
        std::cout << "as i known, release " << min << " failed" << std::endl;
    }
    else
    {
        assert(false);
    }

    int handle = 0;
    while (manager.acquire(handle))
    {
        std::cout << "acquire " << handle << std::endl;
    }

    for (int i = min; i < max; i += 2)
    {
        manager.release(i);
        std::cout << "release " << i << std::endl;
    }

    if (!manager.release(min))
    {
        std::cout << "as i known, release " << min << " failed" << std::endl;
    }
    else
    {
        assert(false);
    }

    if (manager.acquire(handle))
    {
        std::cout << "acquire " << handle << std::endl;
        manager.release(handle);
        std::cout << "release " << handle << std::endl;
    }

    for (int i = max - 1; i > min; i -= 2)
    {
        manager.release(i);
        std::cout << "release " << i << std::endl;
    }

    if (!manager.release(min))
    {
        std::cout << "as i known, release " << min << " failed" << std::endl;
    }
    else
    {
        assert(false);
    }

    if (manager.acquire(handle) && min == handle)
    {
        std::cout << "acquire " << handle << std::endl;
    }
    else
    {
        assert(false);
    }
}
