#include "102buff.h"
#include <stdlib.h>
#include <string.h>

Time_str::Time_str()
{
    m_Year   = 0;
    m_Month  = 0;
    m_Day    = 0;
    m_Hour   = 0;
    m_Minute = 0;
    m_Second = 0;
    m_tv_usec = 0;

    struct timeval tv;
    struct tm localtm;
    memset ( &localtm, 0, sizeof ( localtm ) );
    memset ( &tv, 0, sizeof ( tv ) );

    gettimeofday ( &tv, NULL );
    m_tv_usec = tv.tv_usec;
    m_t = tv.tv_sec; //返回从纪元开始至今的秒数

    localtime_r ( &m_t, &localtm ); //获取本地时间

    m_Year   = localtm.tm_year + 1900;
    m_Month  = localtm.tm_mon + 1;
    m_Day    = localtm.tm_mday;
    m_Hour   = localtm.tm_hour;
    m_Minute = localtm.tm_min;
    m_Second = localtm.tm_sec;

}
Time_str::~Time_str()
{

}
void Time_str::getLocalTime ( const int& v_Minute )
{

    struct timeval tv;
    struct tm localtm;
    memset ( &localtm, 0, sizeof ( localtm ) );
    memset ( &tv, 0, sizeof ( tv ) );

    gettimeofday ( &tv, NULL );

    m_tv_usec = tv.tv_usec;
    m_t = tv.tv_sec;//返回从纪元开始至今的秒数
    m_t = m_t +  v_Minute * 60;

    localtime_r ( &m_t, &localtm ); //获取本地时间

    m_Year   = localtm.tm_year + 1900;
    m_Month  = localtm.tm_mon + 1;
    m_Day    = localtm.tm_mday;
    m_Hour   = localtm.tm_hour;
    m_Minute = localtm.tm_min;
    m_Second = localtm.tm_sec;

    return;
}

void Time_str::getLocalTime_day ( const int& v_day )
{
    struct timeval tv;
    struct tm localtm;
    memset ( &localtm, 0, sizeof ( localtm ) );
    memset ( &tv, 0, sizeof ( tv ) );

    gettimeofday ( &tv, NULL );

    m_tv_usec = tv.tv_usec;
    m_t = tv.tv_sec;//返回从纪元开始至今的秒数
    m_t = m_t + 60 * 60 * 24 * v_day;

    localtime_r ( &m_t, &localtm );

    int tmp = localtm.tm_hour * 60 * 60 + localtm.tm_min * 60 + localtm.tm_sec;
    m_t = m_t - tmp;

    return;
}

void Time_str::getLocalTime_day_hour_minute ( const int& v_Minute )
{
    struct tm localtm;
    memset ( &localtm, 0, sizeof ( localtm ) );



    m_t = m_t + v_Minute * 60;

    localtime_r ( &m_t, &localtm );

    m_Year   = localtm.tm_year + 1900;
    m_Month  = localtm.tm_mon + 1;
    m_Day    = localtm.tm_mday;
    m_Hour   = localtm.tm_hour;
    m_Minute = localtm.tm_min;
    m_Second = localtm.tm_sec;

    return;
}



Buff_102::Buff_102()
{
    m_Buffer = ( unsigned char* ) malloc ( MAXBUFFLEN );
    memset ( m_Buffer,
             0,
             sizeof ( unsigned char ) *MAXBUFFLEN
           );
    m_NextFrame = 0;
    m_Filename = "";
    m_BufferLen = 0;
    m_Stat = BUFF_STATE_EMPTY;
    m_DataType = 0;
    m_TryNumber = 0;
    m_yzcd = 0;
}

Buff_102::~Buff_102()
{
    free ( m_Buffer );
    m_NextFrame = 0;
    m_Filename = "";
    m_BufferLen = 0;
    m_Stat = BUFF_STATE_EMPTY;
    m_DataType = 0;
    m_TryNumber = 0;
    m_yzcd = 0;
}


int Buff_102::getBuffFun ( const char* content,
                           const string& file_name,
                           const int& len
                         )
{
    //char  tmp_err[200];
    if ( isUsing() == true )
    {
        return -1;
    }

    m_Mtx_Buff.Lock();

    int ret = 0;
    memset ( m_Buffer,
             0,
             sizeof ( unsigned char ) *MAXBUFFLEN
           );

    if ( len > MAXBUFFLEN )
    {
        memcpy ( ( char* ) m_Buffer, content, MAXBUFFLEN );
        m_yzcd = MAXBUFFLEN;            //获取文件的大小 单位字节
        m_NextFrame = 0;
        m_Filename = file_name;
        m_BufferLen = MAXBUFFLEN;       //文件内部字节数
        m_Stat = BUFF_STATE_USING;
        m_TryNumber = 0;

        ret = 1;
    }
    else
    {
        memcpy ( ( char* ) m_Buffer, content, len );
        m_yzcd = len;               //获取文件的大小 单位字节
        m_NextFrame = 0;
        m_Filename = file_name;
        m_BufferLen = len;          //文件内部字节数
        m_Stat = BUFF_STATE_USING;
        m_TryNumber = 0;

        ret = 0;
    }

    m_Mtx_Buff.unLock();

    return ret;

}

int Buff_102::clearBuffFun()
{
    m_Mtx_Buff.Lock();

    //memset(m_Buffer,0,sizeof(m_Buffer));
    memset ( m_Buffer, 0, sizeof ( unsigned char ) *MAXBUFFLEN );
    m_Stat = BUFF_STATE_EMPTY;
    m_BufferLen = 0;
    m_NextFrame = 0;
    m_Filename = "";
    m_DataType = 0;
    m_TryNumber = 0;

    m_Mtx_Buff.unLock();

    return 0;
}

int Buff_102::resetBuffFun()
{
    m_Mtx_Buff.Lock();
    m_NextFrame = 0;
    m_Mtx_Buff.unLock();
    return 0;
}
bool Buff_102::isUsing()
{
    m_Mtx_Buff.Lock();

    if ( m_Stat == BUFF_STATE_EMPTY )
    {
        m_Mtx_Buff.unLock();
        return false;
    }
    else
    {
        m_Mtx_Buff.unLock();
        return true;
    }
}

unsigned char* Buff_102::getBufferP()
{
    return m_Buffer;
}

void Buff_102::setNextFrame ( const int& next )
{
    m_Mtx_Buff.Lock();
    m_NextFrame = next;
    m_Mtx_Buff.unLock();

    return;
}

int Buff_102::getNextFrame()
{
    return m_NextFrame;
}

string Buff_102::getFileName()
{
    return m_Filename;
}

int Buff_102::getm_BufferLen()
{
    return m_BufferLen;
}

int Buff_102::getType()
{
    return m_DataType;
}

void Buff_102::setType ( const int& v_DataType )
{
    m_Mtx_Buff.Lock();
    m_DataType = v_DataType;
    m_Mtx_Buff.unLock();

    return;
}


void Buff_102::setTryNumber ( const int& tryNumber )
{
    m_Mtx_Buff.Lock();
    m_TryNumber = tryNumber;
    m_Mtx_Buff.unLock();

    return;
}

int& Buff_102::getTryNumber()
{
    return m_TryNumber;
}

void Buff_102::setyzcd ( const int& len )
{
    m_Mtx_Buff.Lock();
    m_yzcd = len;
    m_Mtx_Buff.unLock();

    return;
}

int Buff_102::getyzcd()
{
    return m_yzcd;
}



