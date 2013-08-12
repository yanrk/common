#include "test_base.h"
#include "base_macro.h"

USING_NAMESPACE_COMMON

void test_base_macro(void)
{
    const char * var = nullptr;
    IF_VAR_NOT_EQUAL_VALUE_RETURN_VOID(var, nullptr);
}
