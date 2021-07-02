#ifndef APDU1_H
#define APDU1_H
#include "MACRO.h"
#include "DataType.h"




typedef struct
{
    u_char SYS_ELENum;    //个数
    ELE_TYPE* SYS_TYPE_p; //未知元素个数的数组
} DUT_INF;


typedef DUT_INF ASDU_DUT;
typedef DUT_INF ASDU_DUID;
typedef DUT_INF INF_BODY;
typedef DUT_INF APDU;


/*
    APDU_DSCR.InfoEleLng 添加新的含义 @20180222161842
    一、在分配空间时为了确保分配的空间足够大
    1、当可变结构限定词的SQ = 1 即单对象时：
       InfoEleLng表示信息元素集中每个信息元素的字节长度
    2、当可变结构限定词的SQ = 0 即多对象时：
       InfoEleLng 表示每个信息对象的字节长度
    二、在实际使用APDU的结构时，无论SQ = 1 或 0
    InfoEleLng表示信息元素集中每个信息元素的字节长度
*/
typedef struct
{
    u_int   ASDU_TID;
    u_int   InfoEleLng;//信息元素集中每个信息元素的字节长度
    u_short ASDU_CMTBL [11];
} APDU_DSCR;




/*带符号的八位位组值*/
class BS8s_Ss30
{
    private:
        u_char num;//附近符号位的9位位组个

        BS8* values_p;

        u_char signs;

    public:
        BS8s_Ss30 ( u_char num );

        ~BS8s_Ss30();

        //返回位组个数
        u_char getNumber();

        /*bitsNo为输入参数，表示要获取的位组号1~8，0表示获取所有位组；
          bitsNum为输出参数，附加符号位的9位位组个数；
          返回值：8位值宽度，另附加1个符号位的9位位组值。
        */
        BS8_S getValues ( u_char bitsNo );

        /*bitsNo为输入参数，表示要设置的位组号1~8，0表示设置所有位组；
          bitsNum为输入参数，表示要设置的附加符号位9位位组个数；
          n9BitsValue为输入参数，设置值是包含bitsNum个9位位组元素的数组；
          返回值：设置成功与否的状态。
        */
        u_int setValues ( u_char bitsNo, BS8_S* n9BitsValue_p );

};




class INFO_QUAL1
{
    private:
        u_char num;    //信息体存储占用字节数
        BS8* value_p;  //信息体地址
        QUAL1* qual_p; //品质描述词

    public:
        INFO_QUAL1();

        ~ INFO_QUAL1 ();

        /* bodyWidth为输出参数，表示信息体字节数；
           返回值：信息体指针。
        */
        BS8* getValue ( u_char* bodyWidth_p );

        //获取品质描述词
        QUAL1* getQual();

        /*设置信息体
          infoValue 为输入参数，表示信息体地址
          bodyWidth 为输入参数，表示信息体字节长度
        */
        u_int setValue ( BS8* infoValue_p, u_char* bodyWidth_p );

        /*设置信息体品质描述词
          infoQual 为输入参数，表示品质描述词地址
        */
        u_int setQual ( QUAL1* infoQual_p );
};




class COUNTER1
{
    private:
        u_char num;         //计数器字节宽度
        u_int value;        //按32位定义读数值，有效宽度由num域确定，有效值低地址优先
        CNTR_STATE cntrSeq; //计数器顺序标记，记录此计数器标识和状态

    public:
        u_char cntrType;    //计数器类型，见宏定义

    public:
        COUNTER1 ( u_char cntrType, u_char num );
        ~ COUNTER1();

        /* cntrWidth为输出参数，表示计数器读数寄存器字节宽度；
           返回值：计数器读数寄存器值，实际宽度由输出参数确定，
           有效值低地址优先。
        */
        u_int getValue ( u_char* cntrType_p, u_char* cntrWidth_p );
        u_int setValue ( u_char cntrType, u_int cntrValue, u_char cntrWidth );

        CNTR_STATE* getSeq ( u_char* cntrType_p );
        u_int setSeq ( CNTR_STATE cntrSeq );
};




class APDU1
{
    private:
        u_short DU_Lng; //数据单元长度长度
        u_char DUType;  //指明分配空间的用途：为0时APDU有效，1时ASDU有效，2时两者都有效

    protected:
        u_char allocFlag;
        u_char* APDU_p;     //APDU首地址
        u_char* ASDU_p;     //ASDU首地址
        APDU_DSCR DU_Info;  //APDU描述
        u_char asduVsqVal;  //可变结构体限定词的值:根据此值判断如何解或装信息体对象

    public:

        /*使用定制信息初始化成员APDU_Info，计算APDU总长度，设
          置APDU_Lng成员释放参数指向的地址空间
          allocFlag!=0时，分配APDU存储空间，用存储空间首地址
         （指针）设置APDU成员否则不分配APDU存储空间，成员APDU
          =NULL
        */
        APDU1 ( APDU_DSCR* cstmInfo, u_char duType, u_char  allocFlag );
        ~APDU1();//释放分配空间

        APDU_DSCR* getDU_Info();

        /*以下函数（方法）APDU获得存储位置，或向存储中设置值
          获取或设置APDU_Info.ASDU_CMTBL中未设置的基本元素
          时，返回NULL或0（失败）
          index 不为0时一般用于ASDU中多对象时指定具体取的对象
          索引
        */
        u_char* getAPDU_ELEMENT ( u_short* lng_p,
                                  u_short emt,
                                  u_char index = 0
                                );
        int setAPDU_ELEMENT ( u_char* p_Mem,
                              u_short lng,
                              u_short emt,
                              u_char index = 0
                            );

        //获得APDU的总长度
        u_short getDU_Lng ();

        //获取当前类中储存的缓冲区类型
        u_char getDUType();

        //获得APDU存储空间的首地址指针
        u_char* getDU ( char duType );

        //获得ASDU存储空间的首地址指针
        u_char* getDU ( u_short& apduLng, char duType );

        /*以下方法用于按组装表结构返回组合元素，
          它们为返回的组装表创建新的地址空间调
          用者用后要调用对应的释放方法
        */

        //获取与设置数据单元类型
        ASDU_DUT*    getASDU_DUT();
        u_int freeASDU_DUT ( ASDU_DUT* pSpace );

        //获取与设置数据单元标识符
        ASDU_DUID*   getASDU_DUID();
        u_int freeASDU_DUID ( ASDU_DUID* pSpace );

        //获取与设置信息体（对象）
        INF_BODY*    getINF_BODY ( u_char index = 0 );
        u_int freeINF_BODY ( INF_BODY* pSpace );

        u_int freeAPDU ( INF_BODY* pSpace );

        //将一块存储空间绑定到对象上，设置成员APDU=memBlock，用于接收
        u_int bindToMem ( u_char* memBlock_p, u_char duType, u_short APDU_Lng );
};




#endif // APDU1_H