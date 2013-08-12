#include "test_base.h"
#include "base_memory.h"
#include "base_exception.h"

USING_NAMESPACE_COMMON

void test_base_memory(void)
{
    {
        int * i = NULL;
        BASE_NEW(i, int);
        *i = 5;
        BASE_DELETE(i);
        assert(NULL == i);
    }

    {
        int * i = NULL;
        BASE_NEW(i, int(1));
        assert(1 == *i);
        BASE_DELETE(i);
        assert(NULL == i);
    }

    {
        int * i = NULL;
        BASE_NEW_A(i, int, 1);
        i[0] = 5;
        BASE_DELETE_A(i);
        assert(NULL == i);
    }

    {
        std::string * s = NULL;
        BASE_NEW(s, std::string);
        *s = "hello world";
        BASE_DELETE(s);
        assert(NULL == s);
    }

    {
        std::string * s = NULL;
        BASE_NEW(s, std::string("hello world"));
        assert("hello world" == *s);
        BASE_DELETE(s);
        assert(NULL == s);
    }

    {
        std::string * s = NULL;
        BASE_NEW_A(s, std::string, 1);
        s[0] = "hello world";
        BASE_DELETE_A(s);
        assert(NULL == s);
    }
}
