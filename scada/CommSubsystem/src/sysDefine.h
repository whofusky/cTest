/*
    ƽ̨����
*/

#ifndef H_SYS_DEFINE_
#define H_SYS_DEFINE_

#pragma once

#ifdef _WIN32
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
    #ifndef WIN32
        #define WIN32
    #endif
#endif

#ifdef _WIN64
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
    #ifndef WIN64
        #define WIN64
    #endif
#endif

//�Ƿ��������Ļ���׼���
#define DEBUG               true

//������־�������(ֵԽС�������,ֵ�������ó�2��N�η�)
#define DEBUG_LEVEL_0  0  //��־����������,һ��ָ��������Ĵ�����־
#define DEBUG_LEVEL_1  1  //1����־���
#define DEBUG_LEVEL_2  2  //2����־���

#endif//H_SYS_DEFINE_