/******************************************************************************
** 文件名：unitInfMem.h
** 作者：
** 创建时间：2018-04-21
** 功能：
** 修改：
** 其它：
*******************************************************************************/

#ifndef MC_UNITINFMEM_H
#define MC_UNITINFMEM_H

#pragma once

#include <stdio.h>
#include <string.h>
#include "sysDefine.h"

#include "CPShared/CPLock.h"
#include "ptcl/DataType.h"


//typedef unsigned char u_char;
//typedef unsigned int u_int;
//typedef unsigned short u_short;

#define MC_MAX_PHYNUM  5000    //定义一个通道支持的最大点个数

#define MC_MAX_CYCLES  1000000   //定义最大循环次数,避免程序死循环

//较普遍的错误
#define MC_ERROR_CYCLES  -1000   //表示程序出现循环超过最大次数错误
#define MC_ERROR_CALLOC  -999    //calloc分配空间错误
#define MC_ERROR_CHNN0   -899    //没有找到相应的通道号
#define MC_ERROR_INPAR   -898    //接口入参错误
#define MC_ERROR_PHYTY   -897    //物理量的类型phyType错误
#define MC_ERROR_PNTAD   -896    //点地址在配置中没有找到
#define MC_ERROR_CAUS    -895    //因果关系或逻辑错误:如因前设置值才能获取得到
#define MC_ERROR_OVMAX   -700    //超过设置的最大值不允许再操作

//此处为了实现随便定义的一个数据id结构,注:正式运行时需要替换下面的结构
//#pragma pack (1)
//typedef struct
//{
//    u_char B_Confidence; //数据可信度
//    u_char B_Interval;   //数据的时间间隔
//    u_char B_Height;     //数据的所在高度
//    u_char B_Source;     //数据来源
//    u_char B_Kind;       //数据特征
//    u_char B_Length;     //数据项的字节长度
//    u_char B_Amount;     //此字节在数据为数据集时有效，代表数据集中元素的个数
//    u_short  W_Encoding; //代表所属设备编号
//    u_char B_Type;       // 数据类型
//    u_char B_Catalog;    //数据具体的大分类，包括文件，sql，数据项，数据集
//} DATA_ID;
//#pragma pack()


typedef u_int        PNT_ADDR_TYPE;    //点地址数据类型
//typedef DATA_ID      PHY_DATA_ID;       //数据id类型
typedef T_snnUsrData COM_OBJ_TYPE;      //定义与协议相关的通用信息类型

//物理量类型定义
typedef u_int PHY_UNIT_TYPE;

#define MC_INVALID_PHTYPE  0


//物理量值的算法
typedef enum
{
    PHY_CALC_INVALID = 0,     //Invalid  //无效值
    //.. 添加算法在此添加
    //...
    PHY_CALC_SAVE,             //直接保存
    PHY_CALC_CCNT,             //将点值按配置的顺序连接
	PHY_CALC_SUM,              //累加做除法
	PHY_CALC_CCNT_AND_SUM,     //将点按配置的顺序拼接并累加做除法
	PHY_CALC_CCNT_AND_BIT,     //将点按配置顺序（按位拼接） 
    PHY_CALC_LAST              //添加类型在此值之前添加,此值表示算法的个数
} PHY_CALC_METHD;  //physical quantity, method of calculation


//连续点地址段
typedef struct
{
    //+u_int       addrNumS;    /*连续点地址段包括的点地址数量*/
    PNT_ADDR_TYPE  beginAddrVal;/*开始点地址值*/
    PNT_ADDR_TYPE  endAddrVal;  /*结束点地址值*/
    //u_int        begineIndex; /*开始地址的索引下标*/
} T_contPntAdd;



//不连续点地址表配置结构
typedef struct
{
    T_contPntAdd* contPntAdd_p;/*连续地址段表头*/
    u_int num;                 /*包括不连续地址段的段数*/
} T_pntAddrCfg;


//不连续点地址表索引结构
typedef struct
{
    T_contPntAdd contPntAdd;  /*连接地址段*/
    u_int        begineIndex; /*连续地址段开始值的索引*/
} T_contAddDex;


//不连续地址表控制结构
typedef struct
{
    T_contAddDex* conAddDex_p;
    u_int         num;         /*包括不连接地址段的段数*/
} T_pntAddrCtl;


//带时标存储的通用对象结构
//typedef struct
//{
//    COM_OBJ_TYPE  relateObj; //与点地址关联的通用对象
//    void*         tmStmp_p; //时标
//}T_COM_OBJ_TYPE;


//点对象内存值结构
typedef struct
{
    //PNT_ADDR_TYPE pntAddr; //点地址
    //点地址对应的物理量(注:一个点地址对应唯一物理量)
    PHY_UNIT_TYPE   pntUType;
    COM_OBJ_TYPE    relateObj; //与点地址关联的通用对象
} T_pntMemObj;



//点存储对象结构
typedef struct
{
    //PNT_ADDR_TYPE pntAddr;     //点地址
    //u_int         objLng;      //关联对象的长度
    //PNT_ADDR_TYPE* pntAddrHead_p;
    T_pntAddrCtl  pntAddrCtl;
    T_pntMemObj* pntMemObj_p;
    void*        tmStmp_p;      //时标
} T_pntStrgObj;



//物理量与点地址的对应关系配置结构
typedef struct
{
    PHY_UNIT_TYPE  phyType;
    PHY_CALC_METHD calcMethd; //物理量值的算法
    float          multFactor;//乘法系数
    u_char         phyValLng; //物理量对应的值长度
    //u_char         phyCatName;//物理量的类别:如瞬时值,1分钟平均等
    //void*        phyObjVal_p;  //物理量值地址
    u_int          numOfAdd;  //对应点地址的个数
    PNT_ADDR_TYPE* pntAddr_p; //对应点地址的头
    void*          dataId_p;  //数据id地址
} T_phyValCfg;


//物理量与点地址的对应关系存储结构
typedef struct T_phyValStrg
{
    PHY_UNIT_TYPE  phyType;
    PHY_CALC_METHD calcMethd;    //物理量值的算法
    float          multFactor;   //乘法系数
    u_char         phyValLng;    //物理量对应的值长度
    //u_char       phyCatName;   //物理量的类别:如瞬时值,1分钟平均等
    u_char         acmTimes;     //累加次数
    bool           freshFlag;    //数据是否新鲜:true为新鲜数据
    void*          phyObjVal_p;  //物理量对应的值
    void*          dataId_p;     //数据id值
    u_int          numOfAdd;     //对应点地址的个数
    PNT_ADDR_TYPE* pntAddr_p;    //对应点地址的头
    //u_int*         addIndex_p; //点地址的值对应到内存地址数组的索引
    T_phyValStrg*  next_p;       //同样的点对应多个物理量时，next_p指向下一个物理量
} T_phyValStrg;


//物理量与点地址的对应关系存取值结构
typedef struct
{
    PHY_UNIT_TYPE  phyType;
    PHY_CALC_METHD calcMethd;   //物理量值的算法
    float          multFactor;  //乘法系数
    u_char         phyValLng;   //物理量对应的值长度
    u_char         tmStmpLng;   //时标长度
    //u_char         phyCatName;//物理量的类别:如瞬时值,1分钟平均等
    u_char         acmTimes;     //累加次数
    bool           freshFlag;    //数据是否新鲜:true为新鲜数据
    void*          phyObjVal_p; //物理量对应的值地址
    u_int          numOfAdd;    //对应点地址的个数
    void*          pntObj_p;    //每个点地址关联的对象组成的数组pntObj_p[]
    u_int          pntObjLng;   //每个pntObj_p[i]的长度
} T_phyObjVal;



//物理累计量类别索引结构
typedef struct
{
    int            categoryName;//类别名称:如1分钟值为1
    //u_int          maxNum;      //head_p的最大容量
    u_int          curNum;      //head_p当前已用的容量
    PHY_UNIT_TYPE* head_p;
} T_phyCat;


//通道时标结构
typedef struct
{
    u_char tmStmpLng; //时标长度
    void*  tmStmp_p;  //时标
    void*  dataId_p;  //时标的数据id
    bool   freshFlag; //数据是否新鲜:true为新鲜数据
} T_untChnTmStmp;


//初始化通道点表数据结构
typedef struct
{
    u_int          chnNum;       //通道号
    u_int          pntObjLng;    //点地址关联通用数据对象的长度
    u_int          dataIdLng;    //数据id长度
    u_char         tmStmpLng;    //时标长度
    u_int          pntAddrNum;   //点地址数量
    u_int          phyUnitNum;   //物理量个数
    u_int          maxPhyType;   //一个通道中最大的物理量类型值
    T_pntAddrCfg   pntAddrCfg;   //点地址段配置结构
    T_phyValCfg*   phyObjVal_p;  //物理量配置结构表头
    u_int          numPhyCat;    //phyCat_p[] 元素个数
    T_phyCat*      phyCat_p;     //物理累计量类别索引结构
    T_untChnTmStmp untChnTmStmp; //通道时标
} T_initChnUnit;




//物理量值结构
typedef struct
{
    u_int         phyUnitNum;   //物理量个数
    T_phyObjVal*  phyObjVal_p;  //物理量表头
} T_phyNVal;


//物理量配置结构
typedef struct
{
    u_int         phyUnitNum;   //物理量个数
    T_phyValCfg*  phyValCfg_p;  //物理量配置表头
} T_phyNCfg;


/******************************************************************************/





//通道点(物理量)结构
class C_chnUnitData
{
    public:
        C_chnUnitData();
        ~C_chnUnitData();

    public:
        //初始化
        int initM ( const T_initChnUnit* inPar_p );

        /*根据outData_p->phyType取特定物理量的配置值
            入参:outData_p->phyType 需要调用接口前赋值
            出参:outData_p中其他值
        */
        int getPhyCfgByType ( T_phyValCfg* outData_p
                            ) const;

        /*根据点地址取物理量的配置值
            入参:pntAddVal_p
                 outInNum 数组outData_p的大小
            出参:outData_p
                 outInNum 数组中实际赋值的个数
            返回值:
                0: 成功
                <0:失败
                >0:实际物理量个数大于入参最大容量
        */
        int getPhyCfgByPnt ( const PNT_ADDR_TYPE* pntAddVal_p,
                             T_phyValCfg outData_p[],
                             u_int&  outInNum
                           ) const;

        /*根据多个点地址取物理量的数量(对相同物理量已经去重了:
          多个点对同一个物理量只返回一个)
            入参:pntAddVal_pp
            出参:outNum_p   物理量的数量
                 outPhyType_p (可选)当此参数不为空时输出物理量的类型
        */
        int getPhyNumByNPnt ( COM_OBJ_TYPE* pntAddVal_pp[],
                              const u_int& numPnt,
                              u_int*  outNum_p,
                              PHY_UNIT_TYPE outPhyType_p[] = NULL
                            ) const;

        /*根据多个点地址取物理量的配置值(对相同物理量已经去重了:
          多个点对同一个物理量只返回一个)
            入参:pntAddVal_pp
            出参:outData_p
                 outNum_p   出参T_phyValCfg的数量
        */
        int getPhyCfgByNPnt ( COM_OBJ_TYPE* pntAddVal_pp[],
                              const u_int& numPnt,
                              T_phyValCfg outData_p[],
                              u_int*  outNum_p
                            ) const;

        //设置物理量相关值
        int setM ( const T_phyNVal* inPar_p,
                   const u_char limitFlag = 0
                 );

        //设置通道时标
        int setChnTmStmp ( const void* tmStmp_p );

        /*获取物理量相关值
            入参:inPar_p->phyUnitNum              要获取物理量个数
                 inPar_p->phyObjVal_p[i].phyType  要获取物理量的类型
            出参:inPar_p的其他值
        */
        int getM ( T_phyNVal* inPar_p );

        //获取通道时标
        int getChnTmStmp ( T_untChnTmStmp* chnTmStmp_p );

        /*获取物理量的值及数据Id
          所有出参,不需要的,直接赋予空
          入参:phyType[]
               num
               onePhyVsize 每个phyObjVal[]成员分配的空间大小
          出参:oPhyObjVal_p
               oPhyValLng[] 每个物理量的长度可能不一样,所以是数组
               oDataId_p
               oDataIdLng   每个物理量的数据id长度是一样的,所以用一个值表示
        */
        int getPhyValADid ( const PHY_UNIT_TYPE phyType[],/*物理量类型*/
                            const u_char& num,             /*物理量个数*/
                            const u_char& onePhyVsize,
                            void*  oPhyObjVal_p,/*phyObjVal[],物理量的值*/
                            u_char oPhyValLng[],/*每个物理量值的长度*/
                            void*  oDataId_p,   /*dataId[],物理量对应的数据id*/
                            u_int* oDataIdLng_p   /*数据id的长度*/
                          );

        /*获取物理量值的长度*/
        int getPhyValLng ( const PHY_UNIT_TYPE phyType[], /*物理量类型*/
                           const u_char& num,
                           u_char oPhyValLng[]/*每个物理量值的长度*/
                         ) const;

        //获取物理量类型buf的地址:此buf中存的是相应类别名的物理量类型(物理量的索引)
        const  PHY_UNIT_TYPE* getPhyCatBuf ( const int& phyCatName,/*物理量类别名*/
                                             u_int& bufLng          /*物理量类型buf长度*/
                                           ) const;

    private:
        void freeM ( void );
        int initPntStrgObj ( const T_pntAddrCfg* inPar_p,
                             const u_int&  pntObjLng,
                             const u_int&  pntAddrNum,
                             const u_char& tmStmpLng
                           );
        int initPhyToPnt ( const T_phyValCfg*  inPar_p,
                           const  u_int&       phyUnitNum,
                           const  u_int&       dataIdLng
                         );
        //初始化物理累加量的索引结构
        int initPhyCat ( const u_int& numPhyCat,
                         const T_phyCat*     phyCat_p
                       );
        //根据点地址找到其数组下标索引
        int findPntIndx ( const PNT_ADDR_TYPE& value,
                          u_int& retIndx
                        ) const ;

    public:
        u_int         m_chnNum;       //通道号

    private:
        u_int          m_dataIdLng;    //数据id长度
        u_char         m_tmStmpLng;    //时标长度

        T_pntStrgObj   m_pntStrgObj;   //点地址表头
        u_int          m_pntAddrNum;   //点地址数量
        u_int          m_pntObjLng;    //点地址关联通用数据对象的长度

        T_phyValStrg*  m_phyToPnt_p;   //物理量表头
        u_int          m_phyUnitNum;   //物理量个数
        u_int          m_maxPhyType;   //物理量的最大类型值

        T_phyCat*      m_phyCat_p;     //物理累计量类别索引结构
        u_int          m_numPhyCat;    //m_phyCat_p[]元素个数

        T_untChnTmStmp m_untChnTmStmp; //通道时标

    private:
        nsCPLock::CLock m_valLock;     //设置与取值互斥锁
        nsCPLock::CLock m_tmStmpLock;  //通道时标互斥锁

};





//物理量信息数据/(多通道点(物理量)结构)
class C_unitInfData
{

    public:
        C_unitInfData();
        C_unitInfData ( const u_int& chnNumS  /*通道总数*/
                      );
        ~C_unitInfData();


    public:/*****************************初始化******************************/
        //一次初始化所有通道
        int initM ( const u_int& chnNumS,         /*通道总数*/
                    const T_initChnUnit* inPar_p  /*T_initChnUnit[]数组*/
                  );
        //单个通道初始化
        int initM ( const T_initChnUnit* inPar_p /*T_initChnUnit指针*/
                  );


    public:/**********************get相应的配置值*****************************/
        /*根据outData_p->phyType取某个通道的特定物理量的配置值
            入参:chnNum 通道编号
                outData_p->phyType 需要调用接口前赋值
            出参:outData_p中其他值
        */
        int getPhyCfgByType ( const u_int& chnNum,
                              T_phyValCfg* outData_p ) const;


        /*根据点地址取某个通道物理量的配置值
            入参:chnNum 通道编号
                pntAddVal_p 点地址
               outInNum 数组outData_p的大小
            出参:outData_p
                 outInNum 数组中实际赋值的个数
            返回值:
                0: 成功
                <0:失败
                >0:实际物理量个数大于入参最大容量
            出参:outData_p
        */
        int getPhyCfgByPnt ( const u_int& chnNum,
                             const PNT_ADDR_TYPE* pntAddVal_p,
                             T_phyValCfg outData_p[],
                             u_int&  outInNum
                           ) const;

        /*根据多个点地址取物理量的数量(对相同物理量已经去重了:
                  多个点对同一个物理量只返回一个)
                  入参:chnNum 通道编号
                       pntAddVal_pp
                  出参:outNum_p   物理量的数量
                       outPhyType_p (可选)当此参数不为空时输出物理量的类型
         */
        int getPhyNumByNPnt (  const u_int& chnNum,
                               COM_OBJ_TYPE* pntAddVal_pp[],
                               const u_int& numPnt,
                               u_int*  outNum_p,
                               PHY_UNIT_TYPE outPhyType_p[] = NULL
                            ) const;

        /*根据多个点地址取物理量的配置值(对相同物理量已经去重了:
        多个点对同一个物理量只返回一个)
        入参:chnNum 通道编号
             pntAddVal_pp
        出参:outData_p
            outNum_p   出参T_phyValCfg的数量
        */
        int getPhyCfgByNPnt ( const u_int& chnNum,
                              COM_OBJ_TYPE* pntAddVal_pp[],
                              const u_int& numPnt,
                              T_phyValCfg outData_p[],
                              u_int*  outNum_p
                            ) const;

        /*获取通道总数和已经初始化的数量*/
        void getChnNum ( u_int* chnNumS_p, /*通道总数*/
                         u_int* curNumS_p  /*已经初始化的数量*/
                       ) const;
        /*获取物理量值的长度*/
        int getPhyValLng ( const u_int& chnNum,           /*通道号*/
                           const PHY_UNIT_TYPE phyType[], /*物理量类型*/
                           const u_char& num,
                           u_char oPhyValLng[]/*每个物理量值的长度*/
                         ) const;


    public:/**********************get及set相应的值*****************************/
        //设置物理量相关值
        int setM ( const u_int& chnNum,            /*需要赋值的通道号*/
                   const T_phyNVal* inPar_p
                 );

        /*获取物理量相关值
        入参:valChnNum
            cfgChnNum
            valNData_p->phyUnitNum              要获取物理量个数
            valNData_p->phyObjVal_p[i].phyType  要获取物理量的类型
            cfgNPhyCfg_p->phyUnitNum             要获取物理量个数
            cfgNPhyCfg_p->phyValCfg_p[i].phyType 要获取物理量的类型
        出参:valNData_p的其他值
            cfgNPhyCfg_p的其他值
        */
        int getM ( const u_int& valChnNum, /*要取值的通道号*/
                   const u_int& cfgChnNum,  /*要取配置的通道号*/
                   T_phyNVal*   valNData_p,/*物理量值结构*/
                   T_phyNCfg*   cfgNPhyCfg_p/*物理量配置结构*/
                 ) const;

        //设置通道时标
        int setChnTmStmp ( const u_int& chnNum,      /*需要赋值的通道号*/
                           const void* tmStmp_p
                         );
        //获取通道时标
        int getChnTmStmp ( const u_int& fromChnNum, /*要从哪个通道号取值*/
                           T_untChnTmStmp* chnTmStmp_p
                         );

        /*获取物理量的值及数据Id
          所有出参,不需要的,直接赋予空
          入参:chnNum
               phyType[]
               num
               onePhyVsize 每个phyObjVal[]成员分配的空间大小
          出参:oPhyObjVal_p
               oPhyValLng[] 每个物理量的长度可能不一样,所以是数组
               oDataId_p
               oDataIdLng   每个物理量的数据id长度是一样的,所以用一个值表示
        */
        int getPhyValADid ( const u_int& chnNum,           /*通道号*/
                            const PHY_UNIT_TYPE phyType[],/*物理量类型*/
                            const u_char& num,             /*物理量个数*/
                            const u_char& onePhyVsize,
                            void*  oPhyObjVal_p,/*phyObjVal[],物理量的值*/
                            u_char oPhyValLng[],/*每个物理量值的长度*/
                            void*  oDataId_p,   /*dataId[],物理量对应的数据id*/
                            u_int* oDataIdLng_p   /*数据id的长度*/
                          ) const;

        //获取物理量类型buf的地址:此buf中存的是相应类别名的物理量类型(物理量的索引)
        const  PHY_UNIT_TYPE* getPhyCatBuf ( const u_int& chnNum,    /*通道号*/
                                             const int& phyCatName, /*物理量类别名*/
                                             u_int& bufLng          /*物理量类型buf长度*/
                                           ) const;

    private:
        int findChnIndx ( const u_int& chnNum ) const;

    private:
        void freeM ( void );

    private:
        u_int          m_chnNumS;       //通道数量
        u_int          m_curNumS;       //当前已经初始化的通道数量
        C_chnUnitData* m_chnUnitHead_p; //通道表头


};




#endif //MC_UNITINFMEM_H