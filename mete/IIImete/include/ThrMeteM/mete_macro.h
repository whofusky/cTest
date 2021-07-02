#ifndef METE_MACRO_H
#define METE_MACRO_H


//定义ftp server 的 最大数量
#define MC_FTPSERNUM_MAX         10

//运行软件名称定义
#define MC_M_DOWNFTP_NAME    "meteorological ftp down"     //ftp下载程序
#define MC_M_SYNBUSI_NAME    "synthetic business"          //合成在用业务清单
#define MC_M_STATBUSI_NAME   "statistice Business"         //生成在用业务清单提供情况
#define MC_M_UPFTP_NAME     "meteorological ftp UP"        //ftp上传程序
#define MC_M_FMON_NAME     "meteorological file monitor"        //ftp上传程序


// business list's marker
#define MC_FILE                 "file"              //1）文件根结节标识符：file
#define MC_COORDINATE           "coordinate"        //2）多机协调编码：coordinate
#define MC_SUBMITER         "submiter"          //3）提交者：submiter
#define MC_SUBMITTIME           "submitTime"        //4）提交时间：submitTime
#define MC_TASKID               "taskID"            //任务编号
#define MC_JOBNUMBER            "jobNumber"         //5）业务总数：jobNumber
#define MC_JOBDESCLIST          "jobDescList"       //6）业务清单节标识符：jobDescList
#define MC_JOBITEMDESC          "jobItemDesc"       //7）业务项标识符：jobItemDesc
#define MC_JOBDATALIST          "jobDataList"       //8）业务数据节标识符：jobDataList
#define MC_JOBITEMDATA          "jobItemData"       //9）业务项数据标识符：jobItemData
#define MC_JOBDATA              "jobData"           //10）业务数据：jobData，保持原格式不变
#define MC_JOBID                "jobID"             //11）业务编号：jobID
#define MC_DATAID               "dataID"            //数据集编码 dataID
#define MC_FEATURECODE          "featureCode"       //12）特征编码：featureCode
#define MC_STATISTICALRANGE "statisticalRange"  //13）统计范围：statisticalRange
#define MC_VALIDARALID          "validAralID"       //14）适用域ID：validAralID
#define MC_DATACOMEINTOTIME "dataComeIntoTime"  //15）业务数据生效时间：dataComeIntoTime
#define MC_DATATIMEFEATURE  "dataTimeFeature"   //16）业务数据时间特征：dataTimeFeature
#define MC_POSTPROCESSCODE  "postProcessCode"   //17）后处理编码：postProcessCode
#define MC_JOBPLAN              "jobPlan"           //18）业务计划描述标识符：jobPlan
#define MC_PLANDESC         "planDesc"          //19）业务计划描述：planDesc
#define MC_PLANCOMEINTOTIME "planComeIntoTime"  //20）业务计划生效时间：planComeIntoTime
#define MC_PLANITEMNUMBER       "planItemNumber"    //21）业务计划项总数：planItemNumber
#define MC_PLANPROCESSMODE  "planProcessMode"   //22）业务计划处理方式：planProcessMode
#define MC_ARRANGER         "arranger"          //23）编制者：arranger
#define MC_PLANPROCESSLIST  "planProcessList"   //24）业务计划处理列表：planProcessList
#define MC_ACTIONNUMBER     "actionNumber"      //25）活动总数：actionNumber
#define MC_ACTIONITEM           "actionItem"        //26）活动项：actionItem
#define MC_ACTIONSTARTTIME  "actionStartTime"   //27）活动开始时间：actionStartTime
#define MC_ACTIONFINALTIME  "actionFinalTime"   //28）活动完成时间：actionFinalTime
#define MC_ACTIONALARMTIME  "actionAlarmTime"   //29）活动预警时间：actionAlarmTime
#define MC_ACTIONABORTTIME  "actionAbortTime"   //30）活动截止时间2：actionAbortTime


// 气象下载器本地配置xml元素
#define MC_L_TODO                "ToDo"         //根节点
#define MC_L_LOGCFG              "logCfg"       //日志配置
#define MC_L_DEBUG               "debugFlag"    //0 不打印dubug,1打印debug
#define MC_L_ERRLOG              "errlog"       //错误日志配置
#define MC_L_PATH                "path"         //文件路径
#define MC_L_NAME                "name"         //文件名
#define MC_L_MAINLOG             "mainLog"      //主日志配置
#define MC_L_GETFTPLOG           "getFtpLog"    //下载日志配置
#define MC_L_GETDOWN             "GetDown"      //ftp下载配置
#define MC_L_PUTUP               "PutUp"        //ftp上传配置
#define MC_L_FTPNUM              "ftpnum"       //ftp服务器数量
#define MC_L_FTPSER              "FtpSer"       //ftp服务器
#define MC_L_CYCLE               "Cycle"        //空闲时，循环检查是否有下载文件时间间隔设置
#define MC_L_SECOND              "second"       //秒
#define MC_L_MICROSECOND         "microSecond"  //微秒
#define MC_L_REPET               "repet"        //重复次数:大于0为具体的重复次数，小于0为无限循环
#define MC_L_IP                  "ip"           //ip
#define MC_L_PORT                "port"         //端口
#define MC_L_USER                "user"         //用户名
#define MC_L_PWD                 "pwd"          //密码
#define MC_L_TIMEOUTMS           "timeoutMS"    //超时设置
//fusk@2019-01-22 begine
#define MC_L_TRSTYPE             "trsType"      //0:ascii ,1:binary
#define MC_L_TRSMODE             "trsMode"      //0:passive,1:active
//fusk@2019-01-22 end
#define MC_L_CHKSHNAME           "ChkShName"    //用于校验的shell名
#define MC_L_DOSHNAME            "DoShName"     //用于执行业务的shell名
#define MC_L_REMOTEDIR           "RemoteDir"    //远端服务器路径
#define MC_L_LOCALDIR            "LocalDir"     //本地路径
#define MC_L_FILEDES             "FileDes"      //文件描述
#define MC_L_FNUM                "num"          //文件数量
#define MC_L_FILENAME            "FileName"     //文件名
#define MC_L_DOLIST              "doList"       //本地相关配置
#define MC_L_MONINTEFLAG         "monInteFlag"  //是否启用综合监视器标识
#define MC_L_SRCLIST             "srcList"      //需要处理的业务清单路径
#define MC_L_TOANTI              "toAnti"       //反隔摆渡文件路径
#define MC_L_TOANTIBAK           "toAntiBak"    //反隔摆渡文件路径备
#define MC_L_DOSTAPATH           "doStaPath"    //业务清单提供情况生成目录
#define MC_L_DOSTANAME           "doStaName"    //业务清单提供情况文件名前缀
#define MC_L_STALISTRIG           "staLisTrig"  //业务清单提供情况 任务触发目录
#define MC_L_WINDMEASURE         "windMeasure"  //测风塔本地配置
#define MC_L_ACCTHREE            "accThree"     //是否从III区接入测风塔业务

#define MC_L_DOWPLANPATH         "doWPlanPath"  //III区测风塔采集业务处理计划xml生成目录
#define MC_L_DOWPLANNAME         "doWPlanName"  //III区测风塔采集业务处理计划xml文件名前缀
#define MC_L_CMPWPLANPATH        "cmpWPlanPath"  //测风采集计划生成完毕目录，也是测风数据处理的触发目录

#define MC_L_FIELDNAME           "fieldName"     //场站名称此值暂定配置文件配，场内前台UI有配置此值

#define MC_L_DOHPATH             "doHPath"       //生成健康报告目录
#define MC_L_YESHPATH            "yesHPath"      //健康报告完成目录 也是综合健康报告触发目录
#define MC_L_FRMIIHPATH          "frmIIHPath"    //场内上传的健康报告 也是综合健康报告触发目录
#define MC_L_INTEGHPATH          "integHPath"    //综合健康报告生成目录
#define MC_L_YESINTEGHPATH       "yesIntegHPath" //综合健康报告完成目录
#define MC_L_UPHPATH             "upHPath"       //综合健康报告上传目录
#define MC_L_PRENAMEH            "preNameH"      //健康报告文件名前缀
#define MC_L_PRENAMEINTE         "preNameInte"   //综合健康报告文件名前缀
#define MC_L_NAMEINTEREGEX       "nameInteRegex" //综合健康报告文件名POSIX正则表达式

#define MC_L_HEALTHRP             "healthRp"     //健康报告元素配置节点
//#define MC_L_FIELDNAME            "fieldName"    //场站名称
#define MC_L_HOSTNMAE             "hostNmae"     //宿主名称
#define MC_L_THEMONITOR           "theMonitor"   //监视者

#define MC_L_SHTOOL               "shTool"       //公共shell工具配置
#define MC_L_GETPIDNUM            "getPidNum"    //查询$1进程数量





// 业务清单提供情况xml元素
#define MC_STA_COMMENT          "business list statistics"   // xml描述
#define MC_STA_FILE             "file"                     //根结点
#define MC_STA_ITEMDESC         "itemDesc"                 //描述信息
#define MC_STA_JOBNUMBER        "jobNumber"                //文件个数
#define MC_STA_ITEMLIST         "itemList"                 //文件列表
#define MC_STA_ITEMDATA         "itemData"                 //文件信息数据
#define MC_STA_NAME             "name"                     //文件名
#define MC_STA_COMMITTIME       "commitTime"               //提交时间
#define MC_STA_STADESC          "statDesc"                 //软件(程序)状态描述
#define MC_STA_RETTIME          "retTime"                  //状态返回的时间
#define MC_STA_RETCODE          "retCode"                  //返回的状态码
#define MC_STA_RETMSG           "retMsg"                   //返回的状态的描述信息

//III区测风塔采集业务处理计划xml
#define MC_W_COMMENT          "wind measurement mast processing plan"   // xml描述
#define MC_W_TASK            "task"            //根节点
#define MC_W_ITEMDESC        "itemDesc"        //描述信息
#define MC_W_ALLJOBNUM       "allJobNum"       //总的任务数量
#define MC_W_COMPLETENUM     "completeNum"     //已经完成的任务数量
#define MC_W_ITEMLIST        "itemList"        //列表信息
#define MC_W_ITEMDATA        "itemData"        //任务信息
#define MC_W_COMPLETEFLAG    "completeFlag"    //是否完成标识
#define MC_W_BUSLSTNAME      "busLstName"      //业务清单文件名
#define MC_W_WINDNAME        "windName"        //测风数据文件名
#define MC_W_COMMITTIME      "commitTime"      //完成时间


//多机协同编码宏定义
#define MC_COORD_MASTER       "100"               //主
#define MC_COORD_SLAVES       "200"               //备
#define MC_COORD_INTEG        "3nx"               //综合
#define MC_COORD_PARAL        "4n0"               //并行



#define DOSH_ERROR_OUTTIME      -2            //执行shell脚本超时
#define DFTP_ERROR_DUPFILE      3            //相同文件重复下载

//气象下载程序级别的统一错误编码
#define METE_ERROR_PNULL        -0x80       //指针变量为空

#define METE_SUCCS_DOWNADO      0x100000       //气象下载并业务清单处理成功

#define METE_ERROR_SER1         0x001000       //服务器1错误
#define METE_ERROR_SER2         0x002000       //服务器2错误
#define METE_ERROR_SER3         0x004000       //服务器3错误
#define METE_ERROR_SER4         0x008000       //服务器4错误

#define METE_NOFILE_DOWN       -0x000001       //没有要下载的文件

#define METE_ERROR_CONFSER      0x000001       //连接ftp服务器错误
#define METE_ERROR_UORPWD       0x000002       //用户名或密码错误
#define METE_ERROR_FSPATH       0x000003       //ftp服务器路径错误
#define METE_ERROR_LCPATH       0x000003       //本地路径错误
#define METE_ERROR_NODFILE      0x000004       //不存在要下载的文件
#define METE_ERROR_CHKDOUT      0x000005       //执行下载状态检查操作超时
#define METE_ERROR_CHKOTHER     0x000006       //执行下载状态检查操作其他错误
#define METE_ERROR_GETFTPFILE   0x000007       //执行ftp文件下载超时
#define METE_ERROR_GETOTHER     0x000008       //执行ftp文件下载其他错误

#define METE_ERROR_PARSECFG     0x000091       //解析配置文件错误
#define METE_ERROR_CFGVALUE     0x000092       //配置文件配置的值错误


#define METE_ERROR_GENHREPORT   0x000081       //生成健康报告错误
#define METE_ERROR_MVHREPORT    0x000082       //移动健康报告错误
#define METE_ERROR_MERHREPORT   0x000083       //合成健康报告错误

#define METE_ERROR_MERLIST      0x000101       //合成在用业务清单错误
#define METE_ERROR_GENLSTSTAT   0x000102      //生成业务清单提供情况错误

#define METE_ERROR_FTPPUT       0x000701      //上传ftp错误

#define METE_ERROR_WTSTAT       0x000801     //写软件状态错误
#define METE_ERROR_CPFILE       0x000901     //复制文件错误




#endif // METE_MACRO_H
