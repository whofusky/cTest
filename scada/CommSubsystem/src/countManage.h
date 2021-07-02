
#ifndef MC_COUNTMANAGE_H
#define MC_COUNTMANAGE_H

#pragma once
//#include "FrameGlobal.h"
//#include "ptcl/MACRO.h"
#include "ptcl/DataType.h"
#include "unitInfMem.h"
#include "data.h"




typedef union
{
    short          sInt16; //有符号短整型
    unsigned short uInt16; //无符号短整型
    int            sInt32; //有符号整数
    unsigned int   uInt32; //无符号整数
    float          sFlt32; //浮点数
    double         sFlt64; //双精度浮点数
} T_pntTmpVal;


class countManage
{
    public:
        countManage ( void );
        ~countManage ( void );

    public:
        /*计算通用函数(通过点值计算出物理量的值)
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcFuncAddToVal ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*计算通用函数(拆分物理量的值到点地址的点值)
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcFuncValToAdd ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*获得数据id的对应值的类型与长度
        * 输入：did_p:数据id的二进制串
        *输入：dType：数据id的数据类型
        *返回: 数据类型的长度
        */
        int getDataType ( void* did_p, DATA_TYPE& dType );

    private:

        /*拆分物理量的值到点地址的点值--直接保存
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcTake ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p);

        /*通过点值计算出物理量的值--直接保存
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcSave ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*通过点值计算出物理量的值--将点值按配置的顺序连接
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcCcnt ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p);

        /*通过点值计算出物理量的值--累加做除法
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*通过点值计算出物理量的值--将点按配置的顺序拼接并累加做除法
        * 输入：usrData_p:通用数据对象数组
        *输入：dtCfg_p：物理量的配置结构
        *输出：dtValue_p：计算后的物理量
        */
        int calcCcntAndSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );


        /*通过点值计算出物理量的值--将点按配置顺序（按位拼接）
         * 输入：usrData_p:通用数据对象数组
         *输入：dtCfg_p：物理量的配置结构
         *输出：dtValue_p：计算后的物理量
         */
        int calcCcntAndSumBit ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );
};

#endif//MC_COUNTMANAGE_H