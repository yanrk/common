#include <iostream>

#include "test_base.h"
#include "base_string.h"

USING_NAMESPACE_COMMON

void test_base_string(void)
{
    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[29];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[30];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[31];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[32];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[33];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[34];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[35];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }

    {
        const char fmt[] = "12345678901234567890123456789012";
        char buf[36];
        int size = sizeof(buf)/sizeof(buf[0]);
        base_snprintf(buf, size, fmt);
        printf("%d - %s\n", size, buf);
    }
    std::cout << std::endl;

    std::cout << "compare str1 and str2 ignore case" << std::endl;
    {
        const char * str1 = "ABC";
        const char * str2 = "abb";
        const char * str3 = "abd";
        const char * str4 = "abc";
        const char * str5 = "abcd";

        std::cout << "compare all string" << std::endl;

        std::cout << str1 << " V.S " << str2 << " : " 
                  << base_strcmp_ignore_case(str1, str2) << std::endl;
        std::cout << str1 << " V.S " << str3 << " : " 
                  << base_strcmp_ignore_case(str1, str3) << std::endl;
        std::cout << str1 << " V.S " << str4 << " : " 
                  << base_strcmp_ignore_case(str1, str4) << std::endl;
        std::cout << str1 << " V.S " << str5 << " : " 
                  << base_strcmp_ignore_case(str1, str5) << std::endl;

        std::cout << "compare 3 character" << std::endl;

        std::cout << str1 << " V.S " << str2 << " : " 
                  << base_strncmp_ignore_case(str1, str2, 3) << std::endl;
        std::cout << str1 << " V.S " << str3 << " : " 
                  << base_strncmp_ignore_case(str1, str3, 3) << std::endl;
        std::cout << str1 << " V.S " << str4 << " : " 
                  << base_strncmp_ignore_case(str1, str4, 3) << std::endl;
        std::cout << str1 << " V.S " << str5 << " : " 
                  << base_strncmp_ignore_case(str1, str5, 3) << std::endl;
    }
    std::cout << std::endl;

    std::cout << "test c string to type" << std::endl;
    {
        const char * str = "A";
        char val = ' ';
        base_string2char(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        const char * str = "123";
        short val = 0;
        base_string2short(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        const char * str = "1234";
        int val = 0;
        base_string2int(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        const char * str = "123456789";
        long val = 0;
        base_string2long(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        const char * str = "123.456";
        float val = 0.0f;
        base_string2float(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        const char * str = "123.456789";
        double val = 0.0;
        base_string2double(str, val);
        std::cout << str << " : " << val << std::endl;
    }
    std::cout << std::endl;

    std::cout << "test c++ string to type" << std::endl;
    {
        std::string str("A");
        char val = ' ';
        base_string2char(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        std::string str("123");
        short val = 0;
        base_string2short(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        std::string str("1234");
        int val = 0;
        base_string2int(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        std::string str("123456789");
        long val = 0;
        base_string2long(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        std::string str("123.456");
        float val = 0.0f;
        base_string2float(str, val);
        std::cout << str << " : " << val << std::endl;
    }

    {
        std::string str("123.456789");
        double val = 0.0;
        base_string2double(str, val);
        std::cout << str << " : " << val << std::endl;
    }
    std::cout << std::endl;

    std::cout << "test type to c string" << std::endl;
    {
        const int size = 64;
        char str[size];
        char val = 'A';
        base_char2string(val, str, size);
        std::cout << val << " : " << str << std::endl;
    }

    {
        const int size = 64;
        char str[size];
        short val = 123;
        base_short2string(val, str, size);
        std::cout << val << " : " << str << std::endl;
    }

    {
        const int size = 64;
        char str[size];
        int val = 1234;
        base_int2string(val, str, size);
        std::cout << val << " : " << str << std::endl;
    }

    {
        const int size = 64;
        char str[size];
        long val = 123456789;
        base_long2string(val, str, size);
        std::cout << val << " : " << str << std::endl;
    }

    {
        const int size = 64;
        char str[size];
        float val = 123.456f;
        base_float2string(val, str, size);
        std::cout << val << " : " << str << std::endl;
    }

    {
        const int size = 64;
        char str[size];
        double val = 123.456789;
        base_double2string(val, str, size);
        std::cout << val << " : " << str << std::endl;
    }
    std::cout << std::endl;

    std::cout << "test type to c++ string" << std::endl;
    {
        std::string str;
        char val = 'A';
        base_char2string(val, str);
        std::cout << val << " : " << str << std::endl;
    }

    {
        std::string str;
        short val = 123;
        base_short2string(val, str);
        std::cout << val << " : " << str << std::endl;
    }

    {
        std::string str;
        int val = 1234;
        base_int2string(val, str);
        std::cout << val << " : " << str << std::endl;
    }

    {
        std::string str;
        long val = 123456789;
        base_long2string(val, str);
        std::cout << val << " : " << str << std::endl;
    }

    {
        std::string str;
        float val = 123.456f;
        base_float2string(val, str);
        std::cout << val << " : " << str << std::endl;
    }

    {
        std::string str;
        double val = 123.456789;
        base_double2string(val, str);
        std::cout << val << " : " << str << std::endl;
    }
    std::cout << std::endl;
}
