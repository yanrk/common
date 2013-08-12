#include "test_base.h"
#include "base_exception.h"

USING_NAMESPACE_COMMON

void test_base_exception(void)
{
    try
    {
        BASE_THROW("test way 1");
    }
    catch (BaseException & e)
    {
        printf("%s\n", e.message().c_str());
    }

    try
    {
        BASE_THROW("%s", "test way 2");
    }
    catch (BaseException & e)
    {
        printf("%s\n", e.message().c_str());
    }
}
