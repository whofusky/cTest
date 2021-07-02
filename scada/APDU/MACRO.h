
#pragma once

#ifndef MACRO_H
#define MACRO_H




/*****************************************MODBUS*******************************/
#define REVERS_FLAG             0   //大小端是否转换：0 转换  1不转换
#define ADU_CMTBL_NUM           5   //ADU描述符个数

#define ADU_TYPE                1   //ADU域有效标识
#define PDU_TYPE                0   //PDU域有效标识

#define MAX_ARRAY_INDEX         255 //一般字符数据最大下标值

/***********************MODBUS ADU系统编号********************/
#define ADU_MBAPH_INDEX         0   //ADU MBAP报文头
#define ADU_ADDR_INDEX          1   //ADU 地址域
#define PDU_FC_INDEX            2   //PDU功能码
#define PDU_DATA_INDEX          3   //PDU数据
#define ADU_ECHK_INDEX          4   //ADU差错校验
/************************************************************/

#define MODBUS_TID              1  //为modbus定义的特殊tid
#define MODBUS_COT_ACT          6  //为modbus定义的特殊cot
#define MODBUS_COT_ACK          7  //为modbus定义的特殊cot
/*************配套协议编码************************************/
#define P_MODBUS_TCP         0x6a  //tcp 为偶数，串口为基数

/***************************MODBUS帧**************************/
//对串链路通信来说 MODBUS PDU= 256 - 服务器地址（1字节）- CRC(2字节)
#define MPDUMAX 253
//TCP MODBUS ADU= 253 + MBAP(7字节)
#define MBAPMAX 260

#define MB_REQ_F                0       //MODBUS 请求 帧
#define MB_RSP_F                1       //MODBUS 响应 帧
#define MB_EXCEP_F              3       //MODBUS 异常响应 帧
#define UNKNOW_F                2

/*********************MODBUS线圈状态值定义*********************/
#define MC_COIL_STATUS_ON       1       //线圈状态ON值
#define MC_COIL_STATUS_OFF      0       //线圈状态OFF值


/********************T_snnUsrData结构中存的数据类型************/
#define PNTTYPE_ADDR            1      //起始地址
#define PNTTYPE_NUM             2      //读或写的数量
#define PNTTYPE_BITE_NUM        3      //字节数
#define PNTTYPE_VALUE           4      //输出值或写入值
#define PNTTYPE_MASK            5      //And_Mask与Or_Mask值
#define PNTTYPE_READ_REG        6      //读寄存器
#define PNTTYPE_WITE_REG        7      //写寄存器



/**********************帧格式**********************************/
#define PROTOCOL_MODBUS     0x0000  //MODBUS协议标识字符

#define MODBUS_ASCII        0x00    //MODBUS协议采用ASCII数据帧格式
#define MODBUS_RTU          0x01    //MODBUS协议采用RTU数据帧格式
#define MODBUS_TCP          0x02    //MODBUS协议采用TCP数据帧格式



///////////  底层接收侧ADU解析错误类型      ///////////////
#define  MC_ADU_OK          0  // ADU数据解析成功
#define  MC_UNKNOWN_MODE    1  // 未配置MODBUS协议的帧格式
#define  MC_NATIVE_WARNING  2  // 本地超时告知错误
#define  MC_REMOTE_FLOWCTRL 3  // 远程传输控制要求流量控制
#define  MC_NATIVE_FLOWCTRL 4  // 本地要求流量控制
#define  MC_APDU_DISORDER   5  // APDU帧失序错误
#define  MC_APDU_CHECKSUM   6  // APDU帧校验失败
#define  MC_UNKNOWN_MODBUS  7  // 无法识别的帧


////////MODBUS 功能码定义///////////////////////////////////
#define  MC_MBFC_0x01      0x01 //功能码01：读线圈
#define  MC_MBFC_0x02      0x02 //功能码02：读离散量输入 
#define  MC_MBFC_0x03      0x03 //功能码03：读保持寄存器
#define  MC_MBFC_0x04      0x04 //功能码04：读输入寄存器
#define  MC_MBFC_0x05      0x05 //功能码05：写单个线圈
#define  MC_MBFC_0x06      0x06 //功能码06：写单个寄存器
#define  MC_MBFC_0x0F      0x0F //功能码15：写多个线圈
#define  MC_MBFC_0x10      0x10 //功能码16：写多个寄存器
#define  MC_MBFC_0x14      0x14 //功能码20：读文件记录
#define  MC_MBFC_0x15      0x15 //功能码21：写文件记录
#define  MC_MBFC_0x16      0x16 //功能码22：屏蔽写寄存器 
#define  MC_MBFC_0x17      0x17 //功能码23：读写多个寄存器 
#define  MC_MBFC_0x2B      0x2B //功能码43：读设备识别码 
#define  MC_MBFC_0xFE      0xFE //自定义扩展功能码0:新联一个通道
#define  MC_MBFC_0xFF      0xFF //自定义扩展功能码255:关一个通道 

//判断功能码是否在定义的范围内
#define ASSERTMBFC(mbFc) ( (mbFc) == MC_MBFC_0xFE \
                           || (mbFc) == MC_MBFC_0x01 \
                           || (mbFc) == MC_MBFC_0x02 \
                           || (mbFc) == MC_MBFC_0x03 \
                           || (mbFc) == MC_MBFC_0x04 \
                           || (mbFc) == MC_MBFC_0x05 \
                           || (mbFc) == MC_MBFC_0x06 \
                           || (mbFc) == MC_MBFC_0x0F \
                           || (mbFc) == MC_MBFC_0x10 \
                           || (mbFc) == MC_MBFC_0x14 \
                           || (mbFc) == MC_MBFC_0x15 \
                           || (mbFc) == MC_MBFC_0x16 \
                           || (mbFc) == MC_MBFC_0x17 \
                           || (mbFc) == MC_MBFC_0x2B \
                           || (mbFc) == MC_MBFC_0xFF  )

/******************************************************************************/






#define BIN_CNTR1               1   //二进制计数器读数1
#define BIP_CNTR1               2   //双极性二进制计数器读数1
#define BCD_CNTR1               3   //BCD计数器读数1

#define APDU_TYPE               0   //APDU域有效标识
#define ASDU_TYPE               1   //ASDU域有效标识
#define APDUANDASDU_TYPE        2   //APDU与ASDU域有效标识




/***********************APDU系统编号************************/
#define APDU_APCI_INDEX         0   //应用规约控制接口
#define ASDU_TID_INDEX          1   //类型标识
#define ASDU_LNG_INDEX          2   //ASDU长度
#define ASDU_VSQ_INDEX          3   //可变结构体限定词
#define ASDU_COT_INDEX          4   //传送原因
#define ASDU_ADDR_INDEX         5   //ASDU公共地址
#define BODY_TID_INDEX          6   //信息体类型
#define BODY_ADDR_INDEX         7   //信息体地址
#define BODY_SET_INDEX          8   //信息元素集
#define BODY_TSP_INDEX          9   //信息体时标
#define ASDU_TSP_INDEX          10  //ASDU时标
/************************************************************/




/* 功能为将value的index位设置为0或1*/
#define SetBit(sign,index,value_p)\
    {\
        u_char bit;\
        if (value_p == 0){\
            bit = 1<< index;\
            u_char nMark     = 0;\
            nMark = (~nMark) ^ (bit);\
            sign &= nMark;\
        }\
        else{\
            bit   = 1<<index;\
            sign |= bit;\
        }\
    }




//判断tid是否为 <22...29>兼容的tid
#define CCIFTID(tmpTID) (tmpTID >= MC_TID_22 && tmpTID <= MC_TID_29)


//104信息元素长度超过4字节的tid
#define OVER4TID(tmpTID) (tmpTID == MC_TID_7 \
                          || tmpTID == MC_TID_8 \
                          || tmpTID == MC_TID_13 \
                          || tmpTID == MC_TID_14 \
                          || tmpTID == MC_TID_15 \
                          || tmpTID == MC_TID_16 \
                          || tmpTID == MC_TID_20 \
                          || tmpTID == MC_TID_33 \
                          || tmpTID == MC_TID_36 \
                          || tmpTID == MC_TID_37 \
                          || tmpTID == MC_TID_50 \
                          || tmpTID == MC_TID_63 \
                          || tmpTID == MC_TID_112)




//信息对象类型
#define GINFOTYPE    0    //通用信息对象
#define FINFOTYPE    1    //文件信息对象

//信息对象时标类型
#define TIME_TYPE_56_2A 104 //104规约默认时标类型：时标CP56Time2a
#define TIME_TYPE_24_2A 101 //101默认时标类型：时标CP56TIME2a




/***************************104帧***************************/
#define APDUMAX 255
#define I_F             0       //I帧
#define S_F             1       //S帧
#define U_F             3       //U帧
#define UNKNOW_F        2

#define U_SRTDT_ACT     0x04    //U帧STARTDT激活
#define U_SRTDT_CON     0x08    //U帧STARTDT确认
#define U_STPDT_ACT     0x10    //U帧STOPDT激活
#define U_STPDT_CON     0x20    //U帧STOPDT确认
#define U_TST_ACT       0x40    //U帧TESTFR激活
#define U_TST_CON       0x80    //U帧TESTFR确认

#define PNTTYPE_NULL_SET   1    //信息对象没有信息元素集
#define PNTTYPE_HIGH_SET   2    //信息对象点值超出u_int表示范围,需要点值的高4字节




/***************************************************************************/
/*****************************配置信息*************************************/
#define CHILD_STAND_NUM 2
/**************************************************************************/
#define ACTIVE_TYPE     0   //主动
#define PASSIVE_TYPE    1   //被动

/**协议数据的方向定义**/
#define MC_CONTROL_DIRECTION 0 //控制方向
#define MC_MONITOR_DIRECTION 1 //监视方向

/**突发**/
#define MC_OUTBREAK_DIRECTION 1 //突发方向


/**************************配套协议编码************************************/
#define P_101               0x01
#define P_102               0x02
#define P_103               0x03
#define P_104               0x04
/*********************104协议最大帧长度*************************************/
#define MAX104_APDU_LNG     255
#define MAX104_ASDU_LNG     249
#define MAX104_VSQ_N        2   //一帧中最大的点集个数
/********************************帧格式************************************/
#define F1_2                2
#define UNDEFINE            0xFF
#define SRTCHAR_F1_1        UNDEFINE
#define SGLCTLCHAR_F1_1     0x0 // SGLCTLCHAR:单控制字符
#define SRTCHAR_F1_2_V      0x68    //F1_2：FT1.2，V：变长，SRTCHAR起始字符
#define SRTCHAR_F1_2_F      0x10    //F：定长
#define SGLCTLCHAR1_F1_2    0xE5    // SGLCTLCHAR1:单控制字符1
#define SGLCTLCHAR2_F1_2    0xA2    // SGLCTLCHAR2:单控制字符2
#define ENDCHAR_F1_2        0x16        //ENDCHAR:结束字符
#define SRTCHAR_F2          0x27
#define SGLCTLCHAR_F2       0x14
#define SRTCHAR_F3          0x0564
#define DBLCTLCHAR_F3       0x123D  //DBLCTLCHAR: 双控制字符，启动字符2

/*******************************功能编码**************************************/

#define STN_INIT        1       //站初始化
#define QUE_DATA        2       //查询数据
#define CYC_TRNS        3       //周期传输
#define ACQ_EVT         4       //事件收集
#define QUKCHK_EVT      5       //快检事件
#define STN_INRGTN      6       //站召唤
#define CLK_SYN         7       //时钟同步
#define CMD_TRNS        8       //命令传输
#define ACMU_TRNS       9       //累积量传输
#define PARA_LD         10      //参数加载
#define TST_PRCD        11      //测试过程
#define FILE_TRNS       12      //文件传输
#define ACQ_TRNSDLY     13      //收集传输延迟值
#define ACMU_FRZ        14      //累积量传输前的冻结    此功能是从CMD_TRNS功能的
//模式2，控制站先冻结再请求传输分离出来的
#define U_STARTDT       15      //104协议U帧STARTDT 抽象的会话
#define U_STOPDT        16      //104协议U帧STOPDT 抽象的会话
#define U_TESTFR        17      //104协议U帧TESTFR 抽象的会话
#define U_NEWHOLD       0xFE    //用U帧的STARTDT STOPDT TESTFR 全为0 表示出新的
//会话：新联一个通道
#define U_DELHOLD       0xFF    //用U帧的STARTDT STOPDT TESTFR 全为1 表示出新的
//会话：关一个通道




/*****************************模式码******************************************/
#define MAX_MODE_N  4       //模式码的最大个数

#define AUTO        254
#define UNKNOWN     255
#define ONLY_ONE    0       //唯一模式

//用查询方式采集数据
#define POLL_EVT    0       //此方式不能插入其它数据通信过程
#define POLL_DATA   1       //此方式可插入事件突发过程
#define RD_DATA     2

//时钟同步
#define CRCT_REQ    0       //请求校时
#define CRCT_BC     1       //广播校时

//命令传输
#define CMD_SE      0       //选择模式（不含执行），成功会话进入选择模式
#define CMD_DE      1       //执行模式，会话在选择或直接执行模式下均可执行
#define CMD_CNCL    2       //撤消方式，会话仅在选择模式下方可执行

//累积量传输
#define CTLD_DRCTSONPT_TRNS        0    //模式0，被控站直接突发传输
#define CTL_DRCTREQ_TRNS           1    //模式1，控制站直接请求传输
#define CTL_FRZREQ_TRNS            2    //模式2，控制站先冻结再请求传输
#define CTL_FRZ_CTLD_SONPT_TRNS    3    //模式3，控制站冻结后被控站突发传输

//装载参数
#define ONLY_LD         0       //只加载，模式0
#define ONLYACT_LD      1       //只激活，模式1
#define AUTOACT_LD      2       //自动激活，模式2




/**************************通信层编码*******************************************/
#define PHY_LVL         0x01
#define LNK_LVL         0x02
#define NAS_LVL         0x04            //网络访问层
#define LNK_NAS_LVL     0x06

#define APP_LVL         0x08
#define LNK_APP_LVL     0x82        //=(APP_LVL<<4)|LNK_LVL
#define NAS_APP_LVL     0x83        //=(APP_LVL<<4)|NAS_LVL
#define LNK_NAS_APP_LVL 0x86        //=(APP_LVL<<4)|LNK_NAS_LVL
#define UNKNOWN_LVL     0x00

/*************************/
#define MC_SEND_WAIT_CHECK       0x11           // 发送等待验收
#define MC_SEND_CNUE_EXEC        0x12           // 发送连续执行
#define MC_SEND_SSN_RETRY        0x13           // 发送会话重试
#define MC_SEND_SSN_FNSH         0x16           // 发送会话完成

#define MC_NO_SEND_WAIT_CHECK    0x01       // 不发送等待验收
#define MC_NO_SEND_CNUE_EXEC     0x02       // 不发送连续执行
#define MC_NO_SEND_SSN_STOP      0x04       // 不发送会话中止
#define MC_NO_SEND_SSN_FNSH      0x06       // 不发送会话完成




/*********************************************************************/
/*                以下在基本规则、101和104规约库中定义               */
/****/
//
#define UNDEDFINE_TID   0
#define M_SP            1
#define M_DP            3
//...
#define F_DR            126
#define OUTRANGE_TID    127

//#define time stamp type
#define NO_TMSTAMP      0
#define CP16_TM2a       2
#define CP24_TM2a       3
#define CP56_TM2a       7




/**************************CAUSE OF TRANSMISSION (COT)*************************/
//Cause:=UI6[1...6] <0...63>
//0 未用
#define MC_COT_1         1  //周期、循环;
#define MC_COT_2         2  //背景扫描
#define MC_COT_3         3  //突发(自发);
#define MC_COT_4         4  //初始化
#define MC_COT_5         5  //请求或者被请求
#define MC_COT_6         6  //激活站召唤;
#define MC_COT_7         7  //确认激活站召唤;
#define MC_COT_8         8  //停止激活;
#define MC_COT_9         9  //停止激活确认
#define MC_COT_10        10 //激活终止
#define MC_COT_11        11 //远方命令引起的返送信息
#define MC_COT_12        12 //当地命令引起的返送信息
#define MC_COT_13        13 //文件传输
/*14...19  为配套标准兼容范围保留*/
#define MC_COT_14        14
#define MC_COT_15        15
#define MC_COT_16        16
#define MC_COT_17        17
#define MC_COT_18        18
#define MC_COT_19        19
#define MC_COT_20        20 //响应站召唤;
#define MC_COT_21        21 //响应第1 组召唤;
#define MC_COT_22        22  //响应第2 组召唤
#define MC_COT_23        22  //响应第3 组召唤
#define MC_COT_24        22  //响应第4 组召唤
#define MC_COT_25        22  //响应第5 组召唤
#define MC_COT_26        22  //响应第6 组召唤
#define MC_COT_27        22  //响应第7 组召唤
#define MC_COT_28        22  //响应第8 组召唤
#define MC_COT_29        22  //响应第9 组召唤
#define MC_COT_30        22  //响应第10组召唤
#define MC_COT_31        22  //响应第11组召唤
#define MC_COT_32        22  //响应第12组召唤
#define MC_COT_33        22  //响应第13组召唤
#define MC_COT_34        22  //响应第14组召唤
#define MC_COT_35        22  //响应第15组召唤
#define MC_COT_36        36 //响应第16组召唤;
#define MC_COT_37        37  //响应计数量（累计量）站（总）召唤
#define MC_COT_38        38  //响应第1组计数量（累计量）召唤
#define MC_COT_39        39  //响应第1组计数量（累计量）召唤
#define MC_COT_40        40  //响应第1组计数量（累计量）召唤
#define MC_COT_41        41  //响应第1组计数量（累计量）召唤
/*42...43  为配套标准兼容范围保留*/
#define MC_COT_41        41
#define MC_COT_42        42
#define MC_COT_43        43
#define MC_COT_44        44  //未知的类型标识
#define MC_COT_45        45  //未知的传送原因
#define MC_COT_46        46  //未知的应用服务数据单元公共地址
#define MC_COT_47        47  //未知的信息对象地址
/*48...63  特殊应用保留（专用范围）*/
#define MC_COT_48        48
#define MC_COT_49        49
#define MC_COT_50        50
#define MC_COT_51        51
#define MC_COT_52        52
#define MC_COT_53        53
#define MC_COT_54        54
#define MC_COT_55        55
#define MC_COT_56        56
#define MC_COT_57        57
#define MC_COT_58        58
#define MC_COT_59        59
#define MC_COT_60        60
#define MC_COT_61        61
#define MC_COT_62        62
#define MC_COT_63        63




/**************************TYPE IDENTIFICATION (TID)***************************/
//类型标识=UI8[1...8] <0...127>
//在监视方向上的过程信息
//0 未定义
#define MC_TID_1         1     //单点信息
#define MC_TID_2         2     //带时标的单点信息
#define MC_TID_3         3     //双点信息
#define MC_TID_4         4     //带时标的双点信息
#define MC_TID_5         5     //步位置（档位）信息
#define MC_TID_6         6     //带时标的步位置（档位）信息
#define MC_TID_7         7     //32比特串
#define MC_TID_8         8     //带时标的32比特串
#define MC_TID_9         9     //测量值，规一化值
#define MC_TID_10        10    //测量值，带时标的规一化值
#define MC_TID_11        11    //测量值，标度化值
#define MC_TID_12        12    //测量值，带时标的标度化值
#define MC_TID_13        13    //测量值，短浮点数
#define MC_TID_14        14    //测量值，带时标的短浮点数
#define MC_TID_15        15    //累计量
#define MC_TID_16        16    //带时标的累计量
#define MC_TID_17        17    //带时标的继电保护设备事件
#define MC_TID_18        18    //带时标的继电保护设备成组启动事件
#define MC_TID_19        19    //带时标的继电保护设备成组输出电路信息
#define MC_TID_20        20    //带变位检出的成组单点信息
#define MC_TID_21        21    //测量值，不带品质描述词的规一化值
/* 22...29 为将来兼容定义保留*/
#define MC_TID_22        22
#define MC_TID_23        23
#define MC_TID_24        24
#define MC_TID_25        25
#define MC_TID_26        26
#define MC_TID_27        27
#define MC_TID_28        28
#define MC_TID_29        29
#define MC_TID_30        30    //带CP56Time2a时标的单点信息
#define MC_TID_31        31    //带CP56Time2a时标的双点信息
#define MC_TID_32        32    //带CP56Time2a时标的步位置信息
#define MC_TID_33        33    //带CP56Time2a时标的32比特串
#define MC_TID_34        34    //带CP56Time2a时标的测量值，规一化值
#define MC_TID_35        35    //带CP56Time2a时标的测量值，标度化值
#define MC_TID_36        36    //带CP56Time2a时标的测量值，短浮点数
#define MC_TID_37        37    //带CP56Time2a时标的累计量
#define MC_TID_38        38    //带CP56Time2a时标的继电保护设备事件
#define MC_TID_39        39    //带CP56Time2a时标的继电保护设备成组启动事件
#define MC_TID_40        40    //带CP56Time2a时标的继电保护设备成组输出电路信息
/* 41...44 为将来兼容定义保留*/
#define MC_TID_41        41
#define MC_TID_42        42
#define MC_TID_43        43
#define MC_TID_44        44
//在控制方向的过程信息
#define MC_TID_45        45    //单点命令
#define MC_TID_46        46    //双点命令
#define MC_TID_47        47    //步调命令
#define MC_TID_48        48    //设定值命令，规一化值
#define MC_TID_49        49    //设定值命令，标度化值
#define MC_TID_50        50    //设定值命令，短浮点数
#define MC_TID_51        51    //32比特串
/* 52...69 为将来兼容定义保留:104又定义了其中58到64*/
#define MC_TID_52        52
#define MC_TID_53        53
#define MC_TID_54        54
#define MC_TID_55        55
#define MC_TID_56        56
#define MC_TID_57        57
#define MC_TID_58        58    //带时标CP56Time2a的单点命令
#define MC_TID_59        59    //带时标CP56Time2a的双点命令
#define MC_TID_60        60    //带时标CP56Time2a的步调命令
#define MC_TID_61        61    //带时标CP56Time2a的设定值命令，规一化值
#define MC_TID_62        62    //带时标CP56Time2a的设定值命令，标度化值
#define MC_TID_63        63    //带时标CP56Time2a的设定值命令，短浮点数
#define MC_TID_64        64    //带时标CP56Time2a的32比特串

//在监视方向的系统命令
#define MC_TID_70        70    //初始化结束
/* 71...99 为将来兼容定义保留*/
#define MC_TID_71        71
#define MC_TID_72        72
#define MC_TID_73        73
#define MC_TID_74        74
#define MC_TID_75        75
#define MC_TID_76        76
#define MC_TID_77        77
#define MC_TID_78        78
#define MC_TID_79        79
#define MC_TID_80        80
#define MC_TID_81        81
#define MC_TID_82        82
#define MC_TID_83        83
#define MC_TID_84        84
#define MC_TID_85        85
#define MC_TID_86        86
#define MC_TID_87        87
#define MC_TID_88        88
#define MC_TID_89        89
#define MC_TID_90        90
#define MC_TID_91        91
#define MC_TID_92        92
#define MC_TID_93        93
#define MC_TID_94        94
#define MC_TID_95        95
#define MC_TID_96        96
#define MC_TID_97        97
#define MC_TID_98        98
#define MC_TID_99        99
//在控制方向的系统命令
#define MC_TID_100       100   //站（总）召唤命令
#define MC_TID_101       101   //计数量召唤命令
#define MC_TID_102       102   //读命令
#define MC_TID_103       103   //时钟同步命令
#define MC_TID_104       104   //测试命令
#define MC_TID_105       105   //复位进程命令
#define MC_TID_106       106   //延时获得命令
/* 107...109 为将来兼容定义保留:104又定义了其中107*/
#define MC_TID_107       107   //带时标CP56Time2a的测试命令
#define MC_TID_108       108
#define MC_TID_109       109
//在控制方向的参数命令
#define MC_TID_110       110   //测试量值参数，规一化值
#define MC_TID_111       111   //测试量值参数，标度化值
#define MC_TID_112       112   //测试量值参数，短浮点数
#define MC_TID_113       113   //参数激活
/* 114...119 为将来兼容定义保留*/
#define MC_TID_114        114
#define MC_TID_115        115
#define MC_TID_116        116
#define MC_TID_117        117
#define MC_TID_118        118
#define MC_TID_119        119
//文件传输
#define MC_TID_120       120   //文件准备就绪
#define MC_TID_121       121   //节准备就绪
#define MC_TID_122       122   //召唤目录，选择文件，召唤文件，召唤节
#define MC_TID_123       123   //最后的节，最后的段
#define MC_TID_124       124   //认可的文件，认可的节
#define MC_TID_125       125   //段
#define MC_TID_126       126   //目录
/* 127 为将来兼容定义保留:104又定义了其中127*/
#define MC_TID_127       127   //日志查询－请求存档文件
/*136...255 特殊应用（专用范围）*/
#define MC_TID_136       136   //U帧抽象会话 STARTDT
#define MC_TID_137       137   //U帧抽象会话 STOPDT
#define MC_TID_138       138   //U帧抽象会话 TESTFR
#define MC_TID_139       139   //U帧抽象会话 NEWHOLD 新联一个通道
#define MC_TID_140       140   //U帧抽象会话 DELHOLD 关一个通道




/******************************基本应用功能**************************************/
#define C_IC_NA         100 //召唤命令类型标识(tid)
#define C_RD_NA         102 //读命令类型标识（tid）
#define C_CS_NA         103 //时钟同步命令类型标识（tid）
#define C_TS_NA         104 //测试命令类型标识（tid）

/******************************VARIABLE STRUCTURE QUALIFIER (VSQ)***************/
#define C_N_0           0   //可变结构限定词N部分为0;
#define C_N_1           1   //可变结构限定词N部分为1;
#define C_SQ_0          0   //可变结构限定词SQ部分为0;
#define C_SQ_1          1   //可变结构限定词SQ部分为1;

/******************************步号********************************************/
#define C_STEP_NO_ONE   1   //步号：1
#define C_STEP_NO_TWO   2   //步号：2
#define C_STEP_NO_THREE 3   //步号：3
#define C_STEP_NO_FOUR  4   //步号：4

/***************************************************************************/
#define TIMER_NOP   0       //定时器空操作，继续原状态（或暂停恢复）
#define TIMER_START 1       //定时器启动操作
#define TIMER_PAUSE 2       //定时器定值操作，重新复位
#define TIMER_STOP  3       //定时器暂停操作

/***************************************************************************/




// 站点角色
#define MASTER_STATION     0x1  // 控制站（主站）
#define SLAVE_STATION      0x2  // 被控制站（子站）
#define COMBINED_STATION   0x3  // 系统站（综合站）

// 控制站（主站）的状态类型
#define MASTER_STOPPED              0x0  // STOPPED连接
#define MASTER_STARTED              0x1  // STARTED连接
#define MASTER_UNCONFIRMED_STOPPED  0x2  // 挂起未确认STOPPED连接
#define MASTER_PEND_STARTED         0x3  // 挂起STARTED连接
#define MASTER_PEND_STOPPED         0x4  // 挂起STOPPED连接

// 被控制站（子站）的状态类型
#define SLAVE_STOPPED               0x0  // STOPPED连接
#define SLAVE_STARTED               0x1  // STARTED连接
#define SLAVE_UNCONFIRMED_STOPPED   0x2  // 挂起未确认STOPPED连接

// 定义数据帧的类型的功能
#define IEC104_TESTFR_ACK   0x83  // TESTFR确认
#define IEC104_TESTFR_ACT   0x43  // TESTFR激活
#define IEC104_STOPDT_ACK   0x23  // STOPDT确认
#define IEC104_STOPDT_ACT   0x13  // STOPDT激活
#define IEC104_STARTDT_ACK  0x0B  // STARTDT确认
#define IEC104_STARTDT_ACT  0x07  // STARTDT激活

#define IEC104_NEWHOLD_ACT  0x03  // 新联一个通道 激活
#define IEC104_NEWHOLD_ACK  0xAB  // 新联一个通道 确认
#define IEC104_DELHOLD_ACT  0xFF  // 关一个通道 激活
#define IEC104_DELHOLD_ACK  0x57  // 关一个通道 确认

// 通道执行编码
#define NO_OPERATION   0x0  // 不对链路进行处理
#define OPEN_CONNECT   0x1  // 打开链路
#define CLOSE_CONNECT  0x2  // 关闭链路
#define RETRY_CONNECT  0x3  // 重启链路
#define TEST_CONNECT   0x4  // 获取链路状态信息
#define WRITE_CONNECT  0x5  // 通过链路发送数据
#define READ_CONNECT   0x6  // 通过链路读取数据
#define PROC_RECV_CTRL 0x7  // 通过链路来处理接收的传输控制数据
#define PROC_RECV_ASDU 0x8  // 通过链路来处理接收的ASDU数据

///////////  底层接收侧APDU解析错误类型      ///////////////
#define  MC_APDU_OK         0  // APDU数据解析成功
#define  MC_REMOTE_WARNING  1  // 远程传输控制告知错误
#define  MC_NATIVE_WARNING  2  // 本地超时告知错误
#define  MC_REMOTE_FLOWCTRL 3  // 远程传输控制要求流量控制
#define  MC_NATIVE_FLOWCTRL 4  // 本地要求流量控制
#define  MC_APDU_DISORDER   5  // APDU帧失序错误
#define  MC_APDU_CHECKSUM   6  // APDU帧校验失败
#define  MC_APDU_UNKNOWN    7  // 无法识别的帧

// 在准备发送数据之前，检查发送状态，返回状态的编码
#define SEND_READY_STATUS       0x0  // 发送准备状态正常，发送操作准备就绪；
#define SEND_ROLE_ERROR         0x1  // 站点的角色错误；
#define SEND_FRAME_ERROR        0x2  // 非法的数据帧，比如U帧同时存在多种功能；
#define SEND_STATUS_ERROR       0x3  // 非法的发送状态，即状态机中不存在的状态；
#define SEND_TRANSITION_ERROR   0x4  // 状态迁徙错误，即不满足发送数据的条件；
#define SEND_TIMESCALE_ERROR    0x5  // 发送时标错误；

//异常错误统一编码
#define ERR_POINTER_NULL        -0x80 //指针变量为空报错
#define ERR_STEP_NO             -0x01 //步号错误
#define ERR_FRAME_NO            -0x02 //帧号错误
#define ERR_COT_VAL             -0x03 //cot值错误
#define ERR_ACT_PAS_VAL         -0x04 //主被动性参数值错误
#define ERR_VSQN_NO             -0x05 //限定词与实际元素个数不等
#define ERR_TID_VAL             -0x06 //TID值错误
#define ERR_MAX_VAL_CH          -0x15 //最大值验收失败
#define ERR_MIN_VAL_CH          -0x16 //最小值验收失败
#define ERR_INTE_VAL_CH         -0x17 //区间值验收失败
#define ERR_FRAME_LNG           -0x18 //最大帧长度错误
#define ERR_ACS_MTHD            -0x19 //帧的最大点个数配置错误
#define ERR_CONFIG              -0x1A //配置错误
#define ERR_TIME_TYPE           -0x1B //时标类型错误
#define ERR_VSQN_SQ             -0x1C //限定词的sq类型错误
#define ERR_PNTVAL_LNG          -0x1D //点值应有的长度错误
#define ERR_INFOELELNG_CFG      -0x1E //信息元素集中每个信息元素的字节长度配置错误
#define ERR_SYSTEM_LOGIC        -0x1F //系统逻辑错误
#define ERR_LOGIC_RUN           -0x20 //逻辑错误
#define ERR_FC_VAL_RAN          -0x21 //会话号或Function Code 值的范围错误
#define ERR_RESP_NUM            -0X22 //响应的数量错误
#define ERR_ADDR_VAL            -0X23 //
#define ERR_INPUT_VAL           -0x24 //参数输入错误
#define ERR_VSQN_N              -0x25 //限定词的N类型错误




#ifndef FunTypeOne
    typedef void ( *FunTypeOne ) ( void* );
#endif




#endif // MACRO_H