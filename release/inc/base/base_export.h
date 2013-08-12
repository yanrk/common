/********************************************************
 * Description : dynamic link library export macro
 * Data        : 2013-05-18 17:50:12
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_EXPORT_H
#define COMMON_BASE_EXPORT_H


#ifdef WIN32
    #define YRK_CDECL        __cdecl
    #define YRK_STDCALL      __stdcall
    #define YRK_EXPORT_DLL   __declspec(dllexport)
#else
    #define YRK_CDECL
    #define YRK_STDCALL
    #define YRK_EXPORT_DLL
#endif // WIN32

#define YRK_C_API(ret_type)    extern "C" YRK_EXPORT_DLL ret_type YRK_CDECL
#define YRK_CXX_API(ret_type)  extern     YRK_EXPORT_DLL ret_type YRK_CDECL


#endif // COMMON_BASE_EXPORT_H
