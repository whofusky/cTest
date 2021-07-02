
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
    short          sInt16; //�з��Ŷ�����
    unsigned short uInt16; //�޷��Ŷ�����
    int            sInt32; //�з�������
    unsigned int   uInt32; //�޷�������
    float          sFlt32; //������
    double         sFlt64; //˫���ȸ�����
} T_pntTmpVal;


class countManage
{
    public:
        countManage ( void );
        ~countManage ( void );

    public:
        /*����ͨ�ú���(ͨ����ֵ�������������ֵ)
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcFuncAddToVal ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*����ͨ�ú���(�����������ֵ�����ַ�ĵ�ֵ)
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcFuncValToAdd ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*�������id�Ķ�Ӧֵ�������볤��
        * ���룺did_p:����id�Ķ����ƴ�
        *���룺dType������id����������
        *����: �������͵ĳ���
        */
        int getDataType ( void* did_p, DATA_TYPE& dType );

    private:

        /*�����������ֵ�����ַ�ĵ�ֵ--ֱ�ӱ���
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcTake ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p);

        /*ͨ����ֵ�������������ֵ--ֱ�ӱ���
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcSave ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*ͨ����ֵ�������������ֵ--����ֵ�����õ�˳������
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcCcnt ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p);

        /*ͨ����ֵ�������������ֵ--�ۼ�������
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );

        /*ͨ����ֵ�������������ֵ--���㰴���õ�˳��ƴ�Ӳ��ۼ�������
        * ���룺usrData_p:ͨ�����ݶ�������
        *���룺dtCfg_p�������������ýṹ
        *�����dtValue_p��������������
        */
        int calcCcntAndSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );


        /*ͨ����ֵ�������������ֵ--���㰴����˳�򣨰�λƴ�ӣ�
         * ���룺usrData_p:ͨ�����ݶ�������
         *���룺dtCfg_p�������������ýṹ
         *�����dtValue_p��������������
         */
        int calcCcntAndSumBit ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p );
};

#endif//MC_COUNTMANAGE_H