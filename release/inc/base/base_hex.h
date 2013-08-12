/********************************************************
 * Description : binary to hexadecimal conversion
 * Data        : 2013-05-19 11:53:27
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_HEX_H
#define COMMON_BASE_HEX_H


#include "base_common.h"

NAMESPACE_COMMON_BEGIN

#define HEX_ENCODE_SIZE(in_len) (in_len * 2 + 1)
#define HEX_DECODE_SIZE(in_len) (in_len / 2 + 1)

YRK_C_API(bool)
    binary_to_hex
    (
        const unsigned char src, 
        char dst[2]
    );

YRK_C_API(bool) 
    hex_to_binary
    (
        const char src[2], 
        unsigned char * dst
    );

YRK_CXX_API(bool) 
    hex_encode
    (
        const unsigned char * src, 
        size_t src_len, 
        char * dst, 
        size_t dst_len
    );

YRK_CXX_API(bool) 
    hex_decode
    (
        const char * src, 
        unsigned char * dst, 
        size_t dst_len
    );

YRK_CXX_API(bool) 
    hex_encode
    (
        const void * src, 
        size_t src_len, 
        char * dst, 
        size_t dst_len
    );

YRK_CXX_API(bool) 
    hex_decode
    (
        const char * src, 
        void * dst, 
        size_t dst_len
    );

YRK_CXX_API(bool) 
    hex_encode
    (
        const char * src, 
        char * dst, 
        size_t dst_len
    );

YRK_CXX_API(bool) 
    hex_decode
    (
        const char * src, 
        char * dst, 
        size_t dst_len
    );

NAMESPACE_COMMON_END


#endif // COMMON_BASE_HEX_H
