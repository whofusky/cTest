﻿#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "slave.h"
#include "tinyxml2.h"
#include "EventManager/IEventMeta.h"

using namespace tinyxml2;

//SB_State_List g_stat_list[MC_PRLUP_MAX];


FileList::FileList()
{
    m_Type = 0;
}


FileList::~FileList()
{
    //printf("~FileList ---1111111\n");
}


void FileList::setType ( const int& v_Type )
{
    m_mtx_lister.Lock();
    m_Type = v_Type;
    m_mtx_lister.unLock();

    return;
}

int FileList::getType()
{
    return m_Type;
}

int FileList::getSize()
{
    int ret = 0;

    m_mtx_lister.Lock();
    ret = m_FileLister.size();
    m_mtx_lister.unLock();

    return ret;
}

string FileList::getAFileName()
{
    string tmp;

    if ( getSize() <= 0 )
    {
        //return NULL;
        return tmp;
    }

    m_mtx_lister.Lock();
    tmp = m_FileLister.back();
    m_FileLister.pop_back();
    m_mtx_lister.unLock();

    return tmp;
}

string FileList::backFileName()
{
    string tmp;

    if ( getSize() <= 0 )
    {
        //return NULL;
        return tmp;
    }

    m_mtx_lister.Lock();
    tmp = m_FileLister.back();
    m_mtx_lister.unLock();

    return tmp;
}

void FileList::popBackFileName()
{
    m_mtx_lister.Lock();
    m_FileLister.pop_back();
    m_mtx_lister.unLock();
    return;
}


void FileList::addAFileName2List ( const string& v_FileName )
{
    m_mtx_lister.Lock();

    m_FileLister.push_back ( v_FileName );

    m_mtx_lister.unLock();

    return;
}



CSBCG_State::CSBCG_State()
{
    isbzl = 0;
}

CSBCG_State::~CSBCG_State()
{

}



SB_State_List::SB_State_List()
{

}

SB_State_List::~SB_State_List()
{

}

CSBCG_State SB_State_List::getData()
{
    CSBCG_State tmp;
    m_Mtx_Stat.Lock();

    if ( m_stat_list.size() == 0 ) //返回容器中实际数据的个数。
    {
        m_Mtx_Stat.unLock();
        return * ( CSBCG_State* ) NULL;
    }

    tmp = m_stat_list.back();       //传回最后一个数据，不检查这个数据是否存在。
    m_stat_list.pop_back();         //删除最后一个暑假

    m_Mtx_Stat.unLock();

    return tmp;

}

int SB_State_List::getSize()
{
    return m_stat_list.size();
}

void SB_State_List::insert2list ( const CSBCG_State& v_ss )
{
    m_Mtx_Stat.Lock();
    m_stat_list.push_back ( v_ss ); //在尾部加入一个数据。
    m_Mtx_Stat.unLock();
    return;
}


Sock_Client::Sock_Client()
{
    sockC = -1;
    tid   = 0;
    memset ( ip, 0, sizeof ( ip ) );
}

Sock_Client::~Sock_Client()
{
    if ( sockC >= 0 )
    {
        close ( sockC );
        sockC = -1;
    }
}

void Sock_Client::close_Sock()
{
    m_mtx_sock.Lock();

    if ( sockC >= 0 )
    {
        close ( sockC );
    }

    sockC = -1;
    m_mtx_sock.unLock();

    return;
}

void Sock_Client::set_Sock ( const int& v_sock,
                             const char* v_ip,
                             const pthread_t& id
                           )
{
    m_mtx_sock.Lock();
    sockC = v_sock;
    strncpy ( ip, v_ip, sizeof ( ip ) - 1 );
    tid = id;
    m_mtx_sock.unLock();

    return;
}

bool Sock_Client::is_using()
{
    m_mtx_sock.Lock();

    if ( sockC < 0 )
    {
        m_mtx_sock.unLock();
        return false;
    }
    else
    {
        m_mtx_sock.unLock();
        return true;
    }

}



Slave::Slave()
{
    //  iCommCnt[0] = 0;
    //  iCommCnt[1] = 0;
    mHandler   = NULL;
    mLooper    = NULL;
    cMonFile_p = NULL;
    doingFlag  = 0;
    mPthNum    = 0;
    tidTas     = 0;
    tidTfl     = 0;


    memset ( iCommCnt, 0, sizeof ( iCommCnt ) );
    memset ( monFType, 0, sizeof ( monFType ) );
    memset ( noSockFlag, 0, sizeof ( noSockFlag ) );

    m_sock_timeout  = 0;
    m_sock_stimeout = 0;
    m_sock_rtimeout = 0;
    m_debug         = 0;
    m_filenamelen   = 0;
    m_maxtry        = 0;
    m_fss_houre_ks  = 0;
    m_fss_houre_js  = 0;
    m_port          = 0;

    usedIpNum      = 0;

    m_PFShowMsg    = NULL;

    //协议配置变量初始化 begine
    FixedBegin      = 0;
    VarbleBegin     = 0;
    FrameEnd        = 0;
    SX_KBXD_XDC     = 0;
    XX_YFFW         = 0;
    XX_QQLLZT       = 0;
    XX_QQ1SJ_1      = 0;
    XX_QQ1SJ_0      = 0;
    XX_QQ2SJ_1      = 0;
    XX_QQ2SJ_0      = 0;
    XX_YHSJ_0       = 0;
    XX_YHSJ_1       = 0;
    SX_KDRK         = 0;
    SX_FDRK         = 0;
    SX_LL           = 0;
    SX_WQQSJ_Y1SJ   = 0;
    SX_YHSJ         = 0;
    SX_WQQSJ        = 0;
    SX_W1SJ         = 0;
    SX_Y1SJ         = 0;
    XX_WJCSJS_TYPE  = 0;
    XX_WJCSCF_TYPE  = 0;
    XX_WJCDCC_TYPE  = 0;
    XX_WJMGSCW_TYPE = 0;
    XX_WJDZCC_TYPE  = 0;
    SX_WJCSJS_TYPE  = 0;
    SX_WJCSCF_TYPE  = 0;
    SX_WJCDCC_TYPE  = 0;
    SX_WJMGSCW_TYPE = 0;
    SX_WJDZCC_TYPE  = 0;
    XX_WJJS_CSYY    = 0;
    XX_WJCSCF_CSYY  = 0;
    XX_WJCDCC_CSYY  = 0;
    XX_WJMGSCW_CSYY = 0;
    XX_SDXFSJ_CSYY  = 0;
    SX_WJZH_CSYY    = 0;
    SX_WJFZH_CSYY   = 0;
    SX_WJCDZQ_CSYY  = 0;
    SX_WJCDCW_CSYY  = 0;
    SX_WJCSCF_CSYY  = 0;
    SX_WJCDCC_CSYY  = 0;
    SX_WJMGSCW_CSYY = 0;
    SX_SBDZ         = 0;
    SX_JLDZ         = 0;
    //协议配置变量初始化 end

}

Slave::~Slave()
{
    //释放文件list处理线程
    if ( tidTfl > 0 )
    {
        pthread_cancel ( tidTfl );
        pthread_join ( tidTfl, NULL );
    }

    //释放连接上的资源
    for ( int i = 0; i < usedIpNum; i++ )
    {
        if ( m_Sock[i].tid > 0 )
        {
            pthread_cancel ( m_Sock[i].tid );
            pthread_join ( m_Sock[i].tid, NULL );
        }

        if ( m_Sock[i].is_using() )
        {
            m_Sock[i].close_Sock();
        }
    }

    //释放sock server建立与链接线程
    if ( tidTas > 0 )
    {
        pthread_cancel ( tidTas );
        pthread_join ( tidTas, NULL );
    }

    //释放文件目录监视模块
    if ( cMonFile_p != NULL )
    {
        for ( int i = 0; i < mPthNum; i++ )
        {
            cMonFile_p->stopMonitor ( monPaths[i].c_str() );
        }

        delete cMonFile_p;
        cMonFile_p = NULL;

        //停事件管理线程
        IEventManager::Initialize()->stop();
    }

    //释放文件链表结构
    if ( m_FileList_Lister.size() > 0 )
    {
        for ( int i = 0; i < ( int ) m_FileList_Lister.size(); i++ )
        {
            delete m_FileList_Lister[i];
        }
    }

    //释放消息handler
    if ( mHandler != NULL )
    {
        delete mHandler;
        mHandler = NULL;
    }

    //释放消息looper
    if ( mLooper != NULL )
    {
        mLooper->decRef();
    }


}


int Slave::initeVar ( const string& strXmlPath )
{

    int vRet = 0;

    vRet = loadXmlval ( strXmlPath );

    if ( ( m_debug & 0x02 ) == 2 )
    {
        printf_xmlVal();
    }

    if ( mHandler == NULL )
    {
        mHandler = new FileListHandler ( this );
    }

    if ( mPthNum > 0 && cMonFile_p == NULL )
    {
        cMonFile_p = new FileMonitor();
    }

    return vRet;

}

void Slave::SetShowMessgaeFun ( PFShowMessage PFunShowMessage )
{
    m_PFShowMsg = PFunShowMessage;

    return;
}

void Slave::ShowMessage ( const string& strMsg )
{
    if ( m_PFShowMsg != NULL )
    {
        m_PFShowMsg ( strMsg );
    }

    return;
}

int Slave::selectFun ( const int&  v_Sock,
                       const int&  v_Sec,
                       const char& v_Mod
                     )
{
    struct timeval tv;
    tv.tv_sec = v_Sec ;
    tv.tv_usec = 0;

    fd_set fd;
    int maxid = 0;
    int ret = 0 ;

    FD_ZERO ( &fd );
    FD_SET ( v_Sock, &fd );

    if ( v_Sock >= maxid )
    {
        maxid = v_Sock + 1;
    }

    if ( v_Mod == 'r' )
    {
        ret = select ( maxid, &fd, NULL, NULL, &tv );
    }
    else if ( v_Mod == 'w' )
    {
        ret = select ( maxid, NULL, &fd, NULL, &tv );
    }

    return ret;

}

int Slave::recvBuffFun ( const int& v_Sock,
                         char* v_Buff,
                         const int& v_BuffLen
                       )
{
    int ret = 0;
    char tmpStr[100];
    memset ( tmpStr, 0, sizeof ( tmpStr ) );

    ret = selectFun ( v_Sock, m_sock_timeout, 'r' );

    if ( ret < 0 )
    {
        ShowMessage ( "网络资源不可读!" );
        return -1 ;
    }
    else if ( ret == 0 )
    {
        usleep ( 500 * 1000 );
        ShowMessage ( "读网络无数据的时间超过，"
                      "问答时间周期，当做超时处理，返回，"
                      "关闭sock并将sock设为0" );
        return -2;
    }
    else
    {
        recvMutex.Lock();
        ret = recv ( v_Sock,
                     v_Buff,
                     v_BuffLen,
                     0
                   );

        if ( ret <= 0 )
        {
            snprintf ( tmpStr,
                       sizeof ( tmpStr ),
                       "recv error[%s]",
                       strerror ( errno )
                     );
            ShowMessage ( tmpStr );
            recvMutex.unLock();
            return -1;
        }

        recvMutex.unLock();

        if ( ( m_debug & 0x01 ) == 1 )
        {
            write_debug_msg ( ( unsigned char* ) v_Buff,
                              v_BuffLen,
                              "recv data:"
                            );
        }
    }

    return ret;

}

int Slave::sendBuffFun ( const int& v_Sock,
                         const char* v_Buff,
                         const int& v_BuffLen
                       )
{
    int ret = 0;

    ret = selectFun ( v_Sock, m_sock_timeout, 'w' );

    if ( ret <= 0 )
    {
        ShowMessage ( "写网络数据的网络资源无响应，"
                      "返回，关闭sock并将sock设为0" );
        return -1;
    }
    else
    {
        recvMutex.Lock();
        ret = send ( v_Sock,
                     ( char* ) v_Buff,
                     v_BuffLen,
                     0
                   );

        if ( ret < 0 )
        {
            recvMutex.unLock();
            return -2;
        }

        recvMutex.unLock();

        if ( ( m_debug & 0x01 ) == 1 )
        {
            write_debug_msg ( ( unsigned char* ) v_Buff,
                              v_BuffLen,
                              "send data"
                            );
        }
    }

    return ret;

}

int Slave::efficacyData ( const unsigned char* v_Buff,
                          const int& v_BuffLen
                        )
{
    unsigned char tmp = 0;

    if ( v_Buff[0] == FixedBegin )
    {
        //固定帧效验
        for ( int i = 1; i <= 3; i++ )
        {
            tmp += v_Buff[i];
        }

        if ( tmp % 256 != v_Buff[4] )
        {
            ShowMessage ( "收到的固定长度帧数据,效验和错误!" );
            return -1;
        }
    }
    else if ( v_Buff[0] == VarbleBegin )
    {
        //变长帧效验
        short tmp_1 = v_Buff[1] + v_Buff[2] * 256;

        for ( int i = 4 ; i < ( tmp_1 + 4 ); i++ )
        {
            tmp += v_Buff[i];
        }

        if ( tmp % 256 != v_Buff[tmp_1 + 4] )
        {
            ShowMessage ( "收到的可变长度帧数据,效验和错误!" );
            return -1;
        }
    }
    else
    {
        ShowMessage ( "在效验时发现不明类型的数据!" );

        if ( ( m_debug & 0x01 ) == 1 )
        {
            write_debug_msg ( v_Buff,
                              v_BuffLen,
                              "在效验时发现不明类型的数据!"
                            );
        }

        return -1;
    }

    return 0;

}

int Slave::makeFixedFrameFun ( unsigned char* v_Buff,
                               unsigned char v_Mark,
                               unsigned char v_2,
                               unsigned char v_3
                             )
{
    int ret = 0;
    //memset(v_Buff,0,sizeof(v_Buff));

    v_Buff[0] = 0x10;
    v_Buff[1] = v_Mark;
    v_Buff[2] = v_2;
    v_Buff[3] = v_3;
    v_Buff[4] = 0x00;

    for ( int i = 1; i < 4; i ++ )
    {
        v_Buff[4] += v_Buff[i];
    }

    v_Buff[4] = v_Buff[4] % 256;
    v_Buff[5] = 0x16;

    ret = 6;

    return ret;

}

int Slave::assignment ( unsigned char* v_Buff,
                        const int& v_BuffLen,
                        bool& isend,
                        const int& idx
                      )
{
    int len     = 0;
    int int_tmp = 0;
    int leftlen = 0;
    char vRetMsg[128];

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    string tmp;

    int_tmp = m_Buff102[idx].getFileName().find_last_of ( "/" );
    len = m_Buff102[idx].getFileName().length() - int_tmp - 1;
    tmp = m_Buff102[idx].getFileName().substr ( int_tmp + 1, len );

    //先写文件名
    memset ( v_Buff, 0, v_BuffLen );
    strcpy ( ( char* ) v_Buff, tmp.c_str() );
    //memset(v_Buff + tmp.length() + 1,0,sizeof(v_BuffLen - tmp.length() + 1));

    if ( m_filenamelen < ( int ) tmp.length() )
    {
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "第%d路数据buffer处理:配置的协议文件名长度小于使用的文件名长度",
                   idx + 1 );
        ShowMessage ( vRetMsg );
        return -1;
    }

    //写文件内容
    leftlen = v_BuffLen;
    leftlen = leftlen - m_filenamelen - 2;

    len = m_Buff102[idx].getm_BufferLen() - m_Buff102[idx].getNextFrame();

    if ( leftlen >= len )
    {
        memcpy ( v_Buff + m_filenamelen,
                 & ( m_Buff102[idx].getBufferP() [m_Buff102[idx].getNextFrame()] ),
                 len
               );
        //m_Buff102[idx].m_NextFrame = m_Buff102[idx].m_BufferLen;
        m_Buff102[idx].setNextFrame ( m_Buff102[idx].getm_BufferLen() );
        isend = true;
        return ( len + m_filenamelen );
    }
    else
    {
        memcpy ( v_Buff + m_filenamelen,
                 & ( m_Buff102[idx].getBufferP() [m_Buff102[idx].getNextFrame()] ),
                 leftlen
               );
        //m_Buff102[idx].m_NextFrame = m_Buff102[idx].m_NextFrame + leftlen;
        m_Buff102[idx].setNextFrame ( m_Buff102[idx].getNextFrame() + leftlen );

        isend = false;
        return ( leftlen + m_filenamelen );
    }

}

int Slave::makeVaribleFrameFun ( unsigned char* v_Buff,
                                 const int& v_BuffLen,
                                 const unsigned char* v_RecvBuff,
                                 const int& v_RecvBuffLen,
                                 const int& v_PackType,
                                 const int& idx
                               )
{
    int ret = 0;
    int len = 0;

    //memset(v_Buff,0,sizeof(v_Buff));
    memset ( v_Buff, 0, v_BuffLen );

    bool isdata = false;

    for ( int i = 0; i < ( int ) m_type_list.size(); i++ )
    {
        if ( v_PackType == m_type_list[i] )
        {
            isdata = true;
            break;
        }
    }

    if ( isdata )
    {
        bool isend;
        isend = false;

        ret = assignment ( v_Buff + 13,
                           v_BuffLen - 13,
                           isend,
                           idx
                         );
        v_Buff[0] = VarbleBegin;
        //此处严格需要判断大小端的问题 102协议是小端 v_Buff[1] v_Buff[2]存在帧长度
        * ( short* ) &v_Buff[1] = 9 + ret;
        v_Buff[3] = VarbleBegin;

        if ( isend ) //m_Buff102中未发送的数据能有全部放到v_Buff
        {
            v_Buff[4] = SX_W1SJ;
        }
        else
        {
            v_Buff[4] = SX_Y1SJ;
        }

        v_Buff[5] = v_RecvBuff[2];
        v_Buff[6] = v_RecvBuff[3];
        v_Buff[7] = v_PackType;
        v_Buff[8] = SX_KBXD_XDC;

        if ( isend == true )
        {
            v_Buff[9] = SX_WJZH_CSYY;
        }
        else
        {
            v_Buff[9] = SX_WJFZH_CSYY;
        }

        v_Buff[10] = v_RecvBuff[2];
        v_Buff[11] = v_RecvBuff[3];
        v_Buff[12] = 0x00;
        v_Buff[13 + ret] = 0x00;

        for ( int i = 4 ; i < int ( 13 + ret ); i++ )
        {
            v_Buff[13 + ret] += v_Buff[i];
        }

        v_Buff[13 + ret] = v_Buff[13 + ret] % 256;
        v_Buff[13 + ret + 1] = 0x16;
        len = ret + 15;

        return len;

    }
    else if ( v_PackType == SX_WJCSJS_TYPE ) //子站确认传输结束
    {
        //应答数据做文件传输的全文件效验
        v_Buff[0] = VarbleBegin;
        v_Buff[1] = 0x0d;      //此种情况固定字节
        v_Buff[2] = 0x00;
        v_Buff[3] = VarbleBegin;
        v_Buff[4] = SX_W1SJ;
        v_Buff[5] = v_RecvBuff[5];
        v_Buff[6] = v_RecvBuff[6];
        v_Buff[7] = SX_WJCSJS_TYPE;
        v_Buff[8] = 0x01;//一个信息对象
        //检查上次发送的文件的长度 和 返回确认帧中地文件长度是否相等

        if ( m_Buff102[idx].getm_BufferLen() == * ( int* ) &v_RecvBuff[13] )
            //      if (m_Buff102.m_yzcd == *(int *)&v_RecvBuff[13])
        {
            //当相等时，清理发送字符流，将字符串标识设置为空可用
            v_Buff[9] = SX_WJCDZQ_CSYY;
            * ( int* ) &v_Buff[13] = m_Buff102[idx].getm_BufferLen();
            char tmp_err[100];
            memset ( tmp_err, 0, sizeof ( tmp_err ) );
            snprintf ( tmp_err,
                       sizeof ( tmp_err ),
                       "第%d路 接收到的文件验证长度[%d]",
                       idx + 1, * ( int* ) &v_RecvBuff[13]
                     );
            ShowMessage ( tmp_err );
        }
        else
        {
            //当不相等时
            v_Buff[9] = SX_WJCDCW_CSYY;
            * ( int* ) &v_Buff[13] = m_Buff102[idx].getm_BufferLen();
            char tmp_err[100];
            memset ( tmp_err, 0, sizeof ( tmp_err ) );
            snprintf ( tmp_err,
                       sizeof ( tmp_err ),
                       "第%d路 接收到的文件验证长度[%d]",
                       idx + 1, * ( int* ) &v_RecvBuff[13]
                     );
            ShowMessage ( tmp_err );
        }

        v_Buff[10] = v_RecvBuff[5];
        v_Buff[11] = v_RecvBuff[6];
        v_Buff[12] = 0x00;//记录地址
        v_Buff[17] = 0x00;

        for ( int i = 4; i < 16; i++ )
        {
            v_Buff[17] += v_Buff[i];
        }

        v_Buff[17] = v_Buff[17] % 256;
        v_Buff[18] = FrameEnd;
        len = 19;

    }
    else if ( v_PackType == SX_WJCSCF_TYPE ) //子站确认传输重复
    {
        v_Buff[0]  = VarbleBegin;//FixedBegin;
        v_Buff[1]  = 0x09;      //此种情况固定字节
        v_Buff[2]  = 0x00;
        v_Buff[3]  = VarbleBegin;
        v_Buff[4]  = SX_W1SJ;
        v_Buff[5]  = v_RecvBuff[5];
        v_Buff[6]  = v_RecvBuff[6];
        v_Buff[7]  = SX_WJCSCF_TYPE;
        v_Buff[8]  = 0x01;   //一个信息对象
        v_Buff[9]  = SX_WJCSCF_CSYY;
        v_Buff[10] = v_RecvBuff[5];
        v_Buff[11] = v_RecvBuff[6];
        v_Buff[12] = 0x00;//记录地址
        v_Buff[13] = 0x00;

        for ( int i = 4; i < 13; i++ )
        {
            v_Buff[13] += v_Buff[i];
        }

        v_Buff[13] = v_Buff[13] % 256;
        v_Buff[14] = FrameEnd;
        len = 15;

    }
    else if ( v_PackType == SX_WJCDCC_TYPE ) //子站确认文件超度超长
    {
        v_Buff[0]  = VarbleBegin;
        v_Buff[1]  = 0x09;      //此种情况固定字节
        v_Buff[2]  = 0x00;
        v_Buff[3]  = VarbleBegin;
        v_Buff[4]  = SX_W1SJ;
        v_Buff[5]  = v_RecvBuff[5];
        v_Buff[6]  = v_RecvBuff[6];
        v_Buff[7]  = SX_WJCDCC_TYPE;
        v_Buff[8]  = 0x01;//一个信息对象
        v_Buff[9]  = SX_WJCDCC_CSYY;
        v_Buff[10] = v_RecvBuff[5];
        v_Buff[11] = v_RecvBuff[6];
        v_Buff[12] = 0x00;//记录地址
        v_Buff[13] = 0x00;

        for ( int i = 4; i < 13; i++ )
        {
            v_Buff[13] += v_Buff[i];
        }

        v_Buff[13] = v_Buff[13] % 256;
        v_Buff[14] = FrameEnd;
        len = 15;

    }
    else if ( v_PackType == SX_WJMGSCW_TYPE ) //子站确认文件名格式错误
    {
        v_Buff[0]  = VarbleBegin;
        v_Buff[1]  = 0x09;      //此种情况固定字节
        v_Buff[2]  = 0x00;
        v_Buff[3]  = VarbleBegin;
        v_Buff[4]  = SX_W1SJ;
        v_Buff[5]  = v_RecvBuff[5];
        v_Buff[6]  = v_RecvBuff[6];
        v_Buff[7]  = SX_WJMGSCW_TYPE;
        v_Buff[8]  = 0x01;   //一个信息对象
        v_Buff[9]  = SX_WJMGSCW_CSYY;
        v_Buff[10] = v_RecvBuff[5];
        v_Buff[11] = v_RecvBuff[6];
        v_Buff[12] = 0x00;  //记录地址
        v_Buff[13] = 0x00;

        for ( int i = 4; i < 13; i++ )
        {
            v_Buff[13] += v_Buff[i];
        }

        v_Buff[13] = v_Buff[13] % 256;
        v_Buff[14] = FrameEnd;
        len = 15;

    }
    else if ( v_PackType == SX_WJDZCC_TYPE ) //子站确认文件单帧过长
    {
        v_Buff[0]  = VarbleBegin;
        v_Buff[1]  = 0x09;      //此种情况固定字节
        v_Buff[2]  = 0x00;
        v_Buff[3]  = VarbleBegin;
        v_Buff[4]  = SX_W1SJ;
        v_Buff[5]  = v_RecvBuff[5];
        v_Buff[6]  = v_RecvBuff[6];
        v_Buff[7]  = SX_WJDZCC_TYPE;
        v_Buff[8]  = 0x01;   //一个信息对象
        v_Buff[9]  = 0x14;
        v_Buff[10] = v_RecvBuff[5];
        v_Buff[11] = v_RecvBuff[6];
        v_Buff[12] = 0x00;  //记录地址
        v_Buff[13] = 0x00;

        for ( int i = 4; i < 13; i++ )
        {
            v_Buff[13] += v_Buff[i];
        }

        v_Buff[13] = v_Buff[13] % 256;
        v_Buff[14] = FrameEnd;
        len = 15;

    }

    return len;

}


int Slave::dealBuff ( const int& v_Sock,
                      unsigned char* v_Buff,
                      const int& v_BuffLen,
                      const int& idx
                    )
{
    int ret = 0;
    int sendLen = 0;

    char vRetMsg[256];
    unsigned char fixedBuff[6];
    unsigned char variblBuff[MAX_PACK_LEN];

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( fixedBuff, 0, sizeof ( fixedBuff ) );
    memset ( variblBuff, 0, sizeof ( variblBuff ) );


    if ( v_Buff[0] == FixedBegin ) //定长帧
    {
        //效验和
        if ( efficacyData ( v_Buff, v_BuffLen ) == -1 )
        {
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路,定长帧校验和错误", idx + 1 );
            ShowMessage ( vRetMsg );
            return -1;
        }

        //判断固定帧的类型
        if ( v_Buff[1] == XX_YFFW ) //复位帧
        {
            if ( ( m_debug & 0x02 ) == 2 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,复位帧",
                           idx + 1 );
                ShowMessage ( vRetMsg );
            }

            m_Buff102[idx].resetBuffFun();
            sendLen = makeFixedFrameFun ( fixedBuff,
                                          SX_KDRK,
                                          v_Buff[2],
                                          v_Buff[3]
                                        );

            ret = sendBuffFun ( v_Sock,
                                ( char* ) fixedBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,在网络上发送数据失败!",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }
        }
        else if ( v_Buff[1] == XX_QQLLZT ) //连接状态帧
        {
            if ( ( m_debug & 0x02 ) == 2 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,连接状态帧!",
                           idx + 1 );
                ShowMessage ( vRetMsg );
            }

            sendLen = makeFixedFrameFun ( fixedBuff,
                                          SX_LL,
                                          v_Buff[2],
                                          v_Buff[3]
                                        );

            ret = sendBuffFun ( v_Sock,
                                ( char* ) fixedBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,请求链路状态回复帧在网络上发送数据失败!",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }
        }
        else if ( v_Buff[1] == XX_QQ1SJ_1
                  || v_Buff[1] == XX_QQ1SJ_0
                ) //1级用书数据帧
        {
            //ShowMessage("1级用书数据帧");
            //判断当前是否有可发送的数据
            if ( m_Buff102[idx].isUsing() == false )
            {
                //ShowMessage("---当前否有可发送的数据");
                sendLen = makeFixedFrameFun ( fixedBuff,
                                              SX_WQQSJ_Y1SJ,
                                              v_Buff[2],
                                              v_Buff[3]
                                            );
                ret = sendBuffFun ( v_Sock,
                                    ( char* ) fixedBuff,
                                    sendLen
                                  );

                if ( ret <= 0 )
                {
                    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                    snprintf ( vRetMsg, sizeof ( vRetMsg ),
                               "第%d路,请求1级用户数据回复无发送数据帧在网络上发送数据失败!",
                               idx + 1 );
                    ShowMessage ( vRetMsg );
                    return -1;
                }
            }
            else
            {
                //ShowMessage("+++当前有可发送的数据");
                sendLen = makeVaribleFrameFun ( &variblBuff[0],
                                                sizeof ( variblBuff ),
                                                v_Buff,
                                                v_BuffLen,
                                                m_Buff102[idx].getType(),
                                                idx
                                              );
                ret = sendBuffFun ( v_Sock,
                                    ( char* ) variblBuff,
                                    sendLen
                                  );

                if ( ret <= 0 )
                {
                    //发送数据失败尝试次数加1 查看尝试发送次数 如果
                    //达到尝试次数次数超过预定值 清空m_Buff102
                    m_Buff102[idx].getTryNumber() ++;

                    if ( m_Buff102[idx].getTryNumber() >= m_maxtry )
                    {
                        Time_str datetime;

                        if ( datetime.m_Hour >= m_fss_houre_ks
                             && datetime.m_Hour < m_fss_houre_js
                           )
                        {
                            add_Old_File ( m_Buff102[idx].getFileName(),
                                           m_Buff102[idx].getType()
                                         );
                        }

                        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                                   "第%d路,文件[%s]上传尝试了[%d]次还是失败,删除!",
                                   idx + 1, m_Buff102[idx].getFileName().c_str(), m_maxtry );
                        ShowMessage ( vRetMsg );

                        if ( upRetList == 1 )
                        {
                            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                                       "失败,第%d路上报多次还是失败", idx + 1 );
                            CSBCG_State sbcg;
                            sbcg.isbzl = m_Buff102[idx].getType();
                            sbcg.strsbzt = "失败";
                            sbcg.strsbbz = "上报多次还是失败!";
                            sbcg.strFaileCause = vRetMsg;
                            up_stat_list[idx].insert2list ( sbcg );
                        }

                        m_Buff102[idx].clearBuffFun();

                    }
                    else
                    {
                        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                                   "第%d路,请求1级用户数据回复有发送数据帧在网络上发送数据失败!",
                                   idx + 1 );
                        ShowMessage ( vRetMsg );
                    }

                    return -1;

                }
            }
        }
        else if ( v_Buff[1] == XX_QQ2SJ_1 || v_Buff[1] == XX_QQ2SJ_0 ) //2级用户数据帧
        {
            if ( ( m_debug & 0x02 ) == 2 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,2级用户数据帧",
                           idx + 1 );
                ShowMessage ( vRetMsg );
            }

            if ( m_Buff102[idx].isUsing() == true )
            {
                if ( ( m_debug & 0x02 ) == 2 )
                {
                    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                    snprintf ( vRetMsg, sizeof ( vRetMsg ),
                               "第%d路,2级用户数据帧当前有可发送的数据",
                               idx + 1 );
                    ShowMessage ( vRetMsg );
                }

                sendLen = makeFixedFrameFun ( fixedBuff,
                                              SX_WQQSJ_Y1SJ,
                                              v_Buff[2],
                                              v_Buff[3]
                                            );
            }
            else
            {
                if ( ( m_debug & 0x02 ) == 2 )
                {
                    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                    snprintf ( vRetMsg, sizeof ( vRetMsg ),
                               "第%d路,2级用户数据帧当前没有可发送的数据",
                               idx + 1 );
                    ShowMessage ( vRetMsg );
                }

                sendLen = makeFixedFrameFun ( fixedBuff,
                                              SX_WQQSJ,
                                              v_Buff[2],
                                              v_Buff[3]
                                            );
            }

            ret = sendBuffFun ( v_Sock,
                                ( char* ) fixedBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,2级用户数据回复帧在网络上发送数据失败!",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }
        }
        else
        {
            //printfbyte(v_Buff, v_BuffLen, "定长帧类型错误");
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路,接收到的固定长度帧的控制域超过了已知的类型,判断失败!",
                       idx + 1 );
            ShowMessage ( vRetMsg );
            return -1;

        }
    }
    else if ( v_Buff[0] == VarbleBegin ) //变长帧
    {
        if ( ( m_debug & 0x02 ) == 2 )
        {
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路,变长帧",
                       idx + 1 );
            ShowMessage ( vRetMsg );
        }

        //效验和
        if ( efficacyData ( v_Buff, v_BuffLen ) == -1 )
        {
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路,变长帧校验和错误",
                       idx + 1 );
            ShowMessage ( vRetMsg );
            return -1;
        }

        //判断变长帧的类型
        if ( v_Buff[9] == 0x0a
             && v_Buff[7] == XX_WJCSJS_TYPE
           ) //主站表示文件传输结束
        {
            //unsigned char sendbuff[19];

            sendLen = makeVaribleFrameFun ( variblBuff,
                                            sizeof ( variblBuff ),
                                            v_Buff,
                                            v_BuffLen,
                                            SX_WJCSJS_TYPE,
                                            idx
                                          );

            if ( variblBuff[9] == SX_WJCDZQ_CSYY )
            {
                //正常
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路，文件[%s]的长度[%d]确认正常,传输结束!",
                           idx + 1,
                           m_Buff102[idx].getFileName().c_str(),
                           m_Buff102[idx].getm_BufferLen() );
                ShowMessage ( vRetMsg );

                if ( upRetList == 1 )
                {
                    CSBCG_State sbcg;
                    sbcg.isbzl = m_Buff102[idx].getType();
                    sbcg.strsbzt = "成功";
                    sbcg.strFaileCause = "成功";
                    sbcg.strsbxzwjm = m_Buff102[idx].getFileName().c_str();
                    up_stat_list[idx].insert2list ( sbcg );
                }

                m_datetime_stat.getLocalTime ( 0 );
                m_Buff102[idx].clearBuffFun();

            }
            else if ( variblBuff[9] == SX_WJCDCW_CSYY )
            {
                //异常
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路 文件[%s]的长度异常[%d],文件已无意义,清理缓冲区!",
                           idx + 1,
                           m_Buff102[idx].getFileName().c_str(),
                           m_Buff102[idx].clearBuffFun()
                         );
                ShowMessage ( vRetMsg );

                if ( upRetList == 1 )
                {
                    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                    snprintf ( vRetMsg, sizeof ( vRetMsg ),
                               "失败,第%d路长度确认异常", idx + 1 );
                    CSBCG_State sbcg;
                    sbcg.isbzl         = m_Buff102[idx].getType();
                    sbcg.strsbzt       = "失败";
                    sbcg.strsbxzwjm    = m_Buff102[idx].getFileName().c_str();
                    sbcg.strsbbz       = "长度确认异常";
                    sbcg.strFaileCause = vRetMsg;
                    up_stat_list[idx].insert2list ( sbcg );
                }

                m_Buff102[idx].clearBuffFun();

            }
            else
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,主站表示文件传输结束子站装帧错误",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }

            ret = sendBuffFun ( v_Sock,
                                ( char* ) variblBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "第%d路,主站表示文件传输结束子站在网络上发送数据失败",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }

        }
        else if ( v_Buff[9] == 0x0d
                  && v_Buff[7] == XX_WJCSCF_TYPE
                ) //主站表示文件传输重复
        {
            //unsigned char sendbuff[15];
            sendLen = makeVaribleFrameFun ( variblBuff,
                                            sizeof ( variblBuff ),
                                            v_Buff, v_BuffLen,
                                            SX_WJCSCF_TYPE,
                                            idx
                                          );

            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路 主站表示文件[%s]重复传输,次文件已无意义,清理缓冲区!",
                       idx + 1, m_Buff102[idx].getFileName().c_str() );
            ShowMessage ( vRetMsg );

            if ( upRetList == 1 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路文件重复传输", idx + 1 );
                CSBCG_State sbcg;
                sbcg.isbzl         = m_Buff102[idx].getType();
                sbcg.strsbzt       = "失败";
                sbcg.strsbxzwjm    = m_Buff102[idx].getFileName().c_str();
                sbcg.strsbbz       = "主站回复文件传输重复";
                sbcg.strFaileCause = vRetMsg;
                up_stat_list[idx].insert2list ( sbcg );
            }

            m_Buff102[idx].clearBuffFun();

            ret = sendBuffFun ( v_Sock,
                                ( char* ) variblBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路主站表示文件传输重复子站在网络上发送数据失败",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }
        }
        else if ( v_Buff[9] == 0x0f
                  && v_Buff[7] == XX_WJCDCC_TYPE
                ) //主站表示文件长度超长
        {
            //unsigned char sendbuff[15];

            sendLen = makeVaribleFrameFun ( variblBuff,
                                            sizeof ( variblBuff ),
                                            v_Buff,
                                            v_BuffLen,
                                            SX_WJCDCC_TYPE,
                                            idx
                                          );

            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路 主站表示[%s]长度超长,长度为[%d],此文件无意义,清理缓冲区!",
                       idx + 1,
                       m_Buff102[idx].getFileName().c_str()
                       , m_Buff102[idx].getm_BufferLen()
                     );
            ShowMessage ( vRetMsg );

            if ( upRetList == 1 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路文件长度超长", idx + 1 );
                CSBCG_State sbcg;
                sbcg.isbzl         = m_Buff102[idx].getType();
                sbcg.strsbzt       = "失败";
                sbcg.strsbxzwjm    = m_Buff102[idx].getFileName().c_str();
                sbcg.strsbbz       = "文件长度超长";
                sbcg.strFaileCause = vRetMsg;
                up_stat_list[idx].insert2list ( sbcg );
            }

            m_Buff102[idx].clearBuffFun();

            ret = sendBuffFun ( v_Sock,
                                ( char* ) variblBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路主站表示文件长度越长子站在网络上发送数据失败",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }

        }
        else if ( v_Buff[9] == 0x11
                  && v_Buff[7] == XX_WJMGSCW_TYPE
                ) //主站表示文件名格式错误
        {
            //unsigned char sendbuff[15];
            sendLen = makeVaribleFrameFun ( variblBuff,
                                            sizeof ( variblBuff ),
                                            v_Buff, v_BuffLen,
                                            SX_WJMGSCW_TYPE,
                                            idx
                                          );
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路 主站表示[%s]文件名格式错误,此文件已无意义,清理缓冲区!",
                       idx + 1, m_Buff102[idx].getFileName().c_str() );
            ShowMessage ( vRetMsg );

            Time_str datetime;

            if ( ( datetime.m_Hour == m_fss_houre_ks ) || ( datetime.m_Hour == m_fss_houre_js ) )
            {
                add_Old_File ( m_Buff102[idx].getFileName(), m_Buff102[idx].getType() );
                sleep ( 1 );
            }

            if ( upRetList == 1 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路文件格式名错误", idx + 1 );
                CSBCG_State sbcg;
                sbcg.isbzl         = m_Buff102[idx].getType();
                sbcg.strsbzt       = "失败";
                sbcg.strsbxzwjm    = m_Buff102[idx].getFileName().c_str();
                sbcg.strsbbz       = "文件名格式错误";
                sbcg.strFaileCause = vRetMsg;
                up_stat_list[idx].insert2list ( sbcg );
            }

            m_Buff102[idx].clearBuffFun();
            ret = sendBuffFun ( v_Sock,
                                ( char* ) variblBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路主站表示文件名格式错误子站在网络上发送数据失败",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }

        }
        else if ( v_Buff[9] == 0x13 && v_Buff[7] == XX_WJDZCC_TYPE ) //主站表示单帧过长
        {
            //unsigned char sendbuff[15];
            sendLen = makeVaribleFrameFun ( variblBuff,
                                            sizeof ( variblBuff ),
                                            v_Buff,
                                            v_BuffLen,
                                            XX_WJDZCC_TYPE,
                                            idx
                                          );
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路 主站表示[%s]文件单帧过长,此文件已无意义,清理缓冲区!",
                       idx + 1, m_Buff102[idx].getFileName().c_str() );
            ShowMessage ( vRetMsg );

            Time_str datetime;

            if ( ( datetime.m_Hour == m_fss_houre_ks )
                 || ( datetime.m_Hour == m_fss_houre_js )
               )
            {
                add_Old_File ( m_Buff102[idx].getFileName(),
                               m_Buff102[idx].getType()
                             );
                sleep ( 1 );
            }

            if ( upRetList == 1 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路文件单帧过长", idx + 1 );
                CSBCG_State sbcg;
                sbcg.isbzl         = m_Buff102[idx].getType();
                sbcg.strsbzt       = "失败";
                sbcg.strsbxzwjm    = m_Buff102[idx].getFileName().c_str();
                sbcg.strsbbz       = "文件单帧过长";
                sbcg.strFaileCause = vRetMsg;
                up_stat_list[idx].insert2list ( sbcg );
            }

            m_Buff102[idx].clearBuffFun();
            ret = sendBuffFun ( v_Sock,
                                ( char* ) variblBuff,
                                sendLen
                              );

            if ( ret <= 0 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg, sizeof ( vRetMsg ),
                           "失败,第%d路主站表示文件单帧过长子站在网络上发送数据失败",
                           idx + 1 );
                ShowMessage ( vRetMsg );
                return -1;
            }

        }
        else
        {
            //printfbyte(v_Buff, v_BuffLen, "定长帧类型错误");
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "第%d路接收到的变长长度帧的控制域和应用报文"
                       "类型超过了已知的类型,判断失败,本次连接已经没有意义,"
                       "结束此次连接!",
                       idx + 1 );
            ShowMessage ( vRetMsg );
            return -1;

        }

    }
    else
    {
        //printfbyte(v_Buff, v_BuffLen, "未知帧类型");
        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "第%d路接收到了不明格式（不是定长帧也不是"
                   "变长帧）的数据!", idx + 1 );
        ShowMessage ( vRetMsg );

        if ( ( m_debug & 0x01 ) == 1 )
        {
            write_debug_msg ( v_Buff,
                              v_BuffLen,
                              vRetMsg
                            );
        }

        return -1;

    }

    return 0;

}

int Slave::inIteSlave()
{
    char vErrMsg[256];
    int  vRet = 0;
    //    pthread_t tidTas = 0;
    //    pthread_t tidTfl = 0;
    memset ( vErrMsg, 0, sizeof ( vErrMsg ) );

    vRet = pthread_create ( &tidTas,
                            NULL,
                            ThreadAcceptSock,
                            ( void* ) this
                          );

    if ( 0 != vRet )
    {
        snprintf ( vErrMsg,
                   sizeof ( vErrMsg ),
                   "pthread_create ThreadAcceptSock error[%s]\n",
                   strerror ( vRet )
                 );
        ShowMessage ( vErrMsg );
        return vRet;
    }

    //添加文件列表消息处理线程
    vRet = pthread_create ( &tidTfl,
                            NULL,
                            ThreadFileList,
                            ( void* ) this
                          );

    if ( 0 != vRet )
    {
        snprintf ( vErrMsg,
                   sizeof ( vErrMsg ),
                   "pthread_create ThreadFileList error[%s]\n",
                   strerror ( vRet )
                 );
        ShowMessage ( vErrMsg );
    }

    if ( mPthNum > 0 )
    {
        //事件管理线程
        IEventManager::Initialize()->start();

        //文件监视处理线程
        startMon();

    }

    return vRet;

}

void Slave::add_File_List (  FileList* v_FileList )
{
    //const FileList tmpFList= v_FileList;
    flstMutex.Lock();
    m_FileList_Lister.push_back ( v_FileList );
    flstMutex.unLock();

    return;
}

int Slave::chkFType ( const int& iType )
{
    if ( iType == XX_WJCSJS_TYPE
         || iType == XX_WJCSCF_TYPE
         || iType == XX_WJCDCC_TYPE
         || iType == XX_WJMGSCW_TYPE
         || iType == XX_WJDZCC_TYPE
         || iType == SX_WJCSJS_TYPE
         || iType == SX_WJCSCF_TYPE
         || iType == SX_WJCDCC_TYPE
         || iType == SX_WJMGSCW_TYPE
         || iType == SX_WJDZCC_TYPE
       )
    {
        return -1;
    }

    return 0;
}


int Slave::getFLLsize()
{
    CMutexGuard tGuard ( flstMutex );
    return m_FileList_Lister.size();
}

int Slave::getFTLsize()
{
    CMutexGuard tGuard ( ftypeMutex );
    return m_type_list.size();
}


FileList* Slave::getFLLback()
{
    CMutexGuard tGuard ( flstMutex );
    FileList* tmp_p = m_FileList_Lister.back();

    return tmp_p;
}

void  Slave::pop_backFll()
{
    CMutexGuard tGuard ( flstMutex );
    m_FileList_Lister.pop_back();
    return;
}

int Slave::add_Old_File ( const string& v_FileName,
                          const int& v_Type
                        )
{
    int vRet = 0;
    char vRetMsg[200];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    vRet = chkFType ( v_Type );

    if ( vRet != 0 )
    {
        snprintf ( vRetMsg,
                   sizeof ( vRetMsg ),
                   "文件类型[%d]与已有的typeIdentification应用报文类型冲突",
                   v_Type );
        ShowMessage ( vRetMsg );
        return -1;
    }

    vRet = access ( v_FileName.c_str(), F_OK ); //判断文件是否存在

    if ( vRet != 0 )
    {
        snprintf ( vRetMsg,
                   sizeof ( vRetMsg ),
                   "文件[%s]不存在!error[%s]",
                   v_FileName.c_str(), strerror ( errno ) );
        ShowMessage ( vRetMsg );
        return -1;
    }


    int findFlag = 0;

    //flstMutex.Lock();
    for ( int i = 0; i < ( int ) m_FileList_Lister.size(); i++ )
    {
        if ( m_FileList_Lister[i]->getType() == v_Type )
        {
            m_FileList_Lister[i]->addAFileName2List ( v_FileName );
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg,
                       sizeof ( vRetMsg ),
                       "将文件[%s]类型为[%d]的文件放回到类型为[%d]的文件队列中!",
                       v_FileName.c_str(), v_Type, m_FileList_Lister[i]->getType() );
            ShowMessage ( vRetMsg );

            findFlag = 1;
            break;
        }
    }

    // flstMutex.unLock();

    if ( findFlag == 0 )
    {
        FileList* tmpFList_p = NULL;
        tmpFList_p = new FileList();
        tmpFList_p->setType ( v_Type );
        tmpFList_p->addAFileName2List ( v_FileName );
        add_File_List ( tmpFList_p );
        addType ( v_Type );

    }

    if ( doingFlag == 0 ) //没有进程在处理文件列表，需要发消息通知处理
    {
        Message msg;
        msg.mWhat    = FileListHandler::PARSE_ADD_FILE;
        //msg.mAttr    = attr;//此值暂时不用
        msg.mTarget  = mHandler;
        msg.mTargetLooper = mLooper;
        msg.setValid ( True );

        mHandler->sendMessage ( msg );
    }


    return 0;

}

void Slave::addType ( int v_type )
{
    bool isfind = false;

    for ( int i = 0; i < getFTLsize(); i++ )
    {
        if ( v_type == m_type_list[i] )
        {
            isfind = true;
            break;
        }
    }

    if ( false == isfind )
    {
        ftypeMutex.Lock();
        m_type_list.push_back ( v_type );
        ftypeMutex.unLock();
    }

    return;
}

int Slave::getSockTimeout()
{
    return m_sock_timeout;
}
int Slave::getSockSTimeout()
{
    return m_sock_stimeout;
}
int Slave::getSockRTimeout()
{
    return m_sock_rtimeout;
}


int Slave::getDebug()
{
    return m_debug;
}
int Slave::getFileNameLen()
{
    return m_filenamelen;
}
int Slave::getMaxTry()
{
    return m_maxtry;
}
int Slave::getFssHBegine()
{
    return m_fss_houre_ks;
}
int Slave::getFssHEnd()
{
    return m_fss_houre_js;
}


int Slave::getPort()
{
    return m_port;
}


void Slave::getIps ( string* ips_p )
{
    //ipNum = usedIpNum;
    for ( int i = 0; i < usedIpNum && usedIpNum <= MC_PRLUP_MAX; i++ )
    {
        ips_p[i] = m_ips[i];
    }

    return;
}

int Slave::getIpNum()
{
    return usedIpNum;
}

void Slave::write_debug_msg ( const unsigned char* prtBuffer_p,
                              const int& buffLen,
                              const char* errNote,
                              const int& prtType
                            )
{
    int vRet = 0;
    string byteS;
    FILE* flogfile = NULL;

    m_datetime_stat.getLocalTime();

    char strPathLog[128];
    char tmpStr[1024];
    char curDir[100];

    memset ( strPathLog, 0, sizeof ( strPathLog ) );
    memset ( tmpStr, 0, sizeof ( tmpStr ) );
    memset ( curDir, 0, sizeof ( curDir ) );

    if ( prtType == 0 ) //打印报文 及 errNote
    {
        if ( prtBuffer_p[0] == FixedBegin ) //固定帧
        {
            byteS.append ( errNote );
            byteS.append ( "|" );
            byteS.append ( "FixedBegin:" );
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "buffer[0]=0x%x [1]=0x%x "
                       "[2]=0x%x [3]=0x%x [4]=0x%x [5]=0x%x \n",
                       prtBuffer_p[0], prtBuffer_p[1], prtBuffer_p[2],
                       prtBuffer_p[3], prtBuffer_p[4], prtBuffer_p[5]
                     );
            byteS.append ( tmpStr );

        }
        else if ( prtBuffer_p[0] == VarbleBegin ) //变长帧
        {
            byteS.append ( errNote );
            byteS.append ( "|" );
            byteS.append ( "VarbleBegin:" );
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "buffer[0]=0x%x [1]=0x%x "
                       "[2]=0x%x [3]=0x%x [4]=0x%x [5]=0x%x "
                       "[6]=0x%x [7]=0x%x [8]=0x%x [9]=0x%x [10]=0x%x [11]=0x%x [12]=0x%x \n"
                       "filename:\n",
                       prtBuffer_p[0], prtBuffer_p[1], prtBuffer_p[2],
                       prtBuffer_p[3], prtBuffer_p[4], prtBuffer_p[5],
                       prtBuffer_p[6], prtBuffer_p[7], prtBuffer_p[8],
                       prtBuffer_p[9], prtBuffer_p[10], prtBuffer_p[11],
                       prtBuffer_p[12]
                     );
            byteS.append ( tmpStr );
            bool isFile = false;

            for ( int i = 0; i < ( int ) m_type_list.size(); i++ )
            {
                if ( prtBuffer_p[7] == m_type_list[i] )
                {
                    isFile = true;
                    break;
                }
            }

            if ( isFile )
            {
                byteS.append ( ( char* ) prtBuffer_p + 13, m_filenamelen ); //文件名
                byteS.append ( "\nfile content:" );
                byteS.append ( ( char* ) prtBuffer_p + 13 + m_filenamelen, buffLen - 13 - m_filenamelen );
            }

            byteS.append ( "\n\n" );

        }
        else//未知帧
        {
            byteS.append ( errNote );
            byteS.append ( "|" );
            byteS.append ( "unknown:" );
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "buffer[0]=0x%x [1]=0x%x "
                       "[2]=0x%x [3]=0x%x [4]=0x%x [5]=0x%x \n\n",
                       prtBuffer_p[0], prtBuffer_p[1], prtBuffer_p[2],
                       prtBuffer_p[3], prtBuffer_p[4], prtBuffer_p[5]
                     );
            byteS.append ( tmpStr );
        }
    }
    else
    {
        byteS.append ( errNote );
        byteS.append ( "|" );
        byteS.append ( "debug msg:" );
        byteS.append ( ( char* ) prtBuffer_p, buffLen );
    }


    //取得当前目录
    char* tmp_p = getcwd ( curDir, sizeof ( curDir ) );

    if ( tmp_p == NULL )
    {
        return;
    }

    //在当前目录建目录log
    snprintf ( strPathLog, sizeof ( strPathLog ),
               "%s/log",
               curDir
             );

    if ( access ( strPathLog, 0 ) != 0 )
    {
        vRet = mkdir ( strPathLog, 0755 );

        if ( vRet != 0 )
        {
            return;
        }
    }

    //在log目录下建目录m_Year
    snprintf ( strPathLog, sizeof ( strPathLog ),
               "%s/log/%d",
               curDir, m_datetime_stat.m_Year
             );

    if ( access ( strPathLog, 0 ) != 0 )
    {
        vRet = mkdir ( strPathLog, 0755 );

        if ( vRet != 0 )
        {
            return;
        }
    }

    //日志文件名:debug_${m_Month}${m_Day}.log
    snprintf ( strPathLog, sizeof ( strPathLog ),
               "log/%d/debug_%02d%02d.log",
               m_datetime_stat.m_Year,
               m_datetime_stat.m_Month,
               m_datetime_stat.m_Day );

    flogfile = fopen ( strPathLog, "a" );

    if ( flogfile != NULL )
    {
        fprintf ( flogfile, "%d-%02d-%02d %02d:%02d:%02d.%06d:%s",
                  m_datetime_stat.m_Year,
                  m_datetime_stat.m_Month,
                  m_datetime_stat.m_Day,
                  m_datetime_stat.m_Hour,
                  m_datetime_stat.m_Minute,
                  m_datetime_stat.m_Second,
                  m_datetime_stat.m_tv_usec,
                  byteS.c_str() );
        fclose ( flogfile );
    }

    return;

}

void Slave::printf_xmlVal()
{
    char vRetMsg[512];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  FixedBegin      =  0x%x\n",
               __FILE__, __func__, __LINE__, FixedBegin      );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  VarbleBegin     =  0x%x\n",
               __FILE__, __func__, __LINE__, VarbleBegin     );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  FrameEnd        =  0x%x\n",
               __FILE__, __func__, __LINE__, FrameEnd        );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_KBXD_XDC     =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_KBXD_XDC     );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_YFFW         =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_YFFW         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_QQLLZT       =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_QQLLZT       );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_QQ1SJ_1      =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_QQ1SJ_1      );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_QQ1SJ_0      =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_QQ1SJ_0      );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_QQ2SJ_1      =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_QQ2SJ_1      );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_QQ2SJ_0      =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_QQ2SJ_0      );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_YHSJ_0       =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_YHSJ_0       );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_YHSJ_1       =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_YHSJ_1       );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_KDRK         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_KDRK         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_FDRK         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_FDRK         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_LL           =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_LL           );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WQQSJ_Y1SJ   =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WQQSJ_Y1SJ   );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_YHSJ         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_YHSJ         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WQQSJ        =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WQQSJ        );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_W1SJ         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_W1SJ         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_Y1SJ         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_Y1SJ         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJCSJS_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJCSJS_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJCSCF_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJCSCF_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJCDCC_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJCDCC_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJMGSCW_TYPE =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJMGSCW_TYPE );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJDZCC_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJDZCC_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCSJS_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCSJS_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCSCF_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCSCF_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCDCC_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCDCC_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJMGSCW_TYPE =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJMGSCW_TYPE );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJDZCC_TYPE  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJDZCC_TYPE  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJJS_CSYY    =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJJS_CSYY    );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJCSCF_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJCSCF_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJCDCC_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJCDCC_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_WJMGSCW_CSYY =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_WJMGSCW_CSYY );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  XX_SDXFSJ_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, XX_SDXFSJ_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJZH_CSYY    =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJZH_CSYY    );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJFZH_CSYY   =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJFZH_CSYY   );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCDZQ_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCDZQ_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCDCW_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCDCW_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCSCF_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCSCF_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJCDCC_CSYY  =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJCDCC_CSYY  );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_WJMGSCW_CSYY =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_WJMGSCW_CSYY );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_SBDZ         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_SBDZ         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  SX_JLDZ         =  0x%x\n",
               __FILE__, __func__, __LINE__, SX_JLDZ         );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  upRetList = %d\n",
               __FILE__, __func__, __LINE__, upRetList );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_sock_timeout = %d\n",
               __FILE__, __func__, __LINE__, m_sock_timeout );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_sock_stimeout = %d\n",
               __FILE__, __func__, __LINE__, m_sock_stimeout );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_sock_rtimeout = %d\n",
               __FILE__, __func__, __LINE__, m_sock_rtimeout );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_debug = %d\n",
               __FILE__, __func__, __LINE__, m_debug );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_filenamelen = %d\n",
               __FILE__, __func__, __LINE__, m_filenamelen );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_maxtry = %d\n",
               __FILE__, __func__, __LINE__, m_maxtry );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_fss_houre_ks = %d\n",
               __FILE__, __func__, __LINE__, m_fss_houre_ks );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_fss_houre_js = %d\n",
               __FILE__, __func__, __LINE__, m_fss_houre_js );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  m_port = %d\n",
               __FILE__, __func__, __LINE__, m_port );
    ShowMessage ( vRetMsg );

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ), "%s|%s|%d:  usedIpNum = %d\n",
               __FILE__, __func__, __LINE__, usedIpNum );
    ShowMessage ( vRetMsg );

    for ( int i = 0; i < usedIpNum; i++ )
    {
        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "%s|%s|%d:  m_ips[%d] = [%s]\n",
                   __FILE__, __func__, __LINE__, i, m_ips[i].c_str() );
        ShowMessage ( vRetMsg );
    }

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg, sizeof ( vRetMsg ),
               "%s|%s|%d:  mPthNum = %d\n", __FILE__, __func__, __LINE__, mPthNum );
    ShowMessage ( vRetMsg );

    for ( int i = 0; i < mPthNum; i++ )
    {
        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "%s|%s|%d:  monPaths[%d] = [%s],monFType[%d] = [%d]\n",
                   __FILE__, __func__, __LINE__, i, monPaths[i].c_str(), i, monFType[i] );
        ShowMessage ( vRetMsg );
    }

    return;

}


void Slave::coverXmlVal ( const char* xmlVal_p, int& dstVal )
{
    string tmpString;
    tmpString = xmlVal_p;

    if ( ( tmpString.find ( "0x" ) != std::string::npos )
         || ( tmpString.find ( "0X" ) != std::string::npos )
       )
    {
        sscanf ( xmlVal_p, "%x", &dstVal );
    }
    else
    {
        sscanf ( xmlVal_p, "%d", &dstVal );
    }

    return;
}

int Slave::loadXmlval ( const string& strPath )
{
    int vRet = 0;
    char tmpErr[256];
    memset ( tmpErr, 0, sizeof ( tmpErr ) );

    XMLDocument doc;

    vRet = doc.LoadFile ( strPath.c_str() );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file failed vRet=[%d][%s]",
                   strPath.c_str(), vRet, doc.ErrorName() );

        ShowMessage ( tmpErr );
        return -1;
    }

    const XMLElement* root_p = doc.FirstChildElement ( MC_ROOT );

    if ( root_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s] NULL error",
                   strPath.c_str(), MC_ROOT );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************************************************************
           <operPara> <!--程序运行时的有关参数配置-->
    *******************************************************************/
    const XMLElement* operParaE_P = root_p->FirstChildElement ( MC_OPERPARA );

    if ( operParaE_P == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s] NULL error",
                   strPath.c_str(), MC_OPERPARA );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************upRetList*************************************/
    const XMLElement* upRetListE_p = operParaE_P->FirstChildElement ( MC_UPRETLIST );

    if ( upRetListE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_UPRETLIST );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = upRetListE_p->QueryIntText ( &upRetList );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_UPRETLIST,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************sockWRTimeOut*************************************/
    const XMLElement* sockWRTimeOutE_p = operParaE_P->FirstChildElement ( MC_SOCKWRTIMEOUT );

    if ( sockWRTimeOutE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_SOCKWRTIMEOUT );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = sockWRTimeOutE_p->QueryIntText ( &m_sock_timeout );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_SOCKWRTIMEOUT,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************sockSndTimeOut*************************************/
    const XMLElement* sockSTimeOutE_p = operParaE_P->FirstChildElement ( MC_SOCKSNDTIMEOUT );

    if ( sockSTimeOutE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_SOCKSNDTIMEOUT );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = sockSTimeOutE_p->QueryIntText ( &m_sock_stimeout );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_SOCKSNDTIMEOUT,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************sockRcvTimeOut*************************************/
    const XMLElement* sockRTimeOutE_p = operParaE_P->FirstChildElement ( MC_SOCKRCVTIMEOUT );

    if ( sockRTimeOutE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_SOCKRCVTIMEOUT );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = sockRTimeOutE_p->QueryIntText ( &m_sock_rtimeout );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_SOCKRCVTIMEOUT,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************debug*************************************/
    const XMLElement* debugE_p = operParaE_P->FirstChildElement ( MC_DEBUG );

    if ( debugE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_DEBUG );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = debugE_p->QueryIntText ( &m_debug );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_DEBUG,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************fileNameLen*************************************/
    const XMLElement* fileNameLenE_p = operParaE_P->FirstChildElement ( MC_FILENAMELEN );

    if ( fileNameLenE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_FILENAMELEN );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = fileNameLenE_p->QueryIntText ( &m_filenamelen );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_FILENAMELEN,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************maxTry*************************************/
    const XMLElement* maxTryE_p = operParaE_P->FirstChildElement ( MC_MAXTRY );

    if ( maxTryE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_MAXTRY );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = maxTryE_p->QueryIntText ( &m_maxtry );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_MAXTRY,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************houreBegine*************************************/
    const XMLElement* houreBegineE_p = operParaE_P->FirstChildElement ( MC_HOUREBEGINE );

    if ( houreBegineE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_HOUREBEGINE );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = houreBegineE_p->QueryIntText ( &m_fss_houre_ks );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_HOUREBEGINE,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************houreEnd*************************************/
    const XMLElement* houreEndE_p = operParaE_P->FirstChildElement ( MC_HOUREEND );

    if ( houreEndE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_HOUREEND );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = houreEndE_p->QueryIntText ( &m_fss_houre_js );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_HOUREEND,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************listenPort*************************************/
    const XMLElement* listenPortE_p = operParaE_P->FirstChildElement ( MC_LISTENPORT );

    if ( listenPortE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_LISTENPORT );

        ShowMessage ( tmpErr );

        return -1;
    }

    vRet = listenPortE_p->QueryIntText ( &m_port );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] QueryIntText error[%s]",
                   strPath.c_str(), MC_OPERPARA, MC_LISTENPORT,
                   doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************<masterIPS ipNum="2"> <!--主站ip地址配置-->***/
    const XMLElement* masterIPSE_p = operParaE_P->FirstChildElement ( MC_MASTERIPS );

    if ( masterIPSE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_OPERPARA, MC_MASTERIPS );

        ShowMessage ( tmpErr );

        return -1;
    }

    int vIpNum = 0;
    vRet = masterIPSE_p->QueryIntAttribute ( MC_IPNUM, &vIpNum );

    if ( vRet != 0 )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "[%s] xml file's XMLElement [%s]->[%s] QueryIntAttribute %s failed  vRet=[%d][%s]",
                   strPath.c_str(), MC_OPERPARA, MC_MASTERIPS, MC_IPNUM,
                   vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );

        ShowMessage ( tmpErr );

        return -1;
    }

    if ( vIpNum > MC_PRLUP_MAX )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "配置的ip数量为[%d]大于[%d]个，只加载前[%d]个ip地址",
                   vIpNum, MC_PRLUP_MAX, MC_PRLUP_MAX );
        ShowMessage ( tmpErr );

        usedIpNum = MC_PRLUP_MAX;
    }
    else
    {
        usedIpNum = vIpNum;
    }

    if ( usedIpNum > 0 )
    {
        /************************ip*************************************/
        const XMLElement* ipE_p = masterIPSE_p->FirstChildElement ( MC_IP );

        if ( ipE_p == NULL )
        {
            snprintf ( tmpErr, sizeof ( tmpErr ),
                       "[%s] xml file's XMLElement [%s]->[%s]the 0th [%s] NULL error",
                       strPath.c_str(), MC_OPERPARA, MC_MASTERIPS, MC_IP );

            ShowMessage ( tmpErr );

            return -1;
        }

        const char* ipT_p = ipE_p->GetText();

        if ( ipT_p == NULL )
        {
            snprintf ( tmpErr, sizeof ( tmpErr ),
                       "[%s] xml file's XMLElement [%s]->[%s]the 0th[%s] GetText NULL error",
                       strPath.c_str(), MC_OPERPARA, MC_MASTERIPS, MC_IP );

            ShowMessage ( tmpErr );

            return -1;
        }

        m_ips[0] = ipT_p;

        for ( int i = 1; i < usedIpNum; i++ )
        {
            ipE_p = ipE_p->NextSiblingElement ( MC_IP );

            if ( ipE_p == NULL )
            {

                snprintf ( tmpErr, sizeof ( tmpErr ),
                           "[%s] xml file's XMLElement [%s]->[%s] the %dth [%s] is NULL",
                           strPath.c_str(), MC_OPERPARA, MC_MASTERIPS, i + 1, MC_IP );
                ShowMessage ( tmpErr );

                return -1;
            }

            ipT_p = ipE_p->GetText();

            if ( ipT_p == NULL )
            {
                snprintf ( tmpErr, sizeof ( tmpErr ),
                           "[%s] xml file's XMLElement [%s]->[%s]the %dth [%s] GetText NULL error",
                           strPath.c_str(), MC_OPERPARA, MC_MASTERIPS, i + 1, MC_IP );

                ShowMessage ( tmpErr );

                return -1;
            }

            m_ips[i] = ipT_p;

        }

    }

    /************************<monitorDir pathNum="2">
    <!--上传文件目录监视，此配置项是可选的；可以不配置:添加文件通过显示的方法添加-->***/
    int cfgMFlag = 0;
    const XMLElement* monitorDirE_p = operParaE_P->FirstChildElement ( MC_MONITORDIR );

    if ( monitorDirE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "上传文件目录监视没有配置,可以不配置"
                   "不配置上传文件列表需要通过显示的方法添加" );
        ShowMessage ( tmpErr );
        cfgMFlag = 1;
    }

    if ( cfgMFlag == 1 )
    {
        mPthNum = 0;
    }
    else
    {
        int vPathNum = 0;
        vRet = monitorDirE_p->QueryIntAttribute ( MC_PATHNUM, &vPathNum );

        if ( vRet != 0 )
        {
            snprintf ( tmpErr, sizeof ( tmpErr ),
                       "[%s] xml file's XMLElement [%s]->[%s] "
                       "QueryIntAttribute %s failed  vRet=[%d][%s]",
                       strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, MC_PATHNUM,
                       vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );

            ShowMessage ( tmpErr );

            return -1;
        }

        if ( vPathNum > MC_FILEMON_MAX )
        {
            snprintf ( tmpErr, sizeof ( tmpErr ),
                       "配置的监视目录数量为[%d]大于[%d]个，只加载前[%d]个目录",
                       vPathNum, MC_FILEMON_MAX, MC_FILEMON_MAX );
            ShowMessage ( tmpErr );

            mPthNum = MC_FILEMON_MAX;
        }
        else
        {
            mPthNum = vPathNum;
        }

        if ( mPthNum > 0 )
        {
            /************************<dirPath fileType="149"></dirPath>
             *************************************/
            const XMLElement* dirPathE_p = monitorDirE_p->FirstChildElement ( MC_DIRPATH );

            if ( dirPathE_p == NULL )
            {
                snprintf ( tmpErr, sizeof ( tmpErr ),
                           "[%s] xml file's XMLElement [%s]->[%s]the 0th [%s] NULL error",
                           strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, MC_DIRPATH );

                ShowMessage ( tmpErr );

                return -1;
            }

            const char* dirPathT_p = dirPathE_p->GetText();

            if ( dirPathT_p == NULL )
            {
                snprintf ( tmpErr, sizeof ( tmpErr ),
                           "[%s] xml file's XMLElement [%s]->[%s]the 0th[%s] GetText NULL error",
                           strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, MC_DIRPATH );

                ShowMessage ( tmpErr );

                return -1;
            }

            monPaths[0] = dirPathT_p;

            int vFType = 0;
            vRet = dirPathE_p->QueryIntAttribute ( MC_FILETYPE, &vFType );

            if ( vRet != 0 )
            {
                snprintf ( tmpErr, sizeof ( tmpErr ),
                           "[%s] xml file's XMLElement [%s]->[%s]the 0th[%s]"
                           "  QueryIntAttribute %s failed  vRet=[%d][%s]",
                           strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, MC_DIRPATH, MC_FILETYPE,
                           vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );

                ShowMessage ( tmpErr );

                return -1;
            }

            monFType[0] = vFType;

            for ( int i = 1; i < mPthNum; i++ )
            {
                dirPathE_p = dirPathE_p->NextSiblingElement ( MC_DIRPATH );

                if ( dirPathE_p == NULL )
                {

                    snprintf ( tmpErr, sizeof ( tmpErr ),
                               "[%s] xml file's XMLElement [%s]->[%s] the %dth [%s] is NULL",
                               strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, i + 1, MC_DIRPATH );
                    ShowMessage ( tmpErr );

                    return -1;
                }

                dirPathT_p = dirPathE_p->GetText();

                if ( dirPathT_p == NULL )
                {
                    snprintf ( tmpErr, sizeof ( tmpErr ),
                               "[%s] xml file's XMLElement [%s]->[%s]the %dth [%s] GetText NULL error",
                               strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, i + 1, MC_DIRPATH );

                    ShowMessage ( tmpErr );

                    return -1;
                }

                monPaths[i] = dirPathT_p;

                vRet = dirPathE_p->QueryIntAttribute ( MC_FILETYPE, &vFType );

                if ( vRet != 0 )
                {
                    snprintf ( tmpErr, sizeof ( tmpErr ),
                               "[%s] xml file's XMLElement [%s]->[%s]the %dth[%s]"
                               "  QueryIntAttribute %s failed  vRet=[%d][%s]",
                               strPath.c_str(), MC_OPERPARA, MC_MONITORDIR, i + 1, MC_DIRPATH, MC_FILETYPE,
                               vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );

                    ShowMessage ( tmpErr );

                    return -1;
                }

                monFType[i] = vFType;

            }

        }
    }


    /******************************************************************
            <slave><!-- 102协议子站上传帧元素值配置-->
    *******************************************************************/
    const XMLElement* slave_p = root_p->FirstChildElement ( MC_SLAVE );

    if ( slave_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s] NULL error",
                   strPath.c_str(), MC_SLAVE );

        ShowMessage ( tmpErr );

        return -1;
    }

    /************************FixedBegin*************************************/
    const XMLElement* FixedBeginE_p = slave_p->FirstChildElement ( MC_FIXEDBEGIN );

    if ( FixedBeginE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_FIXEDBEGIN );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* FixedBeginT_p = FixedBeginE_p->GetText();

    if ( FixedBeginT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_FIXEDBEGIN );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( FixedBeginT_p, FixedBegin );

    /**************************VarbleBegin***********************************/
    const XMLElement* VarbleBeginE_p = slave_p->FirstChildElement ( MC_VARBLEBEGIN );

    if ( VarbleBeginE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_VARBLEBEGIN );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* VarbleBeginT_p = VarbleBeginE_p->GetText();

    if ( VarbleBeginT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_VARBLEBEGIN );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( VarbleBeginT_p, VarbleBegin );

    /**************************FrameEnd***********************************/
    const XMLElement* FrameEndE_p = slave_p->FirstChildElement ( MC_FRAMEEND );

    if ( FrameEndE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_FRAMEEND );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* FrameEndT_p = FrameEndE_p->GetText();

    if ( FrameEndT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_FRAMEEND );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( FrameEndT_p, FrameEnd );

    /**************************SX_KBXD_XDC***********************************/
    const XMLElement* VSQE_p = slave_p->FirstChildElement ( MC_VSQ );

    if ( VSQE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_VSQ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* VSQT_p = VSQE_p->GetText();

    if ( VSQT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_VSQ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( VSQT_p, SX_KBXD_XDC );

    /*************************************************************
        <controlField> <!--控制域-->
    **************************************************************/
    const XMLElement* controlFieldE_p = slave_p->FirstChildElement ( MC_CONTROLFIELD );

    if ( controlFieldE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD );

        ShowMessage ( tmpErr );

        return -1;
    }

    //<downLink>    <!--下行数据-->
    const XMLElement* downLinkE_p = controlFieldE_p->FirstChildElement ( MC_DOWNLINK );

    if ( downLinkE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK );

        ShowMessage ( tmpErr );

        return -1;
    }

    //<fixed>     <!--固定帧-->
    const XMLElement* fixedCDE_p = downLinkE_p->FirstChildElement ( MC_FIXED );

    if ( fixedCDE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************xx_yffw*********************************/
    const XMLElement* xxyffwE_p = fixedCDE_p->FirstChildElement ( MC_XX_YFFW );

    if ( xxyffwE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_YFFW );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxyffwT_p = xxyffwE_p->GetText();

    if ( xxyffwT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_YFFW );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxyffwT_p, XX_YFFW );

    /******************xx_qqllzt*********************************/
    const XMLElement* xxqqllztE_p = fixedCDE_p->FirstChildElement ( MC_XX_QQLLZT );

    if ( xxqqllztE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQLLZT );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxqqllztT_p = xxqqllztE_p->GetText();

    if ( xxqqllztT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQLLZT );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxqqllztT_p, XX_QQLLZT );

    /******************xx_qq1sj_1*********************************/
    const XMLElement* xxqq1sj1E_p = fixedCDE_p->FirstChildElement ( MC_XX_QQ1SJ_1 );

    if ( xxqq1sj1E_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ1SJ_1 );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxqq1sj1T_p = xxqq1sj1E_p->GetText();

    if ( xxqq1sj1T_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ1SJ_1 );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxqq1sj1T_p, XX_QQ1SJ_1 );

    /******************xx_qq1sj_0*********************************/
    const XMLElement* xxqq1sj0E_p = fixedCDE_p->FirstChildElement ( MC_XX_QQ1SJ_0 );

    if ( xxqq1sj0E_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ1SJ_0 );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxqq1sj0T_p = xxqq1sj0E_p->GetText();

    if ( xxqq1sj0T_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ1SJ_0 );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxqq1sj0T_p, XX_QQ1SJ_0 );

    /******************xx_qq2sj_1*********************************/
    const XMLElement* xxqq2sj1E_p = fixedCDE_p->FirstChildElement ( MC_XX_QQ2SJ_1 );

    if ( xxqq2sj1E_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ2SJ_1 );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxqq2sj1T_p = xxqq2sj1E_p->GetText();

    if ( xxqq2sj1T_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ2SJ_1 );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxqq2sj1T_p, XX_QQ2SJ_1 );

    /******************xx_qq2sj_0*********************************/
    const XMLElement* xxqq2sj0E_p = fixedCDE_p->FirstChildElement ( MC_XX_QQ2SJ_0 );

    if ( xxqq2sj0E_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ2SJ_0 );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxqq2sj0T_p = xxqq2sj0E_p->GetText();

    if ( xxqq2sj0T_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_FIXED, MC_XX_QQ2SJ_0 );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxqq2sj0T_p, XX_QQ2SJ_0 );

    //<variable>  <!--变长帧-->
    const XMLElement* variableCDE_p = downLinkE_p->FirstChildElement ( MC_VARIABLE );

    if ( variableCDE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_VARIABLE );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************xx_yhsj_0*********************************/
    const XMLElement* xxyhsj0E_p = variableCDE_p->FirstChildElement ( MC_XX_YHSJ_0 );

    if ( xxyhsj0E_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_VARIABLE, MC_XX_YHSJ_0 );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxyhsj0T_p = xxyhsj0E_p->GetText();

    if ( xxyhsj0T_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_VARIABLE, MC_XX_YHSJ_0 );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxyhsj0T_p, XX_YHSJ_0 );

    /******************xx_yhsj_1*********************************/
    const XMLElement* xxyhsj1E_p = variableCDE_p->FirstChildElement ( MC_XX_YHSJ_1 );

    if ( xxyhsj1E_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_VARIABLE, MC_XX_YHSJ_1 );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxyhsj1T_p = xxyhsj1E_p->GetText();

    if ( xxyhsj1T_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_DOWNLINK, MC_VARIABLE, MC_XX_YHSJ_1 );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxyhsj1T_p, XX_YHSJ_1 );


    //<upLink> <!--上行数据-->
    const XMLElement* upLinkE_p = controlFieldE_p->FirstChildElement ( MC_UPLINK );

    if ( upLinkE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK );

        ShowMessage ( tmpErr );

        return -1;
    }

    //<fixed>     <!--固定帧-->
    const XMLElement* fixedCUE_p = upLinkE_p->FirstChildElement ( MC_FIXED );

    if ( fixedCUE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************sx_kdrk*********************************/
    const XMLElement* sxkdrkE_p = fixedCUE_p->FirstChildElement ( MC_SX_KDRK );

    if ( sxkdrkE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_KDRK );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxkdrkT_p = sxkdrkE_p->GetText();

    if ( sxkdrkT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_KDRK );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxkdrkT_p, SX_KDRK );

    /******************sx_fdrk*********************************/
    const XMLElement* sxfdrkE_p = fixedCUE_p->FirstChildElement ( MC_SX_FDRK );

    if ( sxfdrkE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_FDRK );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxfdrkT_p = sxfdrkE_p->GetText();

    if ( sxfdrkT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_FDRK );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxfdrkT_p, SX_FDRK );

    /******************sx_ll*********************************/
    const XMLElement* sxllE_p = fixedCUE_p->FirstChildElement ( MC_SX_LL );

    if ( sxllE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_LL );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxllT_p = sxllE_p->GetText();

    if ( sxllT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_LL );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxllT_p, SX_LL );

    /******************sx_wqqsj_y1sj*********************************/
    const XMLElement* sxwqqsjy1sjE_p = fixedCUE_p->FirstChildElement ( MC_SX_WQQSJ_Y1SJ );

    if ( sxwqqsjy1sjE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_WQQSJ_Y1SJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwqqsjy1sjT_p = sxwqqsjy1sjE_p->GetText();

    if ( sxwqqsjy1sjT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_WQQSJ_Y1SJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwqqsjy1sjT_p, SX_WQQSJ_Y1SJ );

    /******************sx_yhsj*********************************/
    const XMLElement* sxyhsjE_p = fixedCUE_p->FirstChildElement ( MC_SX_YHSJ );

    if ( sxyhsjE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_YHSJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxyhsjT_p = sxyhsjE_p->GetText();

    if ( sxyhsjT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_YHSJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxyhsjT_p, SX_YHSJ );

    /******************sx_wqqsj*********************************/
    const XMLElement* sxwqqsjE_p = fixedCUE_p->FirstChildElement ( MC_SX_WQQSJ );

    if ( sxwqqsjE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_WQQSJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwqqsjT_p = sxwqqsjE_p->GetText();

    if ( sxwqqsjT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_FIXED, MC_SX_WQQSJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwqqsjT_p, SX_WQQSJ );

    //<variable>  <!--变长帧-->
    const XMLElement* variableCUE_p = upLinkE_p->FirstChildElement ( MC_VARIABLE );

    if ( variableCUE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_VARIABLE );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************sx_w1sj*********************************/
    const XMLElement* sxw1sjE_p = variableCUE_p->FirstChildElement ( MC_SX_W1SJ );

    if ( sxw1sjE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_VARIABLE, MC_SX_W1SJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxw1sjT_p = sxw1sjE_p->GetText();

    if ( sxw1sjT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_VARIABLE, MC_SX_W1SJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxw1sjT_p, SX_W1SJ );

    /******************sx_y1sj*********************************/
    const XMLElement* sxy1sjE_p = variableCUE_p->FirstChildElement ( MC_SX_Y1SJ );

    if ( sxy1sjE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_VARIABLE, MC_SX_Y1SJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxy1sjT_p = sxy1sjE_p->GetText();

    if ( sxy1sjT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CONTROLFIELD, MC_UPLINK, MC_VARIABLE, MC_SX_Y1SJ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxy1sjT_p, SX_Y1SJ );


    /*************************************************************
        <typeIdentification> <!--有关于应用报文类型-->
    **************************************************************/
    const XMLElement* typeIDE_p = slave_p->FirstChildElement ( MC_TYPEID );

    if ( typeIDE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID );

        ShowMessage ( tmpErr );

        return -1;
    }

    //<downLink>    <!--下行数据-->
    const XMLElement* downLinkTE_p = typeIDE_p->FirstChildElement ( MC_DOWNLINK );

    if ( downLinkTE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************xx_wjcsjs_type*********************************/
    const XMLElement* xxwjcsjstypeE_p = downLinkTE_p->FirstChildElement ( MC_XX_WJCSJS_TYPE );

    if ( xxwjcsjstypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJCSJS_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjcsjstypeT_p = xxwjcsjstypeE_p->GetText();

    if ( xxwjcsjstypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJCSJS_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjcsjstypeT_p, XX_WJCSJS_TYPE );

    /******************xx_wjcscf_type*********************************/
    const XMLElement* xxwjcscftypeE_p = downLinkTE_p->FirstChildElement ( MC_XX_WJCSCF_TYPE );

    if ( xxwjcscftypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJCSCF_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjcscftypeT_p = xxwjcscftypeE_p->GetText();

    if ( xxwjcscftypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJCSCF_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjcscftypeT_p, XX_WJCSCF_TYPE );

    /******************xx_wjcdcc_type*********************************/
    const XMLElement* xxwjcdcctypeE_p = downLinkTE_p->FirstChildElement ( MC_XX_WJCDCC_TYPE );

    if ( xxwjcdcctypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJCDCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjcdcctypeT_p = xxwjcdcctypeE_p->GetText();

    if ( xxwjcdcctypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJCDCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjcdcctypeT_p, XX_WJCDCC_TYPE );

    /******************xx_wjmgscw_type*********************************/
    const XMLElement* xxwjmgscwtypeE_p = downLinkTE_p->FirstChildElement (
                                         MC_XX_WJMGSCW_TYPE );

    if ( xxwjmgscwtypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJMGSCW_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjmgscwtypeT_p = xxwjmgscwtypeE_p->GetText();

    if ( xxwjmgscwtypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJMGSCW_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjmgscwtypeT_p, XX_WJMGSCW_TYPE );

    /******************xx_wjdzcc_type*********************************/
    const XMLElement* xxwjdzcctypeE_p = downLinkTE_p->FirstChildElement ( MC_XX_WJDZCC_TYPE );

    if ( xxwjdzcctypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJDZCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjdzcctypeT_p = xxwjdzcctypeE_p->GetText();

    if ( xxwjdzcctypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_DOWNLINK, MC_XX_WJDZCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjdzcctypeT_p, XX_WJDZCC_TYPE );

    //<upLink> <!--上行数据-->
    const XMLElement* upLinkTE_p = typeIDE_p->FirstChildElement ( MC_UPLINK );

    if ( upLinkTE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************sx_wjcsjs_type*********************************/
    const XMLElement* sxwjcsjstypeE_p = upLinkTE_p->FirstChildElement ( MC_SX_WJCSJS_TYPE );

    if ( sxwjcsjstypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJCSJS_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcsjstypeT_p = sxwjcsjstypeE_p->GetText();

    if ( sxwjcsjstypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJCSJS_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcsjstypeT_p, SX_WJCSJS_TYPE );

    /******************sx_wjcscf_type*********************************/
    const XMLElement* sxwjcscftypeE_p = upLinkTE_p->FirstChildElement ( MC_SX_WJCSCF_TYPE );

    if ( sxwjcscftypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJCSCF_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcscftypeT_p = sxwjcscftypeE_p->GetText();

    if ( sxwjcscftypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJCSCF_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcscftypeT_p, SX_WJCSCF_TYPE );

    /******************sx_wjcdcc_type*********************************/
    const XMLElement* sxwjcdcctypeE_p = upLinkTE_p->FirstChildElement ( MC_SX_WJCDCC_TYPE );

    if ( sxwjcdcctypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJCDCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcdcctypeT_p = sxwjcdcctypeE_p->GetText();

    if ( sxwjcdcctypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJCDCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcdcctypeT_p, SX_WJCDCC_TYPE );

    /******************sx_wjmgscw_type*********************************/
    const XMLElement* sxwjmgscwtypeE_p = upLinkTE_p->FirstChildElement ( MC_SX_WJMGSCW_TYPE );

    if ( sxwjmgscwtypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJMGSCW_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjmgscwtypeT_p = sxwjmgscwtypeE_p->GetText();

    if ( sxwjmgscwtypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJMGSCW_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjmgscwtypeT_p, SX_WJMGSCW_TYPE );

    /******************sx_wjdzcc_type*********************************/
    const XMLElement* sxwjdzcctypeE_p = upLinkTE_p->FirstChildElement ( MC_SX_WJDZCC_TYPE );

    if ( sxwjdzcctypeE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJDZCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjdzcctypeT_p = sxwjdzcctypeE_p->GetText();

    if ( sxwjdzcctypeT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_TYPEID, MC_UPLINK, MC_SX_WJDZCC_TYPE );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjdzcctypeT_p, SX_WJDZCC_TYPE );

    /*************************************************************
        <COT> <!--传送原因-->
    **************************************************************/
    const XMLElement* COTE_p = slave_p->FirstChildElement ( MC_COT );

    if ( COTE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT );

        ShowMessage ( tmpErr );

        return -1;
    }

    //<downLink>    <!--下行数据-->
    const XMLElement* downLinkCE_p = COTE_p->FirstChildElement ( MC_DOWNLINK );

    if ( downLinkCE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************xx_wjjs_csyy*********************************/
    const XMLElement* xxwjjscsyyE_p = downLinkCE_p->FirstChildElement ( MC_XX_WJJS_CSYY );

    if ( xxwjjscsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJJS_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjjscsyyT_p = xxwjjscsyyE_p->GetText();

    if ( xxwjjscsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJJS_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjjscsyyT_p, XX_WJJS_CSYY );

    /******************xx_wjcscf_csyy*********************************/
    const XMLElement* xxwjcscfcsyyE_p = downLinkCE_p->FirstChildElement ( MC_XX_WJCSCF_CSYY );

    if ( xxwjcscfcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJCSCF_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjcscfcsyyT_p = xxwjcscfcsyyE_p->GetText();

    if ( xxwjcscfcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJCSCF_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjcscfcsyyT_p, XX_WJCSCF_CSYY );

    /******************xx_wjcdcc_csyy*********************************/
    const XMLElement* xxwjcdcccsyyE_p = downLinkCE_p->FirstChildElement ( MC_XX_WJCDCC_CSYY );

    if ( xxwjcdcccsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJCDCC_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjcdcccsyyT_p = xxwjcdcccsyyE_p->GetText();

    if ( xxwjcdcccsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJCDCC_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjcdcccsyyT_p, XX_WJCDCC_CSYY );

    /******************xx_wjmgscw_csyy*********************************/
    const XMLElement* xxwjmgscwcsyyE_p = downLinkCE_p->FirstChildElement (
                                         MC_XX_WJMGSCW_CSYY );

    if ( xxwjmgscwcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJMGSCW_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxwjmgscwcsyyT_p = xxwjmgscwcsyyE_p->GetText();

    if ( xxwjmgscwcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_WJMGSCW_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxwjmgscwcsyyT_p, XX_WJMGSCW_CSYY );

    /******************xx_sdxfsj_csyy*********************************/
    const XMLElement* xxsdxfsjcsyyE_p = downLinkCE_p->FirstChildElement ( MC_XX_SDXFSJ_CSYY );

    if ( xxsdxfsjcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_SDXFSJ_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* xxsdxfsjcsyyT_p = xxsdxfsjcsyyE_p->GetText();

    if ( xxsdxfsjcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_DOWNLINK, MC_XX_SDXFSJ_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( xxsdxfsjcsyyT_p, XX_SDXFSJ_CSYY );

    //<upLink> <!--上行数据-->
    const XMLElement* upLinkCE_p = COTE_p->FirstChildElement ( MC_UPLINK );

    if ( upLinkCE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************sx_wjzh_csyy*********************************/
    const XMLElement* sxwjzhcsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJZH_CSYY );

    if ( sxwjzhcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJZH_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjzhcsyyT_p = sxwjzhcsyyE_p->GetText();

    if ( sxwjzhcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJZH_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjzhcsyyT_p, SX_WJZH_CSYY );

    /******************sx_wjfzh_csyy*********************************/
    const XMLElement* sxwjfzhcsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJFZH_CSYY );

    if ( sxwjfzhcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJFZH_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjfzhcsyyT_p = sxwjfzhcsyyE_p->GetText();

    if ( sxwjfzhcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJFZH_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjfzhcsyyT_p, SX_WJFZH_CSYY );

    /******************sx_wjcdzq_csyy*********************************/
    const XMLElement* sxwjcdzqcsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJCDZQ_CSYY );

    if ( sxwjcdzqcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCDZQ_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcdzqcsyyT_p = sxwjcdzqcsyyE_p->GetText();

    if ( sxwjcdzqcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCDZQ_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcdzqcsyyT_p, SX_WJCDZQ_CSYY );

    /******************sx_wjcdcw_csyy*********************************/
    const XMLElement* sxwjcdcwcsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJCDCW_CSYY );

    if ( sxwjcdcwcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCDCW_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcdcwcsyyT_p = sxwjcdcwcsyyE_p->GetText();

    if ( sxwjcdcwcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCDCW_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcdcwcsyyT_p, SX_WJCDCW_CSYY );

    /******************sx_wjcscf_csyy*********************************/
    const XMLElement* sxwjcscfcsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJCSCF_CSYY );

    if ( sxwjcscfcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCSCF_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcscfcsyyT_p = sxwjcscfcsyyE_p->GetText();

    if ( sxwjcscfcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCSCF_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcscfcsyyT_p, SX_WJCSCF_CSYY );

    /******************sx_wjcdcc_csyy*********************************/
    const XMLElement* sxwjcdcccsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJCDCC_CSYY );

    if ( sxwjcdcccsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCDCC_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjcdcccsyyT_p = sxwjcdcccsyyE_p->GetText();

    if ( sxwjcdcccsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJCDCC_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjcdcccsyyT_p, SX_WJCDCC_CSYY );

    /******************sx_wjmgscw_csyy*********************************/
    const XMLElement* sxwjmgscwcsyyE_p = upLinkCE_p->FirstChildElement ( MC_SX_WJMGSCW_CSYY );

    if ( sxwjmgscwcsyyE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJMGSCW_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxwjmgscwcsyyT_p = sxwjmgscwcsyyE_p->GetText();

    if ( sxwjmgscwcsyyT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_COT, MC_UPLINK, MC_SX_WJMGSCW_CSYY );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxwjmgscwcsyyT_p, SX_WJMGSCW_CSYY );

    /*************************************************************
        <cmnAdrsASDU> <!--用户服务单元公共地址-->
    **************************************************************/
    const XMLElement* cmnAdrsE_p = slave_p->FirstChildElement ( MC_CMNADRSASDU );

    if ( cmnAdrsE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CMNADRSASDU );

        ShowMessage ( tmpErr );

        return -1;
    }

    /******************sx_sbdz*********************************/
    const XMLElement* sxsbdzE_p = cmnAdrsE_p->FirstChildElement ( MC_SX_SBDZ );

    if ( sxsbdzE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CMNADRSASDU, MC_SX_SBDZ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxsbdzT_p = sxsbdzE_p->GetText();

    if ( sxsbdzT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CMNADRSASDU, MC_SX_SBDZ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxsbdzT_p, SX_SBDZ );

    /******************sx_jldz*********************************/
    const XMLElement* sxjldzE_p = cmnAdrsE_p->FirstChildElement ( MC_SX_JLDZ );

    if ( sxjldzE_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CMNADRSASDU, MC_SX_JLDZ );

        ShowMessage ( tmpErr );

        return -1;
    }

    const char* sxjldzT_p = sxjldzE_p->GetText();

    if ( sxjldzT_p == NULL )
    {
        snprintf ( tmpErr, sizeof ( tmpErr ),
                   "load [%s] xml file's XMLElement [%s]->[%s]->[%s] GetText NULL error",
                   strPath.c_str(), MC_SLAVE, MC_CMNADRSASDU, MC_SX_JLDZ );

        ShowMessage ( tmpErr );

        return -1;
    }

    coverXmlVal ( sxjldzT_p, SX_JLDZ );


    return 0;

}

bool Slave::getFileContent ( char* content,
                             const string& file_name,
                             int& len
                           )
{

    char vRetMsg[256];
    char readBuf[1024];
    int l = 0;
    int tmpLen = 0;

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( readBuf, 0, sizeof ( readBuf ) );

    FILE* fp = fopen ( file_name.c_str(), "r" );


    if ( fp == NULL )
    {
        snprintf ( vRetMsg,
                   sizeof ( vRetMsg ),
                   "getFileContent open [%s] fail[%s]",
                   file_name.c_str(), strerror ( errno )
                 );
        ShowMessage ( vRetMsg );
        return false;
    }

    while ( ( l = fread ( readBuf,
                          sizeof ( char ),
                          sizeof ( readBuf ) - 1,
                          fp
                        )
            ) > 0

          )
    {
        tmpLen += l;

        if ( tmpLen > len )
        {
            snprintf ( vRetMsg,
                       sizeof ( vRetMsg ),
                       "getFileContent file %s length[%d] biger than system config[%d]",
                       file_name.c_str(), tmpLen, len );
            ShowMessage ( vRetMsg );
            fclose ( fp );
            return false;
        }

        strncat ( content, readBuf, l );

        memset ( readBuf, 0, sizeof ( readBuf ) );

    }

    fclose ( fp );

    len = tmpLen;

    return true;

}

Slave::FileListHandler::FileListHandler ( Slave* slave_p )
{
    mHslave_p = slave_p;
}
Slave::FileListHandler::~FileListHandler()
{

}

void Slave::FileListHandler::handleMessage ( Message msg )
{
    Slave* slv = mHslave_p;

    if ( msg.mWhat != PARSE_ADD_FILE || slv->doingFlag == 1 )
    {
        //slv->doingFlag = 0;
        return;
    }


    slv->doingFlag = 1;

    char vRetMsg[512];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    if ( ( ( slv->getDebug() ) & 0x02 ) == 2 )
    {
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "%s|%s|%d:  slv->getFLLsize()[%d]\n",
                   __FILE__, __func__, __LINE__, slv->getFLLsize() );
        slv->ShowMessage ( vRetMsg );
    }

    FileList* tmpFst_p = NULL;
    string tmpStrg;
    bool fileFlag = false;
    bool tmpUsing = false;

    int countTime[MC_PRLUP_MAX];
    memset ( countTime, 0, sizeof ( countTime ) );

    int countSub = 0;
    int lstDex = 0;

    while ( 1 )
    {
        fileFlag = false;
        lstDex = 0;

        for ( int i = 0; i < slv->getFLLsize(); i++ )
        {
            tmpFst_p = slv->m_FileList_Lister[i];

            if ( tmpFst_p->getSize() > 0 )
            {
                fileFlag = true;
                lstDex   = i;
                break;
            }
        }

        //没找到了需要加载的文件
        if ( fileFlag == false )
        {
            break;
        }


        tmpUsing = false;
        countSub = 0;

        for ( int i = 0; i < slv->getIpNum(); i++ )
        {
            if ( slv->m_Buff102[i].isUsing() == true
               ) //有缓冲区在用
            {
                if ( slv->m_Sock[i].is_using() == false ) //socket 不可用
                {
                    //有过超时过程则不再等待，并清空对应buffer状态
                    if ( slv->noSockFlag[i] == 1 )
                    {
                        slv->m_Buff102[i].clearBuffFun();
                        countTime[i] = 0;
                        continue;
                    }
                    else
                    {
                        countTime[i]++;
                        countSub = i;
                    }
                }
                else if ( slv->noSockFlag[i] == 1 )
                {
                    //当前socket可用,不符合可用状态的socket标识需要修改
                    slv->noSockFlag[i] = 0;
                    countTime[i] = 0;
                }

                tmpUsing = true;
                break;
            }
        }

        //上传数据缓冲区为空 且 有需要加载的文件
        if ( false == tmpUsing )
        {
            if ( ( ( slv->getDebug() ) & 0x02 ) == 2 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg,
                           sizeof ( vRetMsg ),
                           "%s|%s|%d:slv->getFLLsize()=[%d]第%d个文件类型链"
                           " tmpFst_p->getSize()=[%d],type=[%d]\n",
                           __FILE__, __func__, __LINE__,
                           slv->getFLLsize(), lstDex + 1, tmpFst_p->getSize(), tmpFst_p->getType() );

                slv->ShowMessage ( vRetMsg );
            }

            //读文件到临时缓冲区
            char content[MAXBUFFLEN];
            memset ( content, 0, sizeof ( content ) );
            int len = sizeof ( content ) - 1;

            if ( ( ( slv->getDebug() ) & 0x02 ) == 2 )
            {
                snprintf ( vRetMsg,
                           sizeof ( vRetMsg ),
                           "%s|%s|%d:  before getFileContent len=[%d]\n",
                           __FILE__, __func__, __LINE__, len );
                slv->ShowMessage ( vRetMsg );
            }


            tmpStrg = tmpFst_p->getAFileName();

            if ( tmpStrg.empty() )
            {
                continue;
            }

            bool bRet = slv->getFileContent ( content, tmpStrg, len );

            if ( false == bRet )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg,
                           sizeof ( vRetMsg ),
                           "当前文件[%s]超过了系统配置的长度不加载，跳过",
                           tmpStrg.c_str()
                         );
                slv->ShowMessage ( vRetMsg );

                continue;
            }

            if ( ( ( slv->getDebug() ) & 0x02 ) == 2 )
            {
                snprintf ( vRetMsg,
                           sizeof ( vRetMsg ),
                           "%s|%s|%d:  end getFileContent len=[%d]\n",
                           __FILE__, __func__, __LINE__, len );
                slv->ShowMessage ( vRetMsg );
            }


            //从临时缓冲区copy到上传缓冲区
            for ( int i = 0; i < slv->getIpNum(); i++ )
            {
                slv->m_Buff102[i].setType ( tmpFst_p->getType() );
                slv->m_Buff102[i].getBuffFun ( content, tmpStrg, len );

            }

            tmpStrg.clear();

        }
        else//上传数据缓冲区有在用的
        {
            //间隔1秒再查是否有缓冲区在用
            sleep ( 1 );

            //防止所有没有socket都不能用，buffer加载数据后countTime++超的情况
            if ( countTime[countSub] > 100 )
            {
                countTime[countSub] = 100;
            }

            //清理掉僵死不能用的buffer
            if ( countTime[countSub] > 10 )
            {

                for ( int i = 0; i < slv->getIpNum(); i++ )
                {
                    //有需要加载的buffer
                    if ( slv->m_Buff102[i].isUsing() == false
                         && slv->m_Sock[i].is_using() == true
                       )
                    {
                        slv->noSockFlag[countSub] = 1;
                        slv->m_Buff102[countSub].clearBuffFun();
                        countTime[countSub] = 0;
                        break;
                    }
                }
            }
        }

    }

    slv->doingFlag = 0;

    if ( ( ( slv->getDebug() ) & 0x02 ) == 2 )
    {
        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "%s|%s|%d:file list load to buffer finished,return\n\n\n\n",
                   __FILE__, __func__, __LINE__ );
        slv->ShowMessage ( vRetMsg );
    }

    return;

}

//Looper* Slave::getLooper()
//{
//    return mLooper;
//}


void* Slave::ThreadFileList ( void* arg )
{

    Slave* slv = ( Slave* ) arg;

    Looper* me = Looper::getLooper();
    slv->mLooper = me;
    slv->mLooper->incRef();
    slv->mLooper->prepare();
    slv->mLooper->Loop();

    //    if(slv->getLooper()!=NULL)
    //    {
    //        slv->getLooper()->incRef();
    //        slv->getLooper()->prepare();
    //        slv->getLooper()->Loop();
    //    }

    return NULL;

}

void Slave::onAppEvent ( const char* event,
                         IEventManager::EVENT_ACTION action,
                         Json::Value eventAttr )
{
    string tmpStrng;
    int vFileType = 0;
    int findFlag = 0;
    char vRetMsg[256];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    if ( strcmp ( event, STORAGE_EVENT ) == 0 )
    {
        for ( int i = 0; i < mPthNum; i++ )
        {
            if ( strcmp ( eventAttr["Parameter"]["Dir"].asCString(),
                          monPaths[i].c_str()
                        ) == 0
               )
            {
                tmpStrng.append ( eventAttr["Parameter"]["Dir"].asCString() );
                tmpStrng.append ( "/" );
                tmpStrng.append ( eventAttr["Parameter"]["FileName"].asCString() );
                vFileType = monFType[i];
                findFlag = 1;
                break;
            }
        }

        if ( findFlag == 0 )
        {
            return;
        }


    }
    else
    {
        return;
    }

    if ( ( m_debug & 0x02 ) == 2 )
    {
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "%s|%s|%d:  fusk test monitor file tmpStrng.c_str()=[%s]\n",
                   __FILE__, __func__, __LINE__, tmpStrng.c_str() );
        ShowMessage ( vRetMsg );
    }

    add_Old_File ( tmpStrng.c_str(), vFileType );

    return;
}

void Slave::startMon()
{
    if ( cMonFile_p == NULL )
    {
        ShowMessage ( "FileMonitor is NULL error" );
        return;
        //cMonFile_p = new FileMonitor();
    }

    char vRetMsg[256];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    for ( int i = 0; i < mPthNum; i++ )
    {
        if ( ( m_debug & 0x02 ) == 2 )
        {
            snprintf ( vRetMsg, sizeof ( vRetMsg ),
                       "监视器添加监视目录monPaths[%d].c_str()=[%s]\n",
                       i, monPaths[i].c_str()
                     );
            ShowMessage ( vRetMsg );

        }

        cMonFile_p->startMonitor ( monPaths[i].c_str(),
                                   EVENT_MOVE_TO | EVENT_CREATE
                                 );
    }

    //注册事件 STORAGE_EVENT
    IEventManager::Initialize()->attachEventHandler (
    STORAGE_EVENT,
    ( IEventHandler* ) this,
    ( IEventManager::HANDLER_FUNC ) &Slave::onAppEvent );

    return;
}

void* Slave::ThreadDealSock ( void* arg )
{
    Slave* sl = ( Slave* ) arg;
    int   idx = -1;
    int   i   = 0;
    unsigned char tmpBuffer[512 * 1024];
    int   ret     = 0;
    int   recvLen = 0;
    string strMsg;
    char vRetMsg[128];

    memset ( tmpBuffer, 0, sizeof ( tmpBuffer ) );
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );


    sleep ( 5 );


    for ( i = 0; i < MC_PRLUP_MAX; i++ )
    {
        if ( sl->m_Sock[i].tid == pthread_self() )
        {
            idx = i;
            break;
        }
    }

    if ( idx == -1 )
    {
        sl->ShowMessage ( "tid没找到，线程退出" );
        return NULL;
    }

    snprintf ( vRetMsg,
               sizeof ( vRetMsg ),
               "开始处理接到的连接! %s  第%d路",
               sl->m_Sock[idx].ip, idx + 1
             );
    strMsg = vRetMsg;
    sl->ShowMessage ( strMsg );

    int sock = sl->m_Sock[idx].sockC;


    while ( true )
    {
        memset ( tmpBuffer, 0, sizeof ( tmpBuffer ) );
        recvLen = sl->recvBuffFun ( sock,
                                    ( char* ) tmpBuffer,
                                    sizeof ( tmpBuffer )
                                  );

        if ( recvLen < 0 ) //接收数据失败
        {
            memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
            snprintf ( vRetMsg,
                       sizeof ( vRetMsg ),
                       "从主动站接收第%d路[%s]数据失败!",
                       idx + 1, sl->m_Sock[idx].ip
                     );
            sl->ShowMessage ( vRetMsg );
            break;
        }
        else//接收数据成功
        {

            sl->iCommCnt[idx]++;


            ret = sl->dealBuff ( sock,
                                 tmpBuffer,
                                 recvLen,
                                 idx
                               );

            if ( ret == -1 )
            {
                memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
                snprintf ( vRetMsg,
                           sizeof ( vRetMsg ),
                           "处理第%d路[%s]数据出现错误!",
                           idx + 1, sl->m_Sock[idx].ip
                         );
                sl->ShowMessage ( vRetMsg );
                break;
            }
        }
    }

    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    snprintf ( vRetMsg,
               sizeof ( vRetMsg ),
               "读socket第%d路[%s]数据,并处理数据失败! 关闭连接!!",
               idx + 1, sl->m_Sock[idx].ip
             );
    sl->ShowMessage ( vRetMsg );

    sl->m_Sock[idx].close_Sock();

    return NULL;

}


void* Slave::ThreadAcceptSock ( void* arg )
{
    Slave* sl = ( Slave* ) arg;

    pthread_t tid = 0;
    int ret = 0;
    char tmpStr[256];
    memset ( tmpStr, 0, sizeof ( tmpStr ) );

CREATESOCK:
    //创建套接字
    int slisten = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( slisten == -1 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "socket error[%s] ! 创建套接字失败",
                   strerror ( errno ) );
        sl->ShowMessage ( tmpStr );
        sleep ( 20 );
        goto CREATESOCK;
    }
    else if ( ( sl->m_debug & 0x02 ) == 2 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "创建套接字成功" );
        sl->ShowMessage ( tmpStr );
    }

REUSEADDR:
    unsigned int value = 1;
    ret = setsockopt ( slisten,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &value,
                       sizeof ( value )
                     );

    if ( ret != 0 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "setsockopt SO_REUSEADDR error[%s] !",
                   strerror ( errno ) );
        sl->ShowMessage ( tmpStr );
        sleep ( 20 );
        goto REUSEADDR;
    }

BIND:
    //绑定IP和端口
    struct sockaddr_in sin;
    memset ( &sin, 0, sizeof ( sin ) );
    sin.sin_family = AF_INET;
    sin.sin_port = htons ( sl->getPort() );
    sin.sin_addr.s_addr = INADDR_ANY;

    ret = bind ( slisten,
                 ( struct sockaddr* ) &sin,
                 sizeof ( struct sockaddr )
               );

    if ( ret != 0 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "bind error[%s] !绑定ip和端口失败,"
                   "请查看是否有其他程序占用了本机的[%d]端口!\n",
                   strerror ( errno ), sl->getPort() );
        sl->ShowMessage ( tmpStr );
        sl->ShowMessage ( "绑定端口失败!" );
        sleep ( 20 );
        goto BIND;
    }
    else if ( ( sl->m_debug & 0x02 ) == 2 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "绑定s_addr= INADDR_ANY和端口[%d]成功\n"
                   , sl->m_port );
        sl->ShowMessage ( tmpStr );
    }

LISTEN:
    //开始监听
    ret = listen ( slisten, 5 );

    if ( ret < 0 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "listen error [%s]! 监听失败\n",
                   strerror ( errno ) );
        sl->ShowMessage ( tmpStr );
        sleep ( 20 );
        goto LISTEN;
    }
    else if ( ( sl->m_debug & 0x02 ) == 2 )
    {
        memset ( tmpStr, 0, sizeof ( tmpStr ) );
        snprintf ( tmpStr, sizeof ( tmpStr ),
                   "监听成功\n" );
        sl->ShowMessage ( tmpStr );
    }

    int sClient;
    struct sockaddr_in remoteAddr;
    socklen_t nAddrlen = sizeof ( remoteAddr );
    int idx = 0;

    string tmpIps[MC_PRLUP_MAX];
    int    tmpIpNum = 0;
    sl->getIps ( tmpIps );
    tmpIpNum = sl->getIpNum();
    int     findFlag = 0;

    while ( true )
    {
        //printf("%s|%s|%d:  while true[%s]\n",__FILE__,__func__,__LINE__, "111111");
        //sl->ShowMessage("等待连接...");
        if ( ( sl->m_debug & 0x02 ) == 2 )
        {
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "等待连接...\n" );
            sl->ShowMessage ( tmpStr );
        }

        sClient = accept ( slisten,
                           ( struct sockaddr* ) &remoteAddr,
                           &nAddrlen
                         );

        if ( sClient < 0 )
        {
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "accept error [%s]!",
                       strerror ( errno ) );
            sl->ShowMessage ( tmpStr );
            continue;
        }

        if ( ( sl->m_debug & 0x02 ) == 2 )
        {
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "接收到一个连接\n" );
            sl->ShowMessage ( tmpStr );
        }

        findFlag = 0;

        for ( int i = 0; i < tmpIpNum; i++ )
        {
            if ( strcmp ( inet_ntoa ( remoteAddr.sin_addr ),
                          tmpIps[i].c_str()
                        ) == 0
               )
            {
                idx = i;
                memset ( tmpStr, 0, sizeof ( tmpStr ) );
                snprintf ( tmpStr, sizeof ( tmpStr ),
                           "接收到第%d路连接  端口号=%d IP=%s",
                           idx + 1, remoteAddr.sin_port,
                           inet_ntoa ( remoteAddr.sin_addr ) );
                sl->ShowMessage ( tmpStr );

                findFlag = 1;
                break;
            }
        }

        if ( findFlag == 0 )
        {
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "接收到未知连接  端口号=%d IP=%s",
                       remoteAddr.sin_port,
                       inet_ntoa ( remoteAddr.sin_addr ) );
            sl->ShowMessage ( tmpStr );
            continue;
        }


        //判断是否有已经接受的连接如果有关闭连接处理
        if ( sl->m_Sock[idx].is_using() == true )
        {
            sl->m_Sock[idx].close_Sock();
        }

    SOSNDTIMEO://设置发送超时
        struct timeval timeoutSend;
        memset ( &timeoutSend, 0, sizeof ( struct timeval ) );
        timeoutSend.tv_sec = sl->getSockSTimeout();
        ret = setsockopt ( sClient,
                           SOL_SOCKET,
                           SO_SNDTIMEO,
                           ( char* ) &timeoutSend,
                           sizeof ( struct timeval )
                         );

        if ( ret != 0 )
        {
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "setsockopt SO_SNDTIMEO error[%s] !",
                       strerror ( errno ) );
            sl->ShowMessage ( tmpStr );
            sleep ( 20 );
            goto SOSNDTIMEO;
        }

    SORCVTIMEO://设置接收超时
        struct timeval timeoutRcv;
        memset ( &timeoutRcv, 0, sizeof ( struct timeval ) );
        timeoutRcv.tv_sec = sl->getSockRTimeout();
        ret = setsockopt ( sClient,
                           SOL_SOCKET,
                           SO_RCVTIMEO,
                           ( char* ) &timeoutRcv,
                           sizeof ( struct timeval )
                         );

        if ( ret != 0 )
        {
            memset ( tmpStr, 0, sizeof ( tmpStr ) );
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "setsockopt SO_RCVTIMEO error[%s] !",
                       strerror ( errno ) );
            sl->ShowMessage ( tmpStr );
            sleep ( 20 );
            goto SORCVTIMEO;
        }

        ret = pthread_create ( &tid,
                               NULL,
                               ThreadDealSock,
                               ( void* ) sl
                             );

        if ( 0 != ret )
        {
            snprintf ( tmpStr, sizeof ( tmpStr ),
                       "CreateThread() ThreadDealSock error[%s]",
                       strerror ( ret )
                     );
            sl->ShowMessage ( tmpStr );
            return NULL;
        }

        sl->m_Sock[idx].set_Sock ( sClient,
                                   inet_ntoa ( remoteAddr.sin_addr ),
                                   tid
                                 );

    }

    return NULL;

}



