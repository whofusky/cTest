

#ifndef BASE_H
#define BASE_H




typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;




typedef struct
{
    u_char value: 2;
    u_char pad: 6;
} UI2;                  //位宽度2的无符整数


typedef u_char UI8;     //位宽度8的无符整数


typedef struct
{
    u_char loBCD: 4;
    u_char hiBCD: 4;
} UI8BCD;                        //2位十进制的BCD编码无符整数


typedef UI8BCD n6UI4BCD[3];      //6位十进制的BCD编码无符整数


typedef n6UI4BCD UI24BCD;        //6位十进制的BCD编码无符整数


typedef char I8;                 //位宽度8的整数


typedef short I16;              //位宽度16的整数


typedef u_short UI16;           //位宽度16的无符整数

typedef struct
{
    u_short value: 12;
    u_short pad: 4;
} I12Right;                     //位宽度12的右对齐整数


typedef struct
{
    u_short pad: 4;
    u_short value: 12;
} I12Left;                      //位宽度12的左对齐整数


typedef struct
{
    UI8BCD n2LoBCD;
    UI8BCD n2MdBCD;
    u_char n1HiBCD: 4;
    u_char sign: 1;
    u_char pad1: 3;
    u_char pad2: 8;
} I21BCD;                      //5位十进制的BCD编码整数


typedef u_char UF8;            //位宽度8的无符归一化定点数


typedef char F8;               //位宽度8的归一化定点数


typedef F8 UF8_1;              //2倍位宽度8的无符归一化定点数


typedef struct
{
    u_short value: 15;
    u_short  sign: 1;
} F16;                         //位宽度16的归一化定点数


typedef UF8 F8_1;              //2倍位宽度8的归一化定点数


typedef struct
{
    u_short value: 11;
    u_short  sign: 1;
    u_char pad1: 4;
} F12Right;                    //位宽度12的右对齐归一化定点数


typedef struct
{
    u_char pad1: 4;
    u_short value: 11;
    u_short  sign: 1;
} F12Left;                     //位宽度12的左对齐归一化定点数


typedef float R32_23;          //总位宽度32小数位宽度23的浮点数


typedef struct
{
    u_char value: 1;
    u_char pad: 7;
} BS1;                         //位宽度1的比特串


typedef struct
{
    u_char value: 2;
    u_char pad: 6;
} BS2;                         //位宽度2的比特串


typedef BS2 n4BS2[4] ;         //4组位宽度2的比特串


typedef u_char BS8;            //位宽度8的比特串


typedef u_short BS16;          //位宽度16的比特串

//二进制编号的不定长字符串(注意：此类型的变量不分配存储空间)
typedef BS8* OS8i;


typedef UI2 DC;                //双命令


typedef UI2 PRC;               //持续调节命令


typedef UI2 SRC;               //步调节命令


typedef UI2 DPI;               //双点信息


typedef UI8 n1BINUI8;          //8取1码


typedef UI8 LTE250UI8;         //0~250的值


typedef BS1 SC;                //单命令


typedef BS8 n8SINBS8;          //8比特状态寄存器


typedef n4BS2 n4S_TINn4BS;     //8比特状态+瞬态检出


typedef BS16 n8S_n8CDINnBS16;  //16比特状态+变位检出


typedef OS8i OS8iASCII;        //ASCII字符串


typedef struct
{
    u_char value: 7;
    u_char error: 1;
} UI7V_E;                      //带差错指示的值


typedef UI7V_E UI7R120V_E;     //带差错指示范围为120的值

typedef struct
{
    u_char value: 7;
    u_char error: 1;
} UF7V_E;                      //带差错指示的归一化值


typedef struct
{
    u_char value: 6;
    u_char transient: 1;
    u_char error: 1;
} UF6V_T_E;                    //带瞬态和差错指示的值


typedef struct
{
    u_short overflow: 1;
    u_short error: 1;
    u_short value: 14;
} O_E_UF14V;                   //带差错和溢出指示的14比特归一化定点数


typedef struct
{
    u_int mScnd: 10;
    u_int pad1: 2;
    u_int scndBCD: 4;
    u_int n10Scnd: 3;
    u_int pad2: 1;
    u_int minBCD: 4;
    u_int n10Min: 3;
    u_int pad3: 1;
    u_int hourBCD: 4;
    u_short n10hour: 2;
    u_short pad4: 2;
    u_short dayBCD: 4;
    u_short n10dayBCD: 4;
    u_short n100dayBCD: 4;
} CP48TIME1BCD;               //BCD时间


typedef struct
{
    u_int mScnd1;
    u_char mScnd2: 6;
    u_char RES: 1;
    u_char SU: 1;
} CP40TIME1;                 //二进制时间1


typedef struct
{
    u_short mScnd;
    u_char min: 6;
    u_char RES1: 1;
    u_char IV: 1;
    u_char hour: 5;
    u_char RES2: 2;
    u_char SU: 1;
    u_char DOM: 5;
    u_char DOW: 3;
    u_char month: 4;
    u_char RES3: 4;
    u_char year: 7;
    u_char RES4: 1;
} CP56TIME2a;               //二进制时间2a


typedef struct
{
    u_short mScnd;
    u_char min: 6;
    u_char RES1: 1;
    u_char IV: 1;
    u_char hour: 5;
    u_char RES2: 2;
    u_char SU: 1;
    u_char DOM: 5;
    u_char DOW: 3;
    u_char week: 6;
    u_char RES3: 2;
    u_char year: 7;
    u_char RES4: 1;
} CP56TIME2b;              //二进制时间2b


typedef struct
{
    u_short mScnd: 10;
    u_short scnd: 6;
    u_char min: 6;
    u_char RES1: 1;
    u_char IV: 1;
    u_char hour: 5;
    u_char RES2: 2;
    u_char SU: 1;
    u_char DOM: 5;
    u_char DOW: 3;
    u_char month: 4;
    u_char RES3: 4;
    u_char year: 7;
    u_char RES4: 1;
} CP56TIME2c;             //二进制时间2c



typedef struct
{
    BS8 value;
    u_char sign: 1;
    u_char pad: 7;
} BS8_S;                  //带符号的八位Bit串


typedef struct
{
    u_char OV: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} QUAL1;                  //品质描述词


typedef struct
{
    u_char memSQ: 5;
    u_char memCY: 1;
    u_char memCA: 1;
    u_char memIV: 1;
} CNTR_STATE;             //计数器顺序标记




/******************************************************************************/

typedef union//quality description word - without dataVal, otherwise see as data.
{
    struct//general QDS
    {
        u_char OV: 1;
        u_char RES: 3;
        u_char BL: 1;
        u_char SB: 1;
        u_char NT: 1;
        u_char IV: 1;
    } infoQDS;
    struct//relay protection equipment event QDS
    {
        u_char RES: 3;
        u_char EI: 1;
        u_char BL: 1;
        u_char SB: 1;
        u_char NT: 1;
        u_char IV: 1;
    } rlyPrtEqpEvtQDS;
    struct//binary couter order discription
    {
        u_char SQ: 5;
        u_char CY: 1;
        u_char CA: 1;
        u_char IV: 1;
    } bcrOrderDS;
    struct//cause of initial
    {
        u_char KPA: 6; //parameter type
        u_char LPC: 1;
        u_char POP: 1;
    } QPM;
} T_infoQDS;


typedef struct
{
    u_char  sysEleID;
    u_short  lng;
    u_char* p_Mem;
} ELE_TYPE;


//u_short stnAcsFlag 的位存储变量
typedef struct
{
    //位0 = 1存储子系统执行数据值的保存；位1 = 1存储子系统执行数据值的加载；全 = 0,
    // do nothing；全 = 1，加载且保存
    u_char  ctlType: 2;
    //保留位
    u_char  ctlRes: 1;
    //APDU元素第3位ASDU_VSQ_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  vsqValid: 1;
    //APDU元素第4位ASDU_COT_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  cotValid: 1;
    //APDU元素第5位ASDU_ADDR_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  addrValidA: 1;
    //APDU元素第6位BODY_TID_INDE ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  tidValid: 1;
    //APDU元素第7位BODY_ADDR_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  addrValidB: 1;
    //APDU元素第8位BODY_SET_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  setValid: 1;
    //APDU元素第9位BODY_TSP_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  tspValidB: 1;
    //APDU元素第10位ASDU_TSP_INDEX ，=1表示ctlType的操作在此元素中有效，=0为无效
    u_char  tspValidA: 1;
    u_char  pad: 5;
} T_stnAcsFlag;

//u_short stnAcsFlag 的位存储变量
typedef struct
{
    /*位0 = 1存储子系统执行数据值的保存；
    位1 = 1存储子系统执行数据值的加载；
    全 = 0, do nothing；全 = 1，加载且保存*/
    u_short  ctlType: 2;
    u_short  ctlRes: 1;     //保留位
    /*ADU元素第3位PDU_DATA_INDEX ，
    =1表示ctlType的操作在此元素中有效，
    =0为无效*/
    u_short  dataValid: 1;
    u_short pad: 12;
} T_stnAcsFlagM;

typedef union
{
    struct
    {
        u_char fName[2];
        u_char fLng[3];
    } fNmAndLng;

    struct
    {
        u_char fName[2];
        u_char sName;
    } fAndsNm;

    struct
    {
        u_char fName[2];
        u_char sName;
        u_char sLng;//ction Length
    } fsNmAndsLng;
    struct
    {
        u_char fName[2];
        u_char sName;
        u_char LSQ;//last section or segment discription
    } fsNmAndsLSQ;
    struct
    {
        u_char fName[2];
        u_char sName;
        u_char LOS;//lenth of segment
    } fsNmAndlLng;
} T_fileDS;


typedef struct
{
    u_int   pntAddr;            // 点地址
    u_int   pntVal1;            // 点值的低4字节，验收时用作阈值，或最小值
    u_int   pntVal2;            // 点值的高4字节，验收时用作最大值
} T_gVal_ChkStd;


typedef struct
{
    u_int pntAddr;
    u_int pntVal;               //点值得低4字节，验收时用作校验和
} T_fVal_ChkStd;


typedef struct
{
    //位0 = 1 表示按最大值验收，//位 1 = 1时按最小值验收
    u_short vrfCd;
    union
    {
        T_fVal_ChkStd fchkStd;
        T_gVal_ChkStd gChkStd;
    } chkStd;
    //验收状态码//位0 = 1 表示按最大值验收成功，//位 1 = 1时按最小值验收成功
    int* chkRslt_p;
} vrfItem;


typedef struct
{
    u_short* TID_p;   // TID_p[]
    u_char*  TIDLng_p;//TIDLng_p[]每一个tid元素的长度
    u_short COT;      // COT
    u_short MaxLen;   // 最大帧长度
    u_char  TidNum;
} ptclStep;


typedef struct
{
    u_short     ssnDscNo;       //会话号
    //判断值是否为0xffff 如果为真表示错误，如果为假(char*) 表示主动性
    u_short     act_prio_chnlID;
    u_char      usingMode;      //模式码
    ptclStep*   remoteSet_p;
    u_char      setNum;         //ptclStep个数
    u_char      realTID;        //当前帧使用的tid
} T_ptclFunDsc;


////////////////////////////104协议/////////////////////////////////////////
typedef ELE_TYPE ASDU_TID;      //类型标识
typedef ELE_TYPE ASDU_LNG;      //ASDU长度
typedef ELE_TYPE ASDU_VSQ;      //可变结构限定词
typedef ELE_TYPE ASDU_COT;      //传送原因
typedef ELE_TYPE ASDU_ADDR;     //ASDU公共地址
typedef ELE_TYPE BODY_TID;      //信息体类型
typedef ELE_TYPE BODY_ADDR;     //信息体地址
typedef ELE_TYPE BODY_SET;      //信息元素集
typedef ELE_TYPE BODY_TSP;      //信息体时标
typedef ELE_TYPE ASDU_TSP;      //ASDU时标
/*
针对上面的数据结构添加额外说明@20180222164533
    BODY_TID , BODY_ADDR , BODY_SET ,BODY_TSP , ASDU_TSP 这5种类型
    在ASDU不同结构时对应的 lng 成员 表示的含义如下：
    1、当可变结构限定词的SQ = 1 即单对象时：
        lng 表示的是 p_Mem对应地址中数据的整个长度
    2、当可变结构限定词的SQ = 0 即多对象时：
        lng 表示的是 p_Mem对应地址中单个元素的长度
*/


typedef struct
{
    BODY_TID   body_Tid;    //信息体类型
    BODY_ADDR  body_Addr;   //信息体地址
    BODY_SET   body_Set;    //信息元素集
    BODY_TSP   body_Tsp;
} BODY_OBJ;


typedef struct
{
    ASDU_TID  asduTid;      //类型标识
    ASDU_LNG  asduLng;      //ASDU长度
    ASDU_VSQ  asduVsq;      //可变结构限定词
    ASDU_COT  asduCot;      //传送原因
    ASDU_ADDR asduAddr;     //ASDU公共地址
    BODY_OBJ* bodyObj_p;    //信息对象
    ASDU_TSP  asduTsp;
} dataFrame;
////////////////////////////end 104协议/////////////////////////////////////////



////////////////////////////modbus协议/////////////////////////////////////////
typedef u_short ADU_TRANS_ID; //事务元标识符
typedef u_short ADU_PCTL_ID;  //协议标识符
typedef u_short ADU_LENGTH;   //长度
typedef u_char  ADU_UNIT_ID;  //单元标识符

typedef ELE_TYPE ELE_MBAPH;    //MBAP报文头

typedef struct
{
    ADU_TRANS_ID   trans_id;  //事务元标识符
    ADU_PCTL_ID    pctl_id;   //协议标识符
    ADU_LENGTH     length;    //长度
    ADU_UNIT_ID    unit_id;   //单元标识符
} MBAP_HEADER; //MBAP报文头

typedef ELE_TYPE ADU_ADDR;  //ADU 地址域
typedef u_char   PDU_FC;    //PDU功能码
typedef ELE_TYPE PDU_DATA;  //PDU数据
typedef ELE_TYPE ADU_ECHK;  //ADU差错校验

typedef struct
{
    MBAP_HEADER mbapHeader; //MBAP报文头
    ADU_ADDR    aduAddr;    //ADU 地址域
    PDU_FC      pduFc;      //PDU功能码
    PDU_DATA    pduData;    //PDU数据
    ADU_ECHK    aduEchk;    //ADU差错校验
} T_modbusFrame;

////////////////////////////end modbus协议//////////////////////////////////////

typedef union//定义配置协议的通过帧结构
{
    T_modbusFrame modbus;
    //...  可扩展其他协议
} T_comFrame;




/*
    20180411144232 注释 (*reqEleVal_pp)[11] 及 (*respEleVal_pp)[11]
                   添加 comReqData_p 及 comRespData_p
*/
typedef struct
{
    /*位0 = 1存储子系统执行数据值的保存；位1 = 1存储子系统执行数据值的加载；全 = 0,
          do nothing；全 = 1，加载且保存
    位3 - 10 分别表示APDU元素3 -10，=1表示，位0与位1的操作在此元素中有效，=0为无效
    */
    u_short      stnAcsFlag;

    //请求帧个数
    u_char       reqAPDUNum;
    u_char*      reqBody_setNum_p;

    // reqEleVal_pp指向数据的指针，这个数组有11个元素，ELE_TYPE ele[i][11] ,
    //0<=i<reqASDUNum
    ELE_TYPE     ( *reqEleVal_pp ) [11];

    //104协议用
    dataFrame*    reqData_p;
    //bodbus及其他协议用
    T_comFrame*   comReqData_p;

    //响应帧个数
    u_char       respAPDUNum;
    u_char*      respBody_setNum_p;

    //respEleVal_pp指向数据的指针，这个数组有11个元素 ELE_TYPE ele[i][11] ,
    //0<=i<reqASDUNum
    ELE_TYPE         ( *respEleVal_pp ) [11];

    //104协议用
    dataFrame*    respData_p;
    //bodbus及其他协议用
    T_comFrame*   comRespData_p;

} T_ssnStepCfg;




typedef struct
{
    u_char  ssnID;   //会话号
    u_char  actFlag; //会话的主动性，保留
    u_char  infoType;//会话的信息类型，通用信息对象或文件信息对象
    u_short cmnAddr; //公共地址
    u_char  stepNum; //包含的会话总步数
    T_ssnStepCfg* stepCfg_p; //会话步配置 stepCfg[i], 0<=i<stepNum
} T_snnCfg; //会话步数据信息，包含该步内单向所有信息


// 会话参数
typedef struct//通用信息对象
{
    u_char  pntType;            //指示了信息对象类型和取值范围 //点的类型
    u_char  tmStmpType;         // 信息对象时标类型    非104，101情况，时标字节
    void* tmStmp_p;             //时标
    T_gVal_ChkStd val_chkStd;   //元素集
    T_infoQDS dDsc;             // 通用信息对象限定词
} snnPar_gInfoObj;


typedef struct ONE_STEP
{
    u_char stepNo;      //步号
    u_char reqPDU_FCode;//请求帧使用的帧格式编码，见2.2.5节
    u_char reqPDU_Num;  //请求帧（PDU）个数
    //请求帧APDU对象指针表，个数为reqPDU_Num，同类型APDU共用相同对象，
    //表中放入相同的APDU对象指针
    u_char* reqAPDU_Obj_p;

    //响应帧使用的帧格式编码。不需要响应时=unknow，见2.2.5节
    u_char respPDU_FCode;
    //响应PDU个数。0--不需要响应；255：需要以ACK（肯定或否定）作为响应；
    //1~254：需要响应的PDU数
    u_char respPDU_Num;
    //响应帧APDU对象指针表，个数为respPDU_Num，同类型APDU共用相同对象，
    //表中放入相同的APDU对象指针
    u_char* respAPDU_Obj_p;
    //专用步验证块，在用户配置时给出。无时为NULL，只使用配套规约库的步验证方法
    vrfItem* sVerifyBlock_p;
    //下一步指针。最后一步时，=NULL
    ONE_STEP* nextStep_p;
} ONE_STEP;


typedef struct
{
    //通道组号确定远程站，即使相同协议不同的远程站也可能采用不时的协议实现方式，
    //因此需要给出标识
    u_int chGropNo;
    //服务号，即功能号
    u_char srvNo;

    /*在用模式码，模式码用于区分相同功能在不同场景中的不同实现方法。
    模式码值是steps_p[]数组域的索引，即模式码=i的步组合保存在steps[i]规则步链中,
    所在通信层保存在cmLvl[i]中，步数保存在stepNum[i]中。
    当usingMode=AUTO时，表示在用模式由APDU决定，无法静态赋值。
    当usingMode=UNKNOWN时，表示基本规约库提供所有模式规则步链，
    具体使用哪一条，或某条规则链的组成步数，需由配套规约具体指定。
    模式码中不同功能中分别定义。*/
    u_char usingMode;

    ONE_STEP* steps_p[4];
    //所在通信层，见2.2.5
    u_char cmLvl[4];
    //包含的步数
    u_char stepNum[4];
} FUN_RL;


typedef struct//文件信息对象
{
    u_char  pntType;            //指示了信息对象类型和取值范围
    u_char  tmStmpType;         // 信息对象时标类型
    void* tmStmp_p;             //时标
    // 指示点T_fileDS的信息类型，以及数据内容，如校验码、文件数据等
    T_fVal_ChkStd val_chkStd;
    T_fileDS dDsc;              // 文件对象限定词
} snnPar_fInfoObj;


typedef union
{
    snnPar_gInfoObj gData;
    snnPar_fInfoObj fData;
} T_snnUsrData;




typedef struct  //会话步数据信息，包含该步内单向所有信息对象，
{
    //步号
    u_char stepNo;
    //=0，snnPar_gInfoObj或=1，snnPar_fInfoObj
    u_char infoType;

    //位0 = 1存储子系统执行数据值的保存；位1 = 1存储子系统执行数据值的加载；全 = 0,
    //      do nothing；全 = 1，加载且保存
    //位3 - 10 分别表示APDU元素3 -10，=1表示，位0与位1的操作在此元素中有效，=0为无效；
    u_short stnAcsFlag;

    //主动性标识，=0，主动，协议用于发送的数据；=1，被动，协议用于验收的数据，此时
    //pntAddrOrVal表示地址，
    u_char actFlag;

    //=单向总帧数
    u_char  ASDUIDNum;

    // ASDU 标识符（数据单元类型标识）
    //modbus中用于记录某一帧如下数据:“事务元标识符”2字节，“单元标识符”1字节，
    //“功能码”1字节
    u_int*  ASDUID_p;
    //传输原因
    u_short COT;
    //公共地址
    u_short cmnAddr;
    // 访问方法，用于形成可变结构限定词
    u_char acsMthd;
    //数据元素（信息对象）个数（点数）//该步的所有点数
    //输出
    u_short pntNum;
    //当前起始点位置
    u_short pntPos;

    /*信息对象，实现为usrData[pntNum]数组，元素类型为snnPar_gInfoObj
    或snnPar_fInfoObj
    公共时标类型*/
    u_char tmStmpType;

    //时标
    void* tmStmp_p;

    T_snnUsrData* usrData_p;
    u_short pntStorNum;//在modbus中，当帧请求中即有读也有写的时候，此值表示写的量
    u_short pntLoadNum;//在modbus中，当帧请求中即有读也有写的时候，此值表示读的量
    u_short usrMaxIndex;//usrData_p分配空间时的最大个数

} T_snnPara;




//modbus需要在发送一帧请求时需要记录节点，用于验证响应数据是否正确
typedef struct
{
    u_short trans_id;//Transaction Identifier
    u_char  func_id; //Function Code
} T_mbsChkNode;



//modbus需要在发送所有请求时需要记录节点，用于验证响应数据是否正确
typedef struct
{
    u_short    max_index;   //支持的最大结点索引（超过此值有数据越界可能）
    u_short    nex_index;   //下一个添加数据结点的索引
    u_short    cur_index;   //当前数据区域最大索引
    T_mbsChkNode one_node_p[1];//Variable Length data
} T_mbsChkData;



typedef struct
{
    u_short     sendnum;
    u_short     receivenum;
    short       wValue;
    short       kValue;
    u_char      stage;
    //T_mbsChkNode的空间记录请求帧中的功能码和事物处理标
    //识，以便核对服务端发回的响应帧
    T_mbsChkData chkData;
} realTimeValue;                        // 通道实时参数


typedef struct
{
    // 数据发送的控制模式, 它有3种工作模式。SEND_NO_CONTROL：无控制模式；
    //SEND_TIMEOUT_CONTROL: 超时控制模式SEND_RETYR_CONTROL: 重试次数控制模式
    u_char      Mode;                       //默认配置

    u_short     K;                          //K值得最大范围
    u_short     W;                          //W值得最大范围
} realTimeThreshold;




/****/
/*                以上在基本规则、101和104规约库中定义               */
/*********************************************************************/

/*****************************************MODBUS*******************************/
//typedef struct
//{
//
//    u_char        mode;////配置参数cfgParam_p指明了MODBUS包的结构:RTU ASII TCP
//    u_short       W;   //W值得最大范围
//}thresholdCfgPara;


/*****************************************END   *******************************/




#endif // CP_SEMAPHORE_H