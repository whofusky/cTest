#pragma once

#define MAX_DSTG_BODY 50

namespace DATA_REPRSENTATION
{
    enum metaType//元数据类型，元数据类型不定义字符串，它们的类型是指针型
    {
        NOD_INIT = 0,   //节点应用值处于初始化过程，值未知
        ATR_INIT,       //属性应用值处于初始化过程，值未知
        NUM_ZRO,        //0整值型    [=0]
        NUM_POS,            //正整值型   [= xxx]，不含0
        NUM_ZP,             //>=0的整数型
        NUM_NEG,            //负整值型   [= -xxx]
        NUM_ALL,            //整数全集
        FLT_ZRO,            //0浮点值型  [= 0.000]
        FLT_POS,            //正浮点型   [= xx.yyy]，单精和双精由元数据长度决定，即double或float
        FLT_ZP,             //>=0.000，单精和双精由元数据长度决定，即double或float
        FLT_NEG,            //负浮点型   [= -xx.yyy]，单精和双精由元数据长度决定，即double或float
        FLT_ALL,            //浮点全集，单精和双精由元数据长度决定，即double或float
        PTR_NUL,            //空指针型   [= NULL]
        PTR_FUL,            //非空指针型，字符串以'/0'代表结束，若表示定长字符串，它们是字符型数组，由mDataDcrb给出
        PTR_ALL,            //可空的指针型
        CHR_NUL,            //空字符型   [= '/0']
        CHR_FUL,            //非空字符型 [= 'x']
        CHR_ALL,            //可取'/0'的所有字符型
        NOD_NUL,            //值为空节点，不存在的节点
        NOD_SET,            //值为节点集合
        ATR_NUL,            //值为空属性，不存在的属性
        VAL_ILG             //非法应用值
    };
    enum stgType    //storage type
    {
        STG_UNKNOWN = 100, //未知存储类型
        STG_NDST,    //节点集
        STG_NODE,        //单节点类型
        STG_UINT,        //存储单元
        STG_ATRG = 103,  //属性组类型
        STG_GRUP,        //组存储
        STG_NVMX = 104,  //节点复合值
        STG_AVMX = 104,  //属性复合值
        STG_MIMG,        //元数据映像
        STG_NVSP = 105,  //节点分量值（简单值）
        STG_AVSP = 105,  //属性分量值（简单值）
    };
    struct mDataImg//元数据映像
    {
        metaType mType;     //指明本结构体为元数据存储类型，=NUM_ZRO~ATR_NUL
        const char* name;   //元数据映像识别名称
        unsigned char mNum; //元数据数，如以定长数组表示的字符串
        unsigned char mLng; //元数据类型长度，指针型由机器字长确定，0代表无元数据
        void* mImg;     //元数据映像
    };
    struct dataGRP//数据组存储
    {
        stgType dGType;     //指明本结构体为数据组存储类型，=STG_GRUP, STG_NVMX, STG_AVMX
        const char* name;   //数据组存储的识别名称
        unsigned char dStgNum;
        mDataImg* dStgLst;
    };
    union stgSUBdy
    {
        dataGRP GRPBdy;
        mDataImg mImgBdy;
    };
    struct dataSU//数据存储单元
    {
        stgType SUType;     //指明本结构体为数据存储单元类型，=STG_UINT, STG_ATRG
        const char* name;   //存储单元的识别名称
        unsigned char dStgNum;
        stgSUBdy* dStgLst;
    };
    union stgSBBdy
    {
        dataSU SUBdy;
        dataGRP GRPBdy;
        mDataImg mImgBdy;
    };
    struct dataSB   //数据存储块
    {
        stgType sbkType;            //指明本结构体为数据存储块类型，=STG_NODE
        const char* name;           //存储块的识别名称（节点名称）
        unsigned char dStgNum;
        stgSBBdy* dStgLst[MAX_DSTG_BODY + 1];//静态分配空间，最大支持所有属性加1个节点;
    };
    struct dataSBLst
    {
        unsigned int sbNum;
        dataSB** sbBdy;
        void* stub;
    };

    /*以上结构与XML解析结果采用如下对应关系：
    1. 节点组：数据存储块数组，ndDataLst（dataSBLst*）
    2. 单节点，包含节点取值和属性取值：数据存储块，ndData_p（dataSB*），ndData（dataSB）
    3. 属性组：数据存储单元，attrDataLst（dataSU*）
    4. 节点复合值：数据组存储，ndMixVal_p（dataGRP*），ndMixVal（dataGRP）
    5. 属性复合值：数据组存储，attrMixVal_p（dataGRP*），attrMixVal（dataGRP）
    6. 节点取值的单分量实体：元数据映像，ndSmpVal_p（mDataImg*），ndSmpVal（mDataImg）
    7. 属性取值的单分量实体：元数据映像，attrSmpVal_p（mDataImg*），attrSmpVal（mDataImg）
    8. 存储块通用联合体：dStgSBBdy_p（stgSBBdy*），dStgSBBdy（stgSBBdy）
    9. 存储单元通用联合体：dStgSUBdy_p（stgSUBdy*），dStgSUBdy（stgSUBdy ）
    */
    typedef dataSBLst* ndDataLst;   //节点列表
    typedef dataSB* ndData_p;       //单节点指针
    typedef dataSB ndData;          //单节点实体
    typedef stgSBBdy* dStgSBBdy_p;      //通用存储体，适于：dataSU、dataGRP、mDataImg
    typedef stgSBBdy dStgSBBdy;
    typedef stgSUBdy* dStgSUBdy_p;      //通用存储体，适于：dataGRP、mDataImg
    typedef stgSUBdy dStgSUBdy;
    typedef dataSU* attrDataLst;    //属性组
    typedef dataGRP* ndMixVal_p;    //节点复合值指针
    typedef dataGRP ndMixVal;       //节点复合值实体
    typedef dataGRP* attrMixVal_p;  //属性复合值
    typedef dataGRP attrMixVal;
    typedef mDataImg* ndSmpVal_p;   //节点单分量值
    typedef mDataImg ndSmpVal;
    typedef mDataImg* attrSmpVal_p; //属性单分量值
    typedef mDataImg attrSmpVal;    //属性单分量值

    struct memMgr
    {
        void* memBlk_p;
        memMgr* nxt;
    };

    extern void loadOffMemMgr ( void* stub, int all );
    extern void* loadIntoMemMgr ( void* memPtr );


}
