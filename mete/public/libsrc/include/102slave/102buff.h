#ifndef _102BUFF_H_
#define _102BUFF_H_

#include <vector>
#include <string>
#include "mutex/Mutex.h"
#include <sys/time.h>

using namespace std;

class Time_str
{
    public:
        int m_Year;    //年
        int m_Month;   //月
        int m_Day;     //日
        int m_Hour;    //小时
        int m_Minute;  //分钟
        int m_Second;  //秒
        int m_tv_usec;  //微秒
        time_t m_t;

    public:
        //取得从纪元开始至今的秒数赋值于m_t
        //根据m_t的值取本地时间赋值于m_Year~m_Second
        Time_str();
        ~Time_str();

        //取得从纪元开始至今的秒数+v_Minutet赋值于m_t
        //根据m_t的值取本地时间赋值于m_Year~m_Second
        void getLocalTime ( const int& v_Minute = 0 );

        //向后或向前第v_day天的00:00至纪元开始的秒数赋值于m_t
        void getLocalTime_day ( const int& v_day = 0 );

        //根据m_t的值+v_Minutet赋值于m_t
        //根据m_t的值取本地时间赋值于m_Year~m_Second
        void getLocalTime_day_hour_minute ( const int& v_Minute = 0 );

};

#define  BUFF_STATE_EMPTY 2  //Buff102的缓存区的状态 空闲可用
#define  BUFF_STATE_USING 1  //Buff102的缓存区的状态 占用不可用

#define  MAXBUFFLEN       1024*1024 //Buff102的缓存区的长度

class Buff_102
{
    public:
        Buff_102();
        ~Buff_102();
    private:
        CMutex m_Mtx_Buff;  //锁变量

        int    m_Stat;           //数据区的使用状态

    private:

        unsigned char* m_Buffer; //数据区指针
        int    m_NextFrame;      //当前的发送位置
        string m_Filename;       //数据区对应的文件名
        int    m_BufferLen;      //数据区对应的文件的长度

        int    m_DataType;       //数据区中数据的种类
        int    m_TryNumber;      //数据区中地数据已经被尝试发送的次数
        int    m_yzcd;           //文件验证时用的长度

    public:
        /******************************外部接口***************************/
        int clearBuffFun();//清空缓冲区
        int resetBuffFun();//复位缓冲区
        /*将content内容复制到m_Buffer
          返回: 0 所有 content复制到m_Buffer
                1 content长度超出m_Buffer长度,只复制m_Buffer可容纳的长度
        */
        int getBuffFun ( const char* content,    //需要复制的内容
                         const string& file_name,  //内容来自的文件名
                         const int& len            //content是长度
                       );

        //m_Stat == BUFF_STATE_EMPTY ? false :true
        //判断当前缓冲区是否可用 true在用 false不在用
        bool isUsing();

        unsigned char* getBufferP();

        void setNextFrame ( const int& next );
        int getNextFrame();

        //取值m_Filename
        string getFileName();

        int getm_BufferLen();

        //取值m_DataType
        int  getType();
        //赋值m_DataType=v_DataType
        void setType ( const int& v_DataType );



        void setTryNumber ( const int& tryNumber );
        int&  getTryNumber();

        void setyzcd ( const int& len );
        int  getyzcd();


};

#endif//_102BUFF_H_