#include <iostream>

#include "test_base.h"
#include "base_convert.hpp"

USING_NAMESPACE_COMMON

void test_base_convert(void)
{
    std::cout.setf(std::ios::fixed, std::ios::floatfield);

    std::cout << std::endl;
    std::cout << "test type to string" << std::endl;

    {
        bool b1 = false;
        std::string str;
        base_type2string(b1, str);
        std::cout << b1 << " : " << str << std::endl;
    }

    {
        bool b2 = true;
        std::string str;
        base_type2string(b2, str);
        std::cout << b2 << " : " << str << std::endl;
    }

    {
        char c = 'a';
        std::string str;
        base_type2string(c, str);
        std::cout << c << " : " << str << std::endl;
    }

    {
        short s = 123;
        std::string str;
        base_type2string(s, str);
        std::cout << s << " : " << str << std::endl;
    }

    {
        int i = 13579;
        std::string str;
        base_type2string(i, str);
        std::cout << i << " : " << str << std::endl;
    }

    {
        int l = 123456789;
        std::string str;
        base_type2string(l, str);
        std::cout << l << " : " << str << std::endl;
    }

    {
        float f = 123.4567f;
        std::string str;
        base_type2string(f, str);
        std::cout << f << " : " << str << std::endl;
    }

    {
        double d = 123456.1234567;
        std::string str;
        base_type2string(d, str);
        std::cout << d << " : " << str << std::endl;
    }

    std::cout << std::endl;
    std::cout << "test string to type" << std::endl;

    {
        std::string str("false");
        bool b1 = true;
        base_string2type(str, b1);
        std::cout << str << " : " << b1 << std::endl;
    }

    {
        std::string str("true");
        bool b2 = false;
        base_string2type(str, b2);
        std::cout << str << " : " << b2 << std::endl;
    }

    {
        std::string str("a");
        char c = ' ';
        base_string2type(str, c);
        std::cout << str << " : " << c << std::endl;
    }

    {
        std::string str("123");
        short s = 0;
        base_string2type(str, s);
        std::cout << str << " : " << s << std::endl;
    }

    {
        std::string str("13579");
        int i = 0;
        base_string2type(str, i);
        std::cout << str << " : " << i << std::endl;
    }

    {
        std::string str("123456789");
        int l = 0;
        base_string2type(str, l);
        std::cout << str << " : " << l << std::endl;
    }

    {
        std::string str("123.4567");
        float f = 0.0f;
        base_string2type(str, f);
        std::cout << str << " : " << f << std::endl;
    }

    {
        std::string str("123456.1234567");
        double d = 0.0;
        base_string2type(str, d);
        std::cout << str << " : " << d << std::endl;
    }

    std::cout << std::endl;

    std::cout.unsetf(std::ios::floatfield);
}
