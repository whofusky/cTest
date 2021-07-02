
#ifndef _SLAVE_H_
#define _SLAVE_H_

#include "102buff.h"
#include "EventManager/IEventManager.h"
#include "FileMonitor/FileMonitor.h"


//定义xml配置文件中元素节点名称
#define MC_ROOT                  "root"              //根节点
#define MC_OPERPARA              "operPara"          //程序运行时的有关参数配置
#define MC_UPRETLIST             "upRetList"         //是否需要将上传文件成功失败信息放入up_stat_list以便应用取文件上传状态:1,需要；其他不需要
#define MC_SOCKWRTIMEOUT         "sockWRTimeOut"     //socket 读写超时 单位秒
#define MC_SOCKSNDTIMEOUT        "sockSndTimeOut"    //socket send超时 单位秒
#define MC_SOCKRCVTIMEOUT        "sockRcvTimeOut"    //socket recv超时 单位秒
#define MC_DEBUG                 "debug"             //调试模式开关:1 调试，0 非调试
#define MC_FILENAMELEN           "fileNameLen"       //协议文件长度
#define MC_MAXTRY                "maxTry"            //网络传输错误重试上限
#define MC_HOUREBEGINE           "houreBegine"       //段上时间传范围开始
#define MC_HOUREEND              "houreEnd"          //段上时间传范围结束
#define MC_LISTENPORT            "listenPort"        //本地监听端口
#define MC_MASTERIPS             "masterIPS"         //主站ip地址配置
#define MC_IPNUM                 "ipNum"             //is地址数量
#define MC_IP                    "ip"                //ip
#define MC_MONITORDIR            "monitorDir"        //上传文件目录监视节点
#define MC_PATHNUM               "pathNum"           //监视目录的数量
#define MC_DIRPATH               "dirPath"           //监视目录的路径
#define MC_FILETYPE              "fileType"          //文件类型
#define MC_SLAVE                 "slave"             //协议配置根节点
#define MC_FIXEDBEGIN            "FixedBegin"        //固定长度帧开始
#define MC_VARBLEBEGIN           "VarbleBegin"       //可变长度帧开始
#define MC_FRAMEEND              "FrameEnd"          //结束
#define MC_VSQ                   "VSQ"               //可变结构限制词
#define MC_CONTROLFIELD          "controlField"      //控制域
#define MC_DOWNLINK              "downLink"          //下行数据
#define MC_FIXED                 "fixed"             //固定帧
#define MC_VARIABLE              "variable"          //变长帧
#define MC_XX_YFFW               "xx_yffw"           //远方复位帧
#define MC_XX_QQLLZT             "xx_qqllzt"         //请求链路状态01001001
#define MC_XX_QQ1SJ_1            "xx_qq1sj_1"        //请求1级用户数据01111010
#define MC_XX_QQ1SJ_0            "xx_qq1sj_0"        //请求1级用户数据01011010
#define MC_XX_QQ2SJ_1            "xx_qq2sj_1"        //请求2级用户数据01111011
#define MC_XX_QQ2SJ_0            "xx_qq2sj_0"        //请求2级用户数据01011011
#define MC_XX_YHSJ_0             "xx_yhsj_0"         //下行用户数据0101 0011暂时不做处理
#define MC_XX_YHSJ_1             "xx_yhsj_1"         //下行用户数据0111 0011暂时不做处理
#define MC_UPLINK                "upLink"            //上行数据
#define MC_SX_KDRK               "sx_kdrk"           //肯定认可00000000
#define MC_SX_FDRK               "sx_fdrk"           //否定认可00000001
#define MC_SX_LL                 "sx_ll"             //链路状态或要求的访问  00001011
#define MC_SX_WQQSJ_Y1SJ         "sx_wqqsj_y1sj"     //有1级用户数据|后续可以接受数据|无请求数据 00101001
#define MC_SX_YHSJ               "sx_yhsj"           //被控站有1级数据|后续可以接受数据|用户数据  00101000
#define MC_SX_WQQSJ              "sx_wqqsj"          //无请求数据|后续可以接受数据|无请求数据 00001001
#define MC_SX_W1SJ               "sx_w1sj"           //无1级数据了后面
#define MC_SX_Y1SJ               "sx_y1sj"           //后面有一级数据
#define MC_TYPEID                "typeIdentification"//有关于应用报文类型
#define MC_XX_WJCSJS_TYPE        "xx_wjcsjs_type"    //主站表示文件传输结束
#define MC_XX_WJCSCF_TYPE        "xx_wjcscf_type"    //主站通知文件传输重复
#define MC_XX_WJCDCC_TYPE        "xx_wjcdcc_type"    //文件长度超出规定最大长度
#define MC_XX_WJMGSCW_TYPE       "xx_wjmgscw_type"   //文件名格式错误
#define MC_XX_WJDZCC_TYPE        "xx_wjdzcc_type"    //文件单帧过长
#define MC_SX_WJCSJS_TYPE        "sx_wjcsjs_type"    //子站确认文件传输结束
#define MC_SX_WJCSCF_TYPE        "sx_wjcscf_type"    //子站确认文件传输重复
#define MC_SX_WJCDCC_TYPE        "sx_wjcdcc_type"    //子站确认文件长度超出规定最大长度
#define MC_SX_WJMGSCW_TYPE       "sx_wjmgscw_type"   //子站确认文件名格式错误
#define MC_SX_WJDZCC_TYPE        "sx_wjdzcc_type"    //文件单帧过长
#define MC_COT                   "COT"               //传送原因
#define MC_XX_WJJS_CSYY          "xx_wjjs_csyy"      //主站表示文件接收结束
#define MC_XX_WJCSCF_CSYY        "xx_wjcscf_csyy"    //主站表示文件重复传输
#define MC_XX_WJCDCC_CSYY        "xx_wjcdcc_csyy"    //主站表示文件长度过长
#define MC_XX_WJMGSCW_CSYY       "xx_wjmgscw_csyy"   //主站表示文件文件名格式错误
#define MC_XX_SDXFSJ_CSYY        "xx_sdxfsj_csyy"    //主站表示文件文件名格式错误
#define MC_SX_WJZH_CSYY          "sx_wjzh_csyy"      //子站表示本帧为文件的最后一帧，文件传输结束
#define MC_SX_WJFZH_CSYY         "sx_wjfzh_csyy"     //子站表示本帧不是文件的最后一帧，文件传输未结束
#define MC_SX_WJCDZQ_CSYY        "sx_wjcdzq_csyy"    //子站确认主站接收的文件长度和子站发送的文件长度相同，表示确认文件传送成功，并处理此文件
#define MC_SX_WJCDCW_CSYY        "sx_wjcdcw_csyy"    //子站认为主站接收的文件长度和子站发送的文件长度不同，并准备重新传输该文件
#define MC_SX_WJCSCF_CSYY        "sx_wjcscf_csyy"    //子站确认文件重复，并作其他处理
#define MC_SX_WJCDCC_CSYY        "sx_wjcdcc_csyy"    //子站确认文件长度过长，并作其他处理
#define MC_SX_WJMGSCW_CSYY       "sx_wjmgscw_csyy"   //子站确认文件文件名格式错误，并作其他处理
#define MC_CMNADRSASDU           "cmnAdrsASDU"       //用户服务单元公共地址
#define MC_SX_SBDZ               "sx_sbdz"           //两个8位位组，设备地址
#define MC_SX_JLDZ               "sx_jldz"           //记录地址，本协议默认为0

//有关长度的定义
#define MAX_PACK_LEN        255 //一个数据包的最大长度
#define MC_PRLUP_MAX        4   //102并行发送的最大通道数量
#define MC_FILEMON_MAX      20  //文件监视目录最大个数

class FileList
{
    private:
        CMutex m_mtx_lister;  //锁变量

        vector<string> m_FileLister;
        int m_Type;

    public:
        FileList();
        ~FileList();

        void setType ( const int& v_Type );
        int getType();
        //取得m_FileLister.size()
        int getSize();
        //取得 m_FileLister.back()
        string getAFileName();

        //只有m_FileLister.back() 没有m_FileLister.pop_back()
        string backFileName();
        //只有m_FileLister.pop_back() 没有m_FileLister.back()
        void popBackFileName();

        //m_FileLister.push_back(v_FileName);
        void addAFileName2List ( const string& v_FileName );


};


//上传状态描述
class CSBCG_State
{
    public:
        CSBCG_State();
        ~CSBCG_State();

        int isbzl;              // 上报种类 例子 CDQ
        string strsbnr;         //上报年日常 例子 超短期文件
        string strsbxzwjm;      //上报下载文件名称
        string strsbzt;         //上报 是否成功
        string strsbbz;         //上报备注 打印使用
        string strFaileCause;   //刘富强增加 文件传输失败原因

};

//上传状态vector
class SB_State_List
{
    private:
        CMutex m_Mtx_Stat;  //锁变量

        vector<CSBCG_State>  m_stat_list;

    public:
        SB_State_List();
        ~SB_State_List();

    public:
        //m_stat_list.back();
        CSBCG_State getData();
        //m_stat_list.size()
        int getSize();
        //m_stat_list.push_back(v_ss);
        void insert2list ( const CSBCG_State& v_ss );

};



//accept socket 描述
class  Sock_Client
{
    private:
        CMutex m_mtx_sock;
    public:
        int       sockC;
        char      ip[20];
        pthread_t tid;

    public:
        Sock_Client();
        ~Sock_Client();

        void close_Sock();
        void set_Sock ( const int& v_sock,
                        const char* v_ip,
                        const pthread_t& id
                      );

        bool is_using();

};

//定义函数输出信息 函数指针类型
typedef void ( *PFShowMessage ) ( string strMsg );


/******************************************************************************
*类名:102子站协议处理类
*
*功能概述:
*   根据102上传协议与主站通信并上传指定文件到主站
*
*说明:
*   1、使用之前需要准备好相应的xml配置文件 和 PFShowMessage类型的日志输出函数
*   2、调用SetShowMessgaeFun方法绑定日志输出函数
*   3、调用initeVar方法加载xml的配置
*   4、调用inIteSlave启动线程
*   5、向协议添加要处理的文件有两种方法
*          1)、通过类方法add_Old_File添加
*          2)、通过xml配置文件配置监视文件目录(程序监视目录是否有新产生的文件，
               如果有则自动添加文件名和文件类型到处理链表)
*备注:
*   1、如果在配置文件里的调试开关打开则程序自动在运行目录下新建立log目录,
*      把接收和发送的帧数据输出到日志文件
*   2、如果在配置文件里的调试开关打开则PFShowMessage会输出很多调试信息
*
*编译依赖的静态库文件:
*       -l102slave -ltinyxml2 -ljson_linux-gcc-4.4.7_libmt
*       -lEventManager -lFileMonitor -lInfra -lBase  -lpthread
*
*******************************************************************************/
class Slave: public IEventManager::IEventHandler
{
    public:
        Slave();
        ~Slave();

    private:
        Time_str m_datetime_stat; //时间变量

        /***************************************
                           上传状态
        ****************************************/
    public:
        SB_State_List up_stat_list[MC_PRLUP_MAX];




        /***************************************
                           线程处理
        ****************************************/
    private:
        pthread_t tidTas;//sock server建立与链接处理
        pthread_t tidTfl;//文件list处理线程
        static void* ThreadFileList ( void* arg ); //文件list处理线程
        static void* ThreadDealSock ( void* arg ); //处理连接上的资源
        static void* ThreadAcceptSock ( void* arg ); //sock server建立与链接处理函数
        //static void* ThreadFileMon(void* arg);//文件监视处理线程
    public:
        //启动协议处理线程
        int inIteSlave();



        /****************************************
                         日志输出
        *****************************************/
    private:
        PFShowMessage m_PFShowMsg; //错误信息输出函数指针变量
        //信息显示函数:调用m_PFShowMsg 输出信息
        void ShowMessage ( const string& strMsg );
        /*调试开关打开时,报文打印 日志输出文件到程序运行目
            录的log下(没有log目录时程序自动建立)
         */
        void write_debug_msg ( const unsigned char* prtBuffer_p,
                               const int& buffLen,
                               const char* errNote,
                               const int& prtType = 0
                             );
    public:
        //给 信息显示函数指针m_PFShowMsg 赋值
        void SetShowMessgaeFun ( PFShowMessage PFunShowMessage );



        /***************************************************
                         内部参数
        ****************************************************/
    private:
        int m_sock_timeout; //网络读写超时
        int m_sock_stimeout;//socket send超时 单位秒
        int m_sock_rtimeout;//socket recv超时 单位秒
        int m_debug;        //调试模式开关
        int m_filenamelen;  //协议文件长度
        int m_maxtry;       //网络传输错误重试上限
        int m_fss_houre_ks; //段上时间传范围开始
        int m_fss_houre_js; //段上时间传范围结束
        int m_port;         //本地监听端口
        string m_ips[MC_PRLUP_MAX];//主站的ip地址组
        int    usedIpNum;  //实际使用了的ip数量
        int    upRetList;  //是否需要将上传文件成功失败信息放入up_stat_list以便应用取文件上传状态:1,需要；其他不需要
    public:
        int getSockTimeout(); //取网络读写超时
        int getSockSTimeout();//取socket send超时
        int getSockRTimeout();//socket recv超时
        int getDebug();       //取调试模式开关
        int getFileNameLen(); //取协议文件长度
        int getMaxTry();      //取网络传输错误重试上限
        int getFssHBegine();  //取段上时间传范围开始
        int getFssHEnd();     //取段上时间传范围结束
        int getPort();        //取本地监听端口
        void getIps ( string* ips_p ); //取得多个ip的首地址
        int getIpNum();       //取得ip数量ipNum

        /****************************************************************************************
                                xml 配置相关
         ***************************************************************************************/
    private:
        //begine 协议相关的帧元素值，以下变量需要从xml文件读取配置的值
        int FixedBegin;       //固定长度帧开始
        int VarbleBegin;      //可变长度帧开始
        int FrameEnd;         //结束
        int SX_KBXD_XDC;      //可变结构限制词
        int XX_YFFW;          //远方复位帧
        int XX_QQLLZT;        //请求链路状态01001001
        int XX_QQ1SJ_1;       //请求1级用户数据01111010
        int XX_QQ1SJ_0;       //请求1级用户数据01011010
        int XX_QQ2SJ_1;       //请求2级用户数据01111011
        int XX_QQ2SJ_0;       //请求2级用户数据01011011
        int XX_YHSJ_0;        //下行用户数据0101 0011暂时不做处理
        int XX_YHSJ_1;        //下行用户数据0111 0011暂时不做处理
        int SX_KDRK;          //肯定认可00000000
        int SX_FDRK;          //否定认可00000001
        int SX_LL;            //链路状态或要求的访问  00001011
        int SX_WQQSJ_Y1SJ;    //有1级用户数据|后续可以接受数据|无请求数据 00101001
        int SX_YHSJ;          //被控站有1级数据|后续可以接受数据|用户数据  00101000
        int SX_WQQSJ;         //无请求数据|后续可以接受数据|无请求数据 00001001
        int SX_W1SJ;          //无1级数据了后面
        int SX_Y1SJ;          //后面有一级数据
        int XX_WJCSJS_TYPE;   //主站表示文件传输结束
        int XX_WJCSCF_TYPE;   //主站通知文件传输重复
        int XX_WJCDCC_TYPE;   //文件长度超出规定最大长度
        int XX_WJMGSCW_TYPE;  //文件名格式错误
        int XX_WJDZCC_TYPE;   //文件单帧过长
        int SX_WJCSJS_TYPE;   //子站确认文件传输结束
        int SX_WJCSCF_TYPE;   //子站确认文件传输重复
        int SX_WJCDCC_TYPE;   //子站确认文件长度超出规定最大长度
        int SX_WJMGSCW_TYPE;  //子站确认文件名格式错误
        int SX_WJDZCC_TYPE;   //文件单帧过长
        int XX_WJJS_CSYY;     //主站表示文件接收结束
        int XX_WJCSCF_CSYY;   //主站表示文件重复传输
        int XX_WJCDCC_CSYY;   //主站表示文件长度过长
        int XX_WJMGSCW_CSYY;  //主站表示文件文件名格式错误
        int XX_SDXFSJ_CSYY;   //主站表示文件文件名格式错误
        int SX_WJZH_CSYY;     //子站表示本帧为文件的最后一帧，文件传输结束
        int SX_WJFZH_CSYY;    //子站表示本帧不是文件的最后一帧，文件传输未结束
        int SX_WJCDZQ_CSYY;   //子站确认主站接收的文件长度和子站发送的文件长度相同，表示确认文件传送成功，并处理此文件
        int SX_WJCDCW_CSYY;   //子站认为主站接收的文件长度和子站发送的文件长度不同，并准备重新传输该文件
        int SX_WJCSCF_CSYY;   //子站确认文件重复，并作其他处理
        int SX_WJCDCC_CSYY;   //子站确认文件长度过长，并作其他处理
        int SX_WJMGSCW_CSYY;  //子站确认文件文件名格式错误，并作其他处理
        int SX_SBDZ;          //两个8位位组，设备地址
        int SX_JLDZ;          //记录地址，本协议默认为0
        //end 协议相关的帧元素值，以上变量需要从xml文件读取配置的值

    private:
        //调试时打印xml配置的值
        void printf_xmlVal();

        //判断xml中配置的值是否为16进制并转换成10进制的值赋于dstVal
        void coverXmlVal ( const char* xmlVal_p, //需要转换的地址
                           int& dstVal           //转换后的值
                         );

    public:
        /*加载strPath xml文件配置的值到协议相关的帧格式值变量
        (正常模式不用显式调用此接口，在initeVar接口中已经调用)
        */
        int loadXmlval ( const string& strPath );

        //初始化内部参数
        int initeVar (
        const string& strXmlPath  //协议元素对应的xml配置文件(全路径)
        );

        /****************************************************************************************
                                    有关于文件的操作
        ***************************************************************************************/
    private:

        CMutex  flstMutex;                  //文件列表锁
        vector<FileList*> m_FileList_Lister;//不同类型的文件列表队列
        CMutex ftypeMutex;                  //文件类型列表锁
        //102子站需要处理的文件类型(也是102帧格式中的第8个字节:类型标识)
        vector<int> m_type_list;

    private:
        void add_File_List (  FileList* v_FileList_p );

        //检查添加文件类型的时候是否与typeIdentification应用报文类型冲突
        int chkFType ( const int& iType );

        //加载文件file_name内容到content,长度为len
        bool getFileContent ( char* content,        //将要加载内容的缓冲区
                              const string& file_name,//文件名(全路径)
                              int& len                //函数执行之前值为content容量,执行之后为加载内容的长度
                            );
    public:
        //m_FileList_Lister.size()
        int getFLLsize();
        //m_type_list.size()
        int getFTLsize();

        FileList*  getFLLback();
        void       pop_backFll();
        //m_type_list.push_back(v_type)
        //添加102协议使用的文件类型
        void addType ( int v_type );
        /*向m_FileList_Lister 添加 v_FileName
          向m_type_list 添加v_Type(如果v_Type在链表中不存在)
        */
        int add_Old_File ( const string& v_FileName,
                           const int& v_Type
                         );

        /****************************************************************************************
                                            有关于协议动作
        ***************************************************************************************/
    private:
        //组装固定长度包
        int makeFixedFrameFun ( unsigned char* v_Buff, //组装的缓冲区指针
                                unsigned char v_Mark,  //控制域
                                unsigned char v_2,     //低地址
                                unsigned char v_3      //高地址
                              );
        /*从m_Buff102[idx]中copy数据到buff中并修改m_Buff102[idx]中当前位置
          返回值: buff中数据的长度
        */
        int assignment ( unsigned char* buff, //将要赋值的缓冲区
                         const int& bufflen,   //缓冲区长度
                         bool& isend,          //输出值:true,数据全部装载这是最后一帧；false，不是最后一帧
                         const int& idx        //数据源缓冲区m_Buff102[idx]的索引
                       );
        //组装变量长度包
        int makeVaribleFrameFun ( unsigned char* v_Buff,         //组装的缓冲区指针
                                  const int& v_BuffLen,            //组装的缓冲区长度
                                  const unsigned char* v_RecvBuff, //接收到的数据指针
                                  const int& v_RecvBuffLen,        //接收到的数据长度
                                  const int& v_PackType,           //组包的类型
                                  const int& idx                   //数据源缓冲区m_Buff102[idx]的索引
                                );
        //做和效验数据
        int efficacyData ( const unsigned char* v_Buff, //校验的缓冲区指针
                           const int& v_BuffLen         //缓冲区长度
                         );
        /*有关于协议动作end*/

        /****************************************************************************************
                                      socket 相关处理
        ***************************************************************************************/
    private:
        CMutex recvMutex;       //socket收发锁变量
    private:
        //发送函数
        int sendBuffFun ( const int& v_Sock,  //sock资源
                          const char* v_Buff,   //发送缓冲区指针
                          const int& v_BuffLen  //发送缓冲区的长度
                        );
        //sock资源监测函数
        int selectFun ( const int&  v_Sock, //sock资源
                        const int&  v_Sec, //监测的最长时间(单位秒)
                        const char& v_Mod  //监测模式 r 或 w
                      );
    public:
        //加载文件内容后的 缓冲区
        Buff_102 m_Buff102[MC_PRLUP_MAX];

        //accept socket
        Sock_Client m_Sock[MC_PRLUP_MAX];

        //记录m_Sock[idx]召唤次数
        int iCommCnt[MC_PRLUP_MAX];
    public:

        //处理接收到的字符流
        int dealBuff ( const int& v_Sock,
                       unsigned char* v_Buff,
                       const int& v_BuffLen,
                       const int& idx
                     );

        //接收函数
        int recvBuffFun ( const int& v_Sock, //sock资源
                          char* v_Buff,        //接收缓冲区指针
                          const int& v_BuffLen //接收缓冲区的长度
                        );


        /****************************************************************************************
                              添加文件列表时消息处理模块
        ***************************************************************************************/
    private:
        class FileListHandler: public Handler
        {
            public:
                FileListHandler ( Slave* slave_p );
                ~FileListHandler();

                typedef enum file_what_msg
                {
                    PARSE_ADD_FILE,
                    PARSE_MSG_NULL
                } FILE_WHAT_MSG;

                void handleMessage ( Message msg );

            private:
                Slave* mHslave_p;
        };

        Handler*      mHandler;
        Looper*       mLooper;

    public:
        //Looper*       getLooper();
        int           doingFlag;//是否有进程正在处理文件列表:1 表示有，0 表示无
        int           noSockFlag[MC_PRLUP_MAX];


        /****************************************************************************************
                              添加文件目录监视模块
        ***************************************************************************************/
    private:
        FileMonitor*  cMonFile_p;              //文件监视器
        string        monPaths[MC_FILEMON_MAX];//文件目录
        int           monFType[MC_FILEMON_MAX];//文件类型
        int           mPthNum;                 //目录数量

    private:
        void startMon();//启动文件监视器

        /*来自IEventManager::IEventHandler的虚函数
        IEventManager取出消息后调用此函数实现具体逻辑
        */
        void onAppEvent ( const char* event,
                          IEventManager::EVENT_ACTION action,
                          Json::Value eventAttr
                        );



};


#endif//_SLAVE_H_

