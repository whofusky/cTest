/****************************************
 * 
 *    PvDir.h 
 * 
 *    Create by fushikai
 * 
 *    2022-09-25
 * 
 ****************************************/
#ifndef LIB_PVDIR_H
#define LIB_PVDIR_H



/******************************************************************************/
// 说明:
//      此接口实现的功能为实现光伏2.0项目中取公共配置文件夹名而写
//      1. 调用此接口的程序运行在如下两种目录结构中
//        (1) /一级目录名/项目名/id1/程序文件夹名
//        (2) /一级目录名/项目名/id1/id2/程序文件夹名
//      2. 调用GetPvPubDir接口获取"/一级目录名/项目名/id1/pubfile"目录下
//         公共文件夹的路径
//      3. 调用GetPvPubFile接口获取"/一级目录名/项目名/id1/pubfile/cfg"目录下
//         公共配置文件名(带路径)
//      4. 调用GetPvRunPar接口获取:运行程序相关路径参数

/******************************************************************************/




// "/xxx/pvf20/id1/pubfile"目录下公共的路径枚举定义
//   p1:预测主  p2:预测备  s: scada
typedef enum{
    PV_DIR_APPSENDP1=0,  // app-send-p1 用于气象服务器非EDS软件发往p1
    PV_DIR_APPSENDP2,    // app-send-p2 用于气象服务器非EDS软件发往p2
    PV_DIR_APPSENDS,     // app-send-s  用于气象服务器非EDS软件发往s
    PV_DIR_EDSSENDP1,    // eds-send-p1 用于气象服务器EDS软件发往p1
    PV_DIR_EDSSENDP2,    // eds-send-p2 用于气象服务器EDS软件发往p2
    PV_DIR_EDSSENDS,     // eds-send-s  用于气象服务器EDS软件发往s
    PV_DIR_EDSRCVP1,     // eds-receive-p1 用于气象服务器EDS软件接收p1发出的
    PV_DIR_EDSRCVP2,     // eds-receive-p2 用于气象服务器EDS软件接收p2发出的
    PV_DIR_EDSRCVS,      // eds-receive-s  用于气象服务器EDS软件接收s发出的
    PV_DIR_APPRCV,       // app-receive  用于所有服务器上非EDS软件接收EDS转换后
    PV_DIR_APPSEND,      // app-send     用于非气象服务器上非EDS软件发送给EDS的
    PV_DIR_EDSSEND,      // eds-send     用于非气象服务器上EDS软件发送正隔的
    PV_DIR_EDSRCV,       // eds-receive  用于非气象服务器上EDS软接收反隔的
    PV_DIR_CFG,          // cfg          公共配置文件夹
    PV_DIR_DSRC,         // dsrc         第一数据源文件夹
    PV_DIR_BACKUP,       // backup       备份文件夹
    PV_DIR_TEMP,         // temp         临时文件夹
    PV_DIR_EXPORT,       // export       导出文件夹
    PV_DIR_DSCSQLIN,     // descsql-in   dsc的sql输入文件夹
    PV_DIR_DSCSQLOUT,    // descsql-out  sc的sql输出文件夹
    PV_DIR_APPBACK,      // appback      软件备份文件夹 
    PV_DIR_COUNT
}PUB_PV_DIR_TYPE;


//  "/xxx/pvf20/id1/pubfile/cfg"目录下公共文件枚举定义
typedef enum{
    PV_PUBCFG_FILE=0,    // pub_config.ini
    PV_APP_LIST,         // applist.txt
    PV_PUBFILE_COUNT
}PUB_PV_FILE_TYPE;


// 程序运行相关的变量枚举定义
typedef enum{
    RUNPAR_ID1=0,   // 程序运行路径中的一级ID值
    RUNPAR_ID2,     // 程序运行路径中的二级ID值(注:如果路径中不带二级ID则为空）
    RUNPAR_PATH,    // 程序运行路径
    RUNPAR_EXE,     // 运行程序的程序名
    RUNPAR_PATHEXE, // 带路径值的运行程序名
    RUNPAR_CFGFILE, // 带路径值的程序配置文件名
    RUNPAR_VERFILE, // 带路径值的程序版本文件名
    RUNPAR_COUNT
}APP_RUNPAR_TYPE;


// 返回值定义
enum PVDIRError{
    PVDIR_SUCCESS = 0,
    PVDIR_GETEXEPATH_ERR, /*获取当前程序的运行路径错误*/
    PVDIR_APPRUNDIR_ERR,  /*程序运行的目录结构与要求的不符合,至少在/xxx/pvf20/id下运行*/
    PVDIR_TYPE_UNKNOWN_ERR, /*输入类型在系统中未定义*/

    PVDIR_ERROR_COUNT
};




/*********************************************************************
 *  Dsc:获取公共文件夹路径
 *  in : iType  获取文件夹类型:需要是PUB_PV_DIR_TYPE类型变量
 *  out: ret    当不为空时则返回PVDIRError类型的返回值
 *
**********************************************************************/
const char* GetPvPubDir( int iType, int* ret = NULL );


/*********************************************************************
 *  Dsc:获取公共配置文件名(带路径的文件名)
 *  in : iType  获取文件类型:需要是PUB_PV_FILE_TYPE类型变量
 *  out: ret    当不为空时则返回PVDIRError类型的返回值
 *
**********************************************************************/
const char* GetPvPubFile( int iType, int* ret = NULL );


/*********************************************************************
 *  Dsc:获取程序运行相关参数值(文件运行路径，运行程序名...等)
 *  in : iType  获取类型:需要是APP_RUNPAR_TYPE类型变量
 *  out: ret    当不为空时则返回PVDIRError类型的返回值
 *
**********************************************************************/
const char* GetPvRunPar( int iType, int* ret = NULL );


/*********************************************************************
 *  Dsc:返回值的字符描述
 *  in : errorId  上面接口返回的ret值
 *
**********************************************************************/
const char* PvStrError( int errorId );






class CPvDir
{
    private:
        CPvDir();
        ~CPvDir();

    private:
        static const char* _errorNames[ PVDIR_ERROR_COUNT+1 ];

    public:
        static CPvDir* Instance( void );
        static const char* ErrorIDToName( int errorId);

    public:
        // iType: PUB_PV_DIR_TYPE
        const char* getDir( int iType, int* ret = NULL );
        // iType: PUB_PV_FILE_TYPE
        const char* getFile( int iType, int* ret = NULL );
        // iType: APP_RUNPAR_TYPE
        const char* getPar( int iType, int* ret = NULL );
};

#endif //LIB_PVDIR_H
