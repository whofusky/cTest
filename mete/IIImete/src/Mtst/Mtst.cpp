
#include "funcLib.h"
#include "pubExtern.h"
#include "json/json.h"
//#include <stdio.h>
//#include <unistd.h>
#include<string>
//#include <unistd.h>
#include <sys/stat.h>



using namespace std;
#define MC_PRLUP_MAX 5

int end = 0;
void signal_handler ( int sig )
{
    char buf[100] = {};
    snprintf ( buf, sizeof ( buf ), "catch th signal:[%d]\n", sig );
    write ( STDERR_FILENO, buf, strlen ( buf ) );
    end = 1;
}

//const int usedIpNum = 4;
//string m_ips[usedIpNum] = {"111111","222222","33333","4444"};
//void getIps(string* ips_p)
//{
//    //ipNum = usedIpNum;
//    for(int i=0;i<usedIpNum && usedIpNum<=MC_PRLUP_MAX;i++)
//    {
//        ips_p[i] = m_ips[i];
//        //ips_p = m_ips;
//    }
//    return;
//}

void coverXmlVal ( const char* xmlVal_p, int& dstVal )
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

#define  BUFF_STATE_EMPTY 2  //Buff102的缓存区的状态 空闲可用
#define  BUFF_STATE_USING 1  //Buff102的缓存区的状态 占用不可用

#define  MAXBUFFLEN       1024*1024 //Buff102的缓存区的长度

class Buff_102
{
    public:
        Buff_102();
        ~Buff_102();
    private:

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
        int getBuffFun ( const char* content, //需要复制的内容
                         const char* file_name, //内容来自的文件名
                         const int& len         //content是长度
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
                           const char* file_name,
                           const int& len
                         )
{
    //char  tmp_err[200];
    if ( isUsing() == true )
    {
        return -1;
    }


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


    return ret;

}

int Buff_102::clearBuffFun()
{

    //memset(m_Buffer,0,sizeof(m_Buffer));
    memset ( m_Buffer, 0, sizeof ( unsigned char ) *MAXBUFFLEN );
    m_Stat = BUFF_STATE_EMPTY;
    m_BufferLen = 0;
    m_NextFrame = 0;
    m_Filename = "";
    m_DataType = 0;
    m_TryNumber = 0;


    return 0;
}

int Buff_102::resetBuffFun()
{
    m_NextFrame = 0;
    return 0;
}
bool Buff_102::isUsing()
{
    if ( m_Stat == BUFF_STATE_EMPTY )
    {
        return false;
    }
    else
    {
        return true;
    }
}

unsigned char* Buff_102::getBufferP()
{
    return m_Buffer;
}

void Buff_102::setNextFrame ( const int& next )
{
    m_NextFrame = next;

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
    m_DataType = v_DataType;

    return;
}


void Buff_102::setTryNumber ( const int& tryNumber )
{
    m_TryNumber = tryNumber;

    return;
}

int& Buff_102::getTryNumber()
{
    return m_TryNumber;
}

void Buff_102::setyzcd ( const int& len )
{
    m_yzcd = len;
    return;
}

int Buff_102::getyzcd()
{
    return m_yzcd;
}


int main ( int argc, const char** argv )
{
    int vRet = 0;
    unsigned int a=6;
    int b=-20;
    int c=-2;
    (a+b)>c?printf("%d>%d\n",(a+b),c):printf("%d<%d\n",(a+b),c);
    unsigned int u_a_plus_b=(unsigned int)(a+b);
    unsigned int u_c=(unsigned int)c;
    printf("u_a_plus_b=[%u],u_c=[%u]\n",u_a_plus_b,u_c);
    return 0;
    char oErrMsg_p[MC_ERRMSG_MAX] = {};
    char tmpSS[] = "hahahahah"
                   "ttttttttttttttttttttttt"
                   "hhhhhhhhhhhhhhhhhhhhhhh";
    //    t_printf("%s|%s|%d:  tmpSS[%s]\n",__FILE__,__func__,__LINE__, tmpSS);

    //
    //    char aat[12][13];
    //    t_printf("%s|%s|%d:------sizeof(aat)=[%d],[%d]\n",
    //        __FILE__,__func__,__LINE__, sizeof(aat),sizeof(aat[0]));

    //    return 0;

    char ttoi1_a[20];
    char ttoi2_a[20];
    memset( ttoi1_a, 0,sizeof(ttoi1_a));
    memset( ttoi2_a, 0,sizeof(ttoi2_a));
    strcpy( ttoi1_a,"-1");
    strcpy( ttoi2_a,"2000");
    int tta1=atoi(ttoi1_a);
    int tta2=atoi(ttoi2_a);
    int tInt = METE_SUCCS_DOWNADO;
    Json::Value tAttr;
    tAttr["retCode"] = tInt;

    t_printf ( "%s|%s|%d:  ttoi1_a=[%s],ttoi2_a=[%s],tta1=[%d],tta2=[%d],tInt=[%d],tAttr.retCode=[%d]\n",
            __FILE__, __func__, __LINE__, 
            ttoi1_a,ttoi2_a,tta1,tta2 ,tInt , tAttr["retCode"].asInt() );
    return 0;

    char tmpdir[80] = {};
    char tmpdir2[100] = {};
    char tmpregStr[100]= {};
    char tmpregStr1[100]= {};
    strcpy ( tmpregStr, "stat[0-9]{8,8}\\.xml" );
    strcpy ( tmpregStr1, tmpregStr );
    vRet = mDelByRegAndTm("/home/fczx/tmp/0306",tmpregStr,864000,oErrMsg_p,1);
    if ( vRet != 0 )
    {
        t_printf ( "%s|%s|%d:  oErrMsg_p[%s],vRet=[%d]\n", __FILE__, __func__, __LINE__, oErrMsg_p,vRet );
    }
        t_printf ( "%s|%s|%d:  \\\\\\ oErrMsg_p[%s],vRet=[%d],tmpregStr=[%s],tmpregStr1[%s]\n", __FILE__, __func__, __LINE__, oErrMsg_p,vRet , tmpregStr,tmpregStr1 );
    return 0;
    getcwd ( tmpdir, sizeof ( tmpdir ) );
    t_printf ( "%s|%s|%d:  tmpdir[%s]\n", __FILE__, __func__, __LINE__, tmpdir );


    Buff_102  tmp102;
    tmp102.setTryNumber ( 2 );

    //    t_printf("%s|%s|%d:  TryNumber[%d]\n",__FILE__,__func__,__LINE__, tmp102.getTryNumber());
    //    tmp102.getTryNumber()++;
    //    tmp102.getyzcd()++;
    //    tmp102.getyzcd()++;
    //    t_printf("%s|%s|%d:  getyzcd[%d]\n",__FILE__,__func__,__LINE__, tmp102.getyzcd());
    //    t_printf("%s|%s|%d:  TryNumber[%d]\n",__FILE__,__func__,__LINE__, tmp102.getTryNumber());

    //    char tthstr[120]={};
    //    int ttNum = sizeof(tthstr);
    //    t_printf("%s|%s|%d:  ttNum[%d]\n",__FILE__,__func__,__LINE__, ttNum);

    //    string futStrg="/home/fczx/filedo/down/yes";
    //    t_printf("%s|%s|%d:  1 futStrg[%s]\n",__FILE__,__func__,__LINE__, futStrg.c_str());

    //     futStrg.append("/");
    //    t_printf("%s|%s|%d:  2 futStrg[%s]\n",__FILE__,__func__,__LINE__, futStrg.c_str());

    //    futStrg.append("busilist_0817001.xml");
    //    t_printf("%s|%s|%d:  3 futStrg[%s]\n",__FILE__,__func__,__LINE__, futStrg.c_str());

    //    int tfif=-2;
    //    if(tfif)
    //    {
    //        t_printf("%s|%s|%d:  tfif[%d]\n",__FILE__,__func__,__LINE__, tfif);
    //    }

    //    t_printf("%s|%s|%d:  %s\n",__FILE__,__func__,__LINE__, "finish!!");



    //    static const char* xml = "<doc><element attribute='attribute'/><element attribute='attribute1'/></doc>";
    //            XMLDocument doxxc;
    //            doxxc.Parse( xml );
    //            XMLElement* ele0 = doxxc.FirstChildElement()->FirstChildElement();
    //            XMLElement* ele1 = ele0->NextSiblingElement();

    //            const char* tatt_p= ele1->Attribute("attribute");

    //    t_printf("%s|%s|%d: 3333 tatt_p[%s]\n",__FILE__,__func__,__LINE__, tatt_p);
    //    return 0;

    //    snprintf(tmpdir2,sizeof(tmpdir2),
    //        "%s/log",tmpdir);
    //    if (access(tmpdir2,0)!=0)
    //  {
    //        printf("1111\n");
    //        vRet = mkdir(tmpdir2, 0755);
    //        if(vRet!=0)
    //        {
    //            printf("%s|%s|%d:  error[%s]\n",__FILE__,__func__,__LINE__, strerror(errno));
    //        }
    //  }
    //    snprintf(tmpdir2,sizeof(tmpdir2),
    //        "%s/log/tt",tmpdir);
    //    if (access(tmpdir2,0)!=0)
    //  {
    //        printf("2222\n");
    //        vRet = mkdir(tmpdir2, 0755);
    //        if(vRet!=0)
    //        {
    //            printf("%s|%s|%d:  error[%s]\n",__FILE__,__func__,__LINE__, strerror(errno));
    //        }
    //  }

    // return 0;

    //    string ips_p[usedIpNum];
    //    getIps(ips_p);
    //    t_printf("%s|%s|%d:  ips_p[0][%s]\n",__FILE__,__func__,__LINE__, ips_p[0].c_str());
    //    t_printf("%s|%s|%d:  ips_p[1][%s]\n",__FILE__,__func__,__LINE__, ips_p[1].c_str());
    //    t_printf("%s|%s|%d:  ips_p[2][%s]\n",__FILE__,__func__,__LINE__, ips_p[2].c_str());
    //    t_printf("%s|%s|%d:  ips_p[3][%s]\n",__FILE__,__func__,__LINE__, ips_p[3].c_str());

    //    XMLDocument docttt;

    //    vRet = docttt.LoadFile("/home/fczx/cfg/ftest.xml");
    //    if( vRet!=0 )
    //    {
    //        t_printf(
    //            "load [%s] xml file failed vRet=[%d][%s]\n",
    //            "/home/fczx/cfg/102SlaveFrmEle.xml",vRet,docttt.ErrorName());

    //        return vRet;
    //    }

    //    char ttStr1[][MC_XMLELE_LEM]={"root","MonFileDir","path"};
    //    T_linkXmlEles linkXmlEles1  = {ttStr1,3};
    //    char* eleVal_p1 = NULL;
    //    vRet = getSeqNumEleVal(docttt,
    //                          linkXmlEles1,
    //                          2,
    //                          eleVal_p1,
    //                          oErrMsg_p
    //                          );
    //    if(vRet!=0)
    //    {
    //        printf("%s|%s|%d:oErrMsg_p[%s]\n",__FILE__,__func__,__LINE__, oErrMsg_p);
    //        return vRet;
    //    }
    //    t_printf("%s|%s|%d:getFirstEleVal  return vRet[%d],eleVal_p=[%s]\n",
    //        __FILE__,__func__,__LINE__, vRet,eleVal_p1);



    //    return 0;


    //    tmpStrg = tmphh;
    //    tmp0x = tmpStrg.find("0x");
    //    tmp0X = tmpStrg.find("0X");
    //if(strncmp(tmphh,"0x",2)==0 || strncmp(tmphh,"0X",2)==0 )
    //    if(tmpStrg.find("0x")!=std::string::npos || tmpStrg.find("0x")!=std::string::npos)
    //    {
    //        sscanf(tmphh,"%x",&hahatmp);
    //    }
    //    else
    //    {
    //        sscanf(tmphh,"%d",&hahatmp);
    //    }

    //coverXmlVal(tmphh,hahatmp);
    //hahatmp = atoi(tmphh);
    //t_printf("%s|%s|%d:  hahatmp[%d]\n",__FILE__,__func__,__LINE__, hahatmp);

    //return 0;

    //    int prtFlag = 0;
    //    char errMsg[MC_ERRMSG_MAX]={};

    //    strcpy(errMsg,"input errr");
    //    snprintf(oErrMsg_p,sizeof(oErrMsg_p),
    //        "%s|%s|%d:[%s]\n",
    //        __FILE__,__func__,__LINE__,errMsg);
    //    //printf("oErrMsg_p[%s]\n", oErrMsg_p);

    //    snprintf(errMsg,sizeof(errMsg),
    //            "%s|%s|%d:input param ym_p or fieldNmae_p or doFlagStr_p is NULL error\n%s",
    //            __FILE__,__func__,__LINE__,oErrMsg_p);
    //    snprintf(oErrMsg_p,sizeof(oErrMsg_p),
    //        "%s|%s|%d:thththththt\n%s",
    //        __FILE__,__func__,__LINE__,errMsg);
    //    printf("oErrMsg_p[%s]\n", oErrMsg_p);
    //    return 0;

    //    int ttPrt = C_MSysInit::Instance()->getPrtFlag();
    //    printf("%s|%s|%d:xmlName[%s],prtFlag[%d]\n",
    //        __FILE__,__func__,__LINE__,
    //        C_MSysInit::Instance()->getXmlName(),
    //        ttPrt
    //        );

    //    T_meteLogs ttLogs = C_MSysInit::Instance()->getMeteLog();
    //    printf("[%s]\n", ttLogs.fullErrLog);
    //    printf("[%s]\n", ttLogs.fullGetLog);
    //    printf("[%s]\n", ttLogs.fullMainLog);


    //    //XMLDocument docTmp = C_MSysInit::Instance()->getDoc();
    //    const XMLElement* logCfgElement_p = C_MSysInit::Instance()->getDoc().FirstChildElement( MC_L_TODO )->FirstChildElement( MC_L_LOGCFG );
    //    const char * tt_p=logCfgElement_p->FirstChildElement(MC_L_ERRLOG)->FirstChildElement(MC_L_NAME)->GetText();
    //    printf("%s|%s|%d:iniSys  tt_p[%s]\n",
    //        __FILE__,__func__,__LINE__, tt_p);

    //    return 0;

    //    T_meteLogs meteLogs1={};
    //    char  locaXmlName1[MC_FULLN_LEN]={};
    //    XMLDocument doc_p;

    //    vRet = iniSys(&meteLogs1,locaXmlName1,0,1,&doc_p);

    //    printf("%s|%s|%d:iniSys  vRet[%d]\n",
    //        __FILE__,__func__,__LINE__, vRet);


    //    XMLElement* logCfgElement_p = doc_p.FirstChildElement( MC_L_TODO )->FirstChildElement( MC_L_LOGCFG );

    //    const char * tt_p=logCfgElement_p->FirstChildElement(MC_L_ERRLOG)->FirstChildElement(MC_L_PATH)->GetText();

    //    printf("%s|%s|%d:iniSys  tt_p[%s]\n",
    //        __FILE__,__func__,__LINE__, tt_p);

    //    return 0;

    //    char fullFileName_p[MC_FULLN_LEN]={};
    //    char dirName_p[]="test/tmp/1";
    //    char fileName_p[]="/hah";
    //    char errLog_p[]="/home/fczx/log/text.log";
    //    strcpy(fullFileName_p,dirName_p);

    //    vRet = conDirName(fullFileName_p,
    //                fullFileName_p,
    //                fileName_p,
    //                errLog_p,
    //                prtFlag
    //               );

    //  printf("%s|%s|%d:conDirName  vRet[%d],fullFileName_p=[%s]\n",
    //    __FILE__,__func__,__LINE__, vRet,fullFileName_p);

    //    return 0;

    char* pidNumSh_p = NULL;
    pidNumSh_p = getenv ( MC_GETPIDNUM );

    if ( pidNumSh_p == NULL )
    {
        printf ( "%s|%s|%d:  getenv [%s] return NULL error\n",
                 __FILE__, __func__, __LINE__, MC_GETPIDNUM );
        return -2;
    }

    int num = executeShell ( pidNumSh_p, "%s", "getFtp" );
    t_printf ( "%s|%s|%d:executeShell getFtp num=[%d]\n", __FILE__, __func__, __LINE__, num );

    if ( num > 1 )
    {
        printf ( "%s|%s|%d:[%s]\n", __FILE__, __func__, __LINE__, "程序已经运行，不能重复运行!" );
        return 0;
    }

    //sleep(30);

    T_meteLogs meteLogs = {};
    char  locaXmlName[MC_FULLN_LEN] = {};
    XMLDocument doc;
    vRet = iniSys ( &meteLogs, locaXmlName, oErrMsg_p, 1, &doc );
    printf ( "%s|%s|%d:  meteLogs.fullErrLog[%s]\n", __FILE__, __func__, __LINE__, meteLogs.fullErrLog );
    printf ( "%s|%s|%d:  meteLogs.fullOtherLog[%s]\n", __FILE__, __func__, __LINE__, meteLogs.fullOtherLog );
    printf ( "%s|%s|%d:  meteLogs.fullMainLog[%s]\n", __FILE__, __func__, __LINE__, meteLogs.fullMainLog );
    printf ( "%s|%s|%d:  meteLogs.debugFlag[%d]\n", __FILE__, __func__, __LINE__, meteLogs.debugFlag );
    printf ( "%s|%s|%d:  locaXmlName[%s]\n", __FILE__, __func__, __LINE__, locaXmlName );

    printf ( "%s|%s|%d:  vRet[%d]\n", __FILE__, __func__, __LINE__, vRet );

    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d:oErrMsg_p[%s]\n", __FILE__, __func__, __LINE__, oErrMsg_p );
        return vRet;
    }

    char ttStr[][MC_XMLELE_LEM] = {MC_L_TODO, MC_L_DOLIST, MC_L_NAMEINTEREGEX};
    T_linkXmlEles linkXmlEles  = {ttStr, 3};
    char* eleVal_p = NULL;
    vRet = getFirstEleVal ( doc,
                            linkXmlEles,
                            eleVal_p,
                            oErrMsg_p
                          );

    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d:oErrMsg_p[%s]\n", __FILE__, __func__, __LINE__, oErrMsg_p );
        return vRet;
    }

    t_printf ( "%s|%s|%d:getFirstEleVal  return vRet[%d],eleVal_p=[%s]\n",
               __FILE__, __func__, __LINE__, vRet, eleVal_p );

    const char* src1 = "intehealth20170807.xml";
    const char* parttern = "busi[0-9]{8,}.xml";

    vRet = regexMatch ( src1, parttern, oErrMsg_p );

    if ( vRet < 0 )
    {
        printf ( "%s|%s|%d:oErrMsg_p[%s]\n", __FILE__, __func__, __LINE__, oErrMsg_p );
        return vRet;
    }

    t_printf ( "%s|%s|%d:regexMatch  return [%d]\n",
               __FILE__, __func__, __LINE__, vRet );

    return 0;

    //    XMLPrinter printer2;
    //    XMLDocument doc2;
    //    vRet = doc2.LoadFile(locaXmlName);
    //    printf("%s|%s|%d:  vRet[%d]\n",__FILE__,__func__,__LINE__, vRet);
    //            doc2.Print(&printer2);
    //            printf("%s|%s|%d:  printer2.CStr=[%s]\n",__FILE__,__func__,__LINE__, printer2.CStr());

    //return 0;

    //    XMLDocument doc1;
    //    vRet = doc1.LoadFile(locaXmlName);
    //    printf("%s|%s|%d:  vRet[%d]\n",__FILE__,__func__,__LINE__, vRet);
    //    char ttStr[][MC_XMLELE_LEM]={"ToDo",MC_L_HEALTHRP,MC_L_FIELDNAME,MC_L_ERRLOG,MC_L_PATH};
    //    T_linkXmlEles linkXmlEles = {ttStr,3};
    //    char * eleVal_p=NULL;

    //    vRet = getFirstEleVal(doc1,linkXmlEles,eleVal_p,meteLogs.fullErrLog,0);
    //    printf("%s|%s|%d:  vRet[%d],eleVal_p=[%s]\n",__FILE__,__func__,__LINE__, vRet,eleVal_p);


    //return 0;

    //


    //    vRet = mergeXmlPs("/home/fczx/filedo/winplan/yes/winPlan20170718.xml","/home/fczx/filedo/winplan/yes/winPlan20170718_merge.xml",MC_L_DOWPLANPATH);
    //       printf("%s|%s|%d:  vRet[%d]\n",__FILE__,__func__,__LINE__, vRet);
    //       return 0;

    //    char tmpTime[MC_TIMES_LEN]={};
    //    getTime(4,tmpTime,NULL,meteLogs.fullErrLog,prtFlag);

    //    T_xmlHtItem xmlHtItem = {};
    //    strcpy(xmlHtItem.taskNum,"2");
    //    strcpy(xmlHtItem.hostName,"qx");
    //    strcpy(xmlHtItem.beWatchedN,"hahah");
    //    strcpy(xmlHtItem.beWatchedS,"ok");
    //    strcpy(xmlHtItem.capTime,tmpTime);
    //    strcpy(xmlHtItem.theMonitor,"qx1qx");

    //    T_xmlHVar xmlHVar = {};
    //    xmlHVar.cfgFlag=0;
    //    strcpy(xmlHVar.fuallName,"/home/fczx/tmp/hhhtext1.xml");
    //    strcpy(xmlHVar.fieldName,"fusk_test");
    //    xmlHVar.xmlHtItem=xmlHtItem;

    //    //memset(locaXmlName,0,sizeof(locaXmlName));

    //    vRet = putXmlLFileH(locaXmlName,MC_L_DOHPATH,xmlHVar,meteLogs.fullErrLog,prtFlag);
    //    printf("%s|%s|%d:  vRet[%d]\n",__FILE__,__func__,__LINE__, vRet);

    //    return 0;

    //    vRet = mergeToIntegXml("/home/fczx/filedo/health/do/health20170720.xml","/home/fczx/filedo/health/do/integ20170720.xml",
    //                "/home/fczx/log/err20170720.log",0);
    //    printf("%s|%s|%d:  vRet[%d]\n",__FILE__,__func__,__LINE__, vRet);

    //    return 0;

    T_doFtpSh shNames = {};
    T_ftpParas ftpParas = {};
    int runFlag = 0;

    if ( argc > 1 )
    {
        runFlag = atoi ( argv[1] );
        printf ( "%s|%s|%d:  argc=[%d] argv[1]=[%s] runFlag=[%d]\n", __FILE__, __func__, __LINE__, argc, argv[1], runFlag );

    }

    if ( ( runFlag & 0x01 ) == 1 )
    {
        printf ( "%s|%s|%d:  enter loadFtpCfg if\n", __FILE__, __func__, __LINE__ );

        vRet = loadFtpCfg ( doc, MC_L_GETDOWN, 1, shNames, ftpParas, oErrMsg_p );

        if ( vRet != 0 )
        {
            printf ( "%s|%s|%d: loadFtpCfg return  error vRet=[%d]\n", __FILE__, __func__, __LINE__, vRet );
            return vRet;
        }

    }

    if ( ( runFlag & 0x02 ) == 2 )
    {
        pid_t  pid1, pid2;
        pid1 = getpid();
        printf ( "%s|%s|%d:  enter downFtpFiles if\n", __FILE__, __func__, __LINE__ );
        vRet = downFtpFiles ( shNames, ftpParas, oErrMsg_p );

        if ( vRet != 0 )
        {
            printf ( "%s|%s|%d: downFtpFiles return  error vRet=[%d]\n", __FILE__, __func__, __LINE__, vRet );
            return vRet;
        }

        pid2 = getpid();

        if ( pid1 != pid2 ) //pid子进程返回
        {
            printf ( "%s|%s|%d:  pid1=[%d],pid2=[%d]\n", __FILE__, __func__, __LINE__, pid1, pid2 );
            return 0;
        }
    }

    if ( ( runFlag & 0x04 ) == 4 )
    {
        printf ( "%s|%s|%d:  enter synthInList if\n", __FILE__, __func__, __LINE__ );
        vRet = synthInList ( doc, "busilist_1.xml", oErrMsg_p );

        if ( vRet != 0 )
        {
            printf ( "%s|%s|%d: synthInList return  error vRet=[%d]\n", __FILE__, __func__, __LINE__, vRet );
            return vRet;
        }
    }


    if ( ( runFlag & 0x08 ) == 8 )
    {
        printf ( "%s|%s|%d:  enter statList if\n", __FILE__, __func__, __LINE__ );
        vRet = statList ( doc, "busilist_1.xml", oErrMsg_p );

        if ( vRet != 0 )
        {
            printf ( "%s|%s|%d: statList return  error vRet=[%d]\n", __FILE__, __func__, __LINE__, vRet );
            return vRet;
        }
    }


    if ( ( runFlag & 0x10 ) == 16 )
    {
        printf ( "%s|%s|%d:  enter loadFtpCfg if\n", __FILE__, __func__, __LINE__ );

        vRet = loadFtpCfg ( doc, MC_L_PUTUP, 1, shNames, ftpParas, oErrMsg_p );

        if ( vRet != 0 )
        {
            printf ( "%s|%s|%d: loadFtpCfg return  error vRet=[%d]\n", __FILE__, __func__, __LINE__, vRet );
            return vRet;
        }
    }


    if ( ( runFlag & 0x20 ) == 32 )
    {
        printf ( "%s|%s|%d:  enter putFtpFiles if\n", __FILE__, __func__, __LINE__ );

        vRet = putFtpFiles ( shNames, ftpParas, oErrMsg_p );

        if ( vRet != 0 )
        {
            printf ( "%s|%s|%d: putFtpFiles return  error vRet=[%d]\n", __FILE__, __func__, __LINE__, vRet );
            return vRet;
        }
    }


    printf ( "%s|%s|%d:  main vRet[%d]\n\n", __FILE__, __func__, __LINE__, vRet );

    //int vRet = 0;
    signal ( SIGTSTP, signal_handler );
    signal ( SIGQUIT, signal_handler );
    signal ( SIGINT, signal_handler );
    signal ( SIGKILL, signal_handler );
    signal ( SIGTERM, signal_handler );
    //    signal(SIGSTOP,signal_handler);
    //    signal(SIGKILL,signal_handler);

    while ( !end );

    return 0;


    /*0、差错处理:当检查到错误时程序上报状态的同时，存入本地数据库，并记入log，丢弃错误继续执行
                  此模块写成单独的函数进行处理，以供其他应用模块调用
                  1) 入参:用于记录状态的结构体变量
                          用于记录日志需要的信息变量(如 路径、名称)
                          记录数据库需要的信息变量
    */

    /*1、 业务清单下载:独立进程
                       C+shell
                       查询和下载周期1分钟
                       状态检查

    */


    /*2、 业务清单处理:
            合成在用业务清单
                如果有测风塔配置：生成III区测风塔采集业务处理计划
            生成业务清单提供情况
    */

    /*3、 测风数据接收*/


    /*4、测风数据处理*/



    /*5、本机任务监视和状态跟踪:生成健康报告

    */

    /*6、上传健康报告*/


    return 0;




}

