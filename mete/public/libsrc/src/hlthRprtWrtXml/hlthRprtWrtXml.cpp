#include "hlthRprtWrtXml.h"



/* 统一错误信息处理*/
#define outPutErrM(oErrMsg_p,errMsg)\
    {\
        if((oErrMsg_p)!=NULL)\
        {\
            snprintf((char*)(oErrMsg_p),MC_ERRMSG_MAXH-1,\
                     "%s|%s|%d:%s\n",\
                     __FILE__,__func__,__LINE__,(errMsg));\
        }\
        else\
        {\
            printf("%s|%s|%d:%s\n",\
                   __FILE__,__func__,__LINE__,(errMsg));\
        }\
    }

/*************************************
  *功能 ：生成健康报告xml的文件头
  * 输入：
  *      doc           :XMLDocument对象
  *      ym_p          :年月日 时间变量
  *      doFlagStr_p   :业务控制字符;(判断是生成健康报告 还是 综合健康报告)
                        MC_FLAG_HLTH,生成健康报告; MC_FLAG_INTHLTH生成综合健康报告
  *      fieldNmae_p   :场站名称
  *      oErrMsg_p    如果此变量为NULL则错误信息直接printf
  *                   如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170714104643 创建函数
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putXmlHeadH ( XMLDocument& doc,
                  const char* ym_p,
                  const char* doFlagStr_p,
                  const char* fieldNmae_p,
                  const char* oErrMsg_p
                )
{
    char errMsg[MC_ERRMSG_MAXH];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( ym_p == NULL
         || fieldNmae_p == NULL
         || doFlagStr_p == NULL
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param ym_p or fieldNmae_p or doFlagStr_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    char tBuf[100] = {};

    if ( strcmp ( doFlagStr_p, MC_FLAG_INTHLTH ) == 0 )
    {
        snprintf ( tBuf, sizeof ( tBuf ), "%s %s", ym_p, MC_H_COMMENT );
    }
    else if ( strcmp ( doFlagStr_p, MC_FLAG_INTHLTH ) == 0 )
    {
        snprintf ( tBuf, sizeof ( tBuf ), "%s %s", ym_p, MC_H_COMMENT_INTEG );
    }

    doc.InsertEndChild ( doc.NewDeclaration() );
    doc.InsertEndChild ( doc.NewComment ( tBuf ) );
    XMLNode* fnm_p = doc.InsertEndChild ( doc.NewElement ( MC_H_FNM ) );
    XMLElement* fEleName_p = doc.NewElement ( MC_H_NAME );
    fEleName_p->SetText ( fieldNmae_p );
    fnm_p->InsertEndChild ( fEleName_p );
    fnm_p->InsertEndChild ( doc.NewElement ( MC_H_ITEMLIST ) );

    return 0;
}

/*************************************
  *功能 ：向康健报告的xml文件加入数据项
  * 输入：
  *      doc           :XMLDocument对象
  *      xmlHtItem     :xml元素结构体
  *      doFlagStr_p   :业务控制字符;(判断是生成健康报告 还是 综合健康报告)
                        MC_FLAG_HLTH,生成健康报告; MC_FLAG_INTHLTH生成综合健康报告
  *      oErrMsg_p    如果此变量为NULL则错误信息直接printf
  *                   如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170719093541 创建函数
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putXmlitemH ( XMLDocument& doc,
                  const T_xmlHtItem& xmlHtItem,
                  const char* doFlagStr_p,
                  const char* oErrMsg_p
                )
{
    char errMsg[MC_ERRMSG_MAXH];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( strcmp ( doFlagStr_p, MC_FLAG_HLTH ) == 0 )
    {
        //根据xmlHtItem拼itemData
        XMLElement* itemData_p = doc.NewElement ( MC_H_ITEMDATA );

        XMLElement* tskNum_p = doc.NewElement ( MC_H_TASKNUM );
        tskNum_p->SetText ( xmlHtItem.taskNum );
        XMLElement* hostName_p = doc.NewElement ( MC_H_HOSTNAME );
        hostName_p->SetText ( xmlHtItem.hostName );
        XMLElement* beWatchedN_p = doc.NewElement ( MC_H_BEWATCHEDN );
        beWatchedN_p->SetText ( xmlHtItem.beWatchedN );
        XMLElement* beWatchedS_p = doc.NewElement ( MC_H_BEWATCHEDS );
        beWatchedS_p->SetText ( xmlHtItem.beWatchedS );
        XMLElement* capTime_p = doc.NewElement ( MC_H_CAPTIME );
        capTime_p->SetText ( xmlHtItem.capTime );
        XMLElement* theMonitor_p = doc.NewElement ( MC_H_THEMONITOR );
        theMonitor_p->SetText ( xmlHtItem.theMonitor );

        itemData_p->InsertEndChild ( tskNum_p );
        itemData_p->InsertEndChild ( hostName_p );
        itemData_p->InsertEndChild ( beWatchedN_p );
        itemData_p->InsertEndChild ( beWatchedS_p );
        itemData_p->InsertEndChild ( capTime_p );
        itemData_p->InsertEndChild ( theMonitor_p );

        doc.FirstChildElement ( MC_H_FNM )->FirstChildElement (
        MC_H_ITEMLIST )->InsertEndChild ( itemData_p );

    }
    else if ( strcmp ( doFlagStr_p, MC_FLAG_INTHLTH ) == 0 )
    {
        //查找已经有的结点中任务编号是否与要添加的任务编号有相同的
        //    如果有相同的则把信息添加到已有的编号节点下面
        //    如果没有则新加一个任务编号节点然后添加信息
        XMLNode* itemList_p = doc.FirstChildElement (
                              MC_H_FNM )->FirstChildElement ( MC_H_ITEMLIST );
        XMLElement* taskNum_p = itemList_p->FirstChildElement ( MC_H_TASKNUM );
        const char* attribNameV_p = NULL;
        int i = 1;

        while ( taskNum_p != NULL )
        {
            attribNameV_p = taskNum_p->Attribute ( MC_H_TASKATTRIBV );

            if ( attribNameV_p == NULL )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "[%s]->[%s]->%d[%s] attribute [%s] is NULL error",
                           MC_H_FNM, MC_H_ITEMLIST, i, MC_H_TASKNUM, MC_H_TASKATTRIBV );
                outPutErrM ( oErrMsg_p, errMsg );

                return ERR_POINTER_NULL;
            }

            if ( strcmp ( attribNameV_p, xmlHtItem.taskNum ) == 0 )
            {
                break;
            }

            taskNum_p = taskNum_p->NextSiblingElement ( MC_H_TASKNUM );
            i++;
        }


        if ( taskNum_p != NULL ) //找到了任务编号与要添加的编号相同值的节点
        {
            XMLElement* itemData_p = doc.NewElement ( MC_H_ITEMDATA );

            XMLElement* hostName_p = doc.NewElement ( MC_H_HOSTNAME );
            hostName_p->SetText ( xmlHtItem.hostName );
            XMLElement* beWatchedN_p = doc.NewElement ( MC_H_BEWATCHEDN );
            beWatchedN_p->SetText ( xmlHtItem.beWatchedN );
            XMLElement* beWatchedS_p = doc.NewElement ( MC_H_BEWATCHEDS );
            beWatchedS_p->SetText ( xmlHtItem.beWatchedS );
            XMLElement* capTime_p = doc.NewElement ( MC_H_CAPTIME );
            capTime_p->SetText ( xmlHtItem.capTime );
            XMLElement* theMonitor_p = doc.NewElement ( MC_H_THEMONITOR );
            theMonitor_p->SetText ( xmlHtItem.theMonitor );

            itemData_p->InsertEndChild ( hostName_p );
            itemData_p->InsertEndChild ( beWatchedN_p );
            itemData_p->InsertEndChild ( beWatchedS_p );
            itemData_p->InsertEndChild ( capTime_p );
            itemData_p->InsertEndChild ( theMonitor_p );

            taskNum_p->InsertEndChild ( itemData_p );

        }
        else
        {
            XMLElement* taskNumNew_p = doc.NewElement ( MC_H_TASKNUM );
            taskNumNew_p->SetAttribute ( MC_H_TASKATTRIBV, xmlHtItem.taskNum );

            XMLElement* itemData_p = doc.NewElement ( MC_H_ITEMDATA );

            XMLElement* hostName_p = doc.NewElement ( MC_H_HOSTNAME );
            hostName_p->SetText ( xmlHtItem.hostName );
            XMLElement* beWatchedN_p = doc.NewElement ( MC_H_BEWATCHEDN );
            beWatchedN_p->SetText ( xmlHtItem.beWatchedN );
            XMLElement* beWatchedS_p = doc.NewElement ( MC_H_BEWATCHEDS );
            beWatchedS_p->SetText ( xmlHtItem.beWatchedS );
            XMLElement* capTime_p = doc.NewElement ( MC_H_CAPTIME );
            capTime_p->SetText ( xmlHtItem.capTime );
            XMLElement* theMonitor_p = doc.NewElement ( MC_H_THEMONITOR );
            theMonitor_p->SetText ( xmlHtItem.theMonitor );

            itemData_p->InsertEndChild ( hostName_p );
            itemData_p->InsertEndChild ( beWatchedN_p );
            itemData_p->InsertEndChild ( beWatchedS_p );
            itemData_p->InsertEndChild ( capTime_p );
            itemData_p->InsertEndChild ( theMonitor_p );

            taskNumNew_p->InsertEndChild ( itemData_p );

            itemList_p->InsertEndChild ( taskNumNew_p );

        }

    }


    return 0;
}

/*************************************
  *功能 ：生成健康报告相应的xml文件
  * 输入：
  *      locaXmlCfm_p    生成xml文件的文件名(带路径)
  *       doFlagStr_p   :MC_FLAG_HLTH 生成健康报告xml;
  *                      MC_FLAG_INTHLTH ，生成综合健康报告xml
  *       xmlHVar       :健康报告生成相关变量
  *      oErrMsg_p    如果此变量为NULL则错误信息直接printf
  *                   如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170719110914 创建函数
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putXmlLFileH ( const char* locaXmlCfg_p,
                   const char* doFlagStr_p,
                   T_xmlHVar& xmlHVar,
                   const char* oErrMsg_p
                 )
{
    char errMsg[MC_ERRMSG_MAXH];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( doFlagStr_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param doFlagStr_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( ( strcmp ( doFlagStr_p, MC_FLAG_HLTH ) != 0 ) &&
         ( strcmp ( doFlagStr_p, MC_FLAG_INTHLTH ) != 0 )
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param doFlagStr_p=[%s] value error",
                   doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    int vRet = 0;

    //    char tmpTime[MC_TIMES_LEN]={};
    //    getTime(3,tmpTime,NULL,errLog_p);

    //    char tFullName[MC_FULLN_LEN] = {};//带路径名的全文件名
    //    char tFieldName[MC_FNM_LEN] = {};//场站名


    //    snprintf(tFullName,sizeof(tFullName),xmlHVar.fuallName);
    //    snprintf(tFieldName,sizeof(tFieldName),xmlHVar.fieldName);

    //取时间%Y%m%d
    time_t nowTimeSec;
    struct tm nowTime;
    struct timeval tv;
    char tmpTime[20];
    memset ( &nowTimeSec, 0, sizeof ( nowTimeSec ) );
    memset ( &nowTime, 0, sizeof ( nowTime ) );
    memset ( &tv, 0, sizeof ( tv ) );
    memset ( &tmpTime, 0, sizeof ( tmpTime ) );
    gettimeofday ( &tv, NULL );
    nowTimeSec = tv.tv_sec;
    localtime_r ( &nowTimeSec, &nowTime );
    strftime ( tmpTime, sizeof ( tmpTime ), "%Y%m%d", &nowTime );

    XMLDocument toDoc;
    //文件是否存在
    vRet = access ( xmlHVar.fuallName, F_OK );

    //vRet = fileExists(xmlHVar.fuallName,errLog_p,1);
    if ( vRet != 0 ) //文件不存在则需要先构建文件头
    {
        vRet = putXmlHeadH ( toDoc,
                             tmpTime,
                             doFlagStr_p,
                             xmlHVar.fieldName,
                             oErrMsg_p
                           );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "putXmlHeadH [%s] return error vRet=[%d]\n%s",
                       doFlagStr_p, vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }
    }
    else //文件已经存在 加载xml文件
    {
        vRet = toDoc.LoadFile ( xmlHVar.fuallName );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "load xml file [%s] failed vRet=[%d][%s]",
                       xmlHVar.fuallName, vRet, toDoc.ErrorName() );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_LOAD;
        }
    }

    //把信息追加到xml中
    vRet = putXmlitemH ( toDoc,
                         xmlHVar.xmlHtItem,
                         doFlagStr_p,
                         oErrMsg_p
                       );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "putXmlitemH [%s] return error vRet=[%d]\n%s",
                   doFlagStr_p, vRet, oErrMsg_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //    toDoc.Print();

    toDoc.SaveFile ( xmlHVar.fuallName );

    toDoc.Clear();

    return 0;
}

/*************************************
  *功能 ：由已有健康报告合并成综合健康报告
  * 输入：
  *       srcXml_p     :健康报告xml文件路径;
  *       mergeXml_p   :最终合并到的文件路径;
  *       oErrMsg_p    如果此变量为NULL则错误信息直接printf
  *                    如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170720172616 创建函数
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int mergeToIntegXml ( const char* srcXml_p,
                      const char* mergeXml_p,
                      const char* oErrMsg_p
                    )
{
    char errMsg[MC_ERRMSG_MAXH];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( srcXml_p == NULL
         || mergeXml_p == NULL
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params srcXml_p or mergeXml_p doFlagStr_p is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int vRet = 0;
    vRet = access ( srcXml_p, F_OK );

    //vRet = fileExists(srcXml_p,errLog_p);
    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "access [%s] return error vRet[%d]",
                   srcXml_p, vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //加载健康报告xml文件
    XMLDocument srcDoc;
    vRet = srcDoc.LoadFile ( srcXml_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "load xml file [%s]failed vRet=[%d][%s]",
                   srcXml_p, vRet, srcDoc.ErrorName() );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_LOAD;
    }

    // srcDoc.Print();

    //合并的综合健康报告xml
    XMLDocument merDoc;
    vRet = access ( mergeXml_p, F_OK );

    //vRet = fileExists(mergeXml_p,errLog_p,1);
    if ( vRet == 0 ) //文件存在
    {
        //加载已经存在的文件
        vRet = merDoc.LoadFile ( mergeXml_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "load xml file [%s]failed vRet=[%d][%s]",
                       mergeXml_p, vRet, merDoc.ErrorName() );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_LOAD;
        }
    }
    else
    {
        //构造综合健康报告头

        //取时间%Y%m%d
        time_t nowTimeSec;
        struct tm nowTime;
        struct timeval tv;
        char tmpTime[20];
        memset ( &nowTimeSec, 0, sizeof ( nowTimeSec ) );
        memset ( &nowTime, 0, sizeof ( nowTime ) );
        memset ( &tv, 0, sizeof ( tv ) );
        memset ( &tmpTime, 0, sizeof ( tmpTime ) );
        gettimeofday ( &tv, NULL );
        nowTimeSec = tv.tv_sec;
        localtime_r ( &nowTimeSec, &nowTime );
        strftime ( tmpTime, sizeof ( tmpTime ), "%Y%m%d", &nowTime );
        //取风场名称
        const char* fieldName_p = srcDoc.FirstChildElement (
                                  MC_H_FNM )->FirstChildElement ( MC_H_NAME )->GetText();
        vRet = putXmlHeadH ( merDoc,
                             tmpTime,
                             MC_FLAG_INTHLTH,
                             fieldName_p,
                             oErrMsg_p
                           );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "putXmlHeadH [%s] return vRet=[%d]\n%s",
                       MC_FLAG_INTHLTH, vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }
    }

    const XMLElement* itemData_p = srcDoc.FirstChildElement (
                                   MC_H_FNM )->FirstChildElement ( MC_H_ITEMLIST )->FirstChildElement ( MC_H_ITEMDATA );
    T_xmlHtItem xmlHtItem = {};

    while ( itemData_p != NULL )
    {
        memset ( &xmlHtItem, 0, sizeof ( T_xmlHtItem ) );
        snprintf ( xmlHtItem.taskNum,
                   sizeof ( xmlHtItem.taskNum ),
                   itemData_p->FirstChildElement ( MC_H_TASKNUM )->GetText() );
        snprintf ( xmlHtItem.hostName,
                   sizeof ( xmlHtItem.hostName ),
                   itemData_p->FirstChildElement ( MC_H_HOSTNAME )->GetText() );
        snprintf ( xmlHtItem.beWatchedN,
                   sizeof ( xmlHtItem.beWatchedN ),
                   itemData_p->FirstChildElement ( MC_H_BEWATCHEDN )->GetText() );
        snprintf ( xmlHtItem.beWatchedS,
                   sizeof ( xmlHtItem.beWatchedS ),
                   itemData_p->FirstChildElement ( MC_H_BEWATCHEDS )->GetText() );
        snprintf ( xmlHtItem.capTime,
                   sizeof ( xmlHtItem.capTime ),
                   itemData_p->FirstChildElement ( MC_H_CAPTIME )->GetText() );
        snprintf ( xmlHtItem.theMonitor,
                   sizeof ( xmlHtItem.theMonitor ),
                   itemData_p->FirstChildElement ( MC_H_THEMONITOR )->GetText() );

        //把信息追加到xml中
        vRet = putXmlitemH ( merDoc,
                             xmlHtItem,
                             MC_FLAG_INTHLTH,
                             oErrMsg_p
                           );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "putXmlitemH [%s] return error vRet=[%d]\n%s",
                       MC_FLAG_INTHLTH, vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }

        itemData_p = itemData_p->NextSiblingElement ( MC_H_ITEMDATA );

    }

    merDoc.SaveFile ( mergeXml_p );

    //    srcDoc.Print();
    //    printf("\n-------------------------\n");
    //    merDoc.Print();

    merDoc.Clear();
    srcDoc.Clear();



    return 0;
}







