/**********************************************************
* name:       mete_init.h
*
* desc:      气象下载器的，初始化
*
* date:      20170803100202
*
***********************************************************/

#ifndef METE_INIT_H
#define METE_INIT_H

#include "tinyxml2.h"
#include "mete_datatype.h"

using namespace tinyxml2;


class C_MSysInit
{
    private:
        C_MSysInit();
        ~C_MSysInit();

    private:
        static C_MSysInit* cInstance_p; //全局对象

    public:
        static C_MSysInit* Instance(); //用于产生或获取全局实例对象
        void init();//给不在构造函数里完成的变量进行初始化
        //void start(); //启动应用对象
        //void stop() ; //停止并删除应用对象
        void clear();//删除应用对象

        char* toAnti_p;     //反隔摆渡文件路径
        char* toAntiBak_p;  //反隔摆渡文件路径备

        void        setPrtFlag ( int iprtFlag ); //设置prtFlag
        const int&  getPrtFlag();             //取prtFlag

        const T_meteLogs&  getMeteLog();      //取日志变量
        const XMLDocument& getDoc();          //取docLxml
        const char*        getXmlName();      //取locaXmlName

    private:

        int          prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char fullOtherLog[MC_FULLN_LEN]; //debug日志名
        T_meteLogs   meteLogs; //日志变量
        char         locaXmlName[MC_FULLN_LEN]; //本地xml文件的全文件名（带路径的文件名)
        XMLDocument  docLxml; //本地xml配置文件加载后的对象变量


};


#endif // METE_INIT_H
