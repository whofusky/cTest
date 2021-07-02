/******************************************************************************
** �ļ�����unitInfMem.h
** ���ߣ�
** ����ʱ�䣺2018-04-21
** ���ܣ�
** �޸ģ�
** ������
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

#define MC_MAX_PHYNUM  5000    //����һ��ͨ��֧�ֵ��������

#define MC_MAX_CYCLES  1000000   //�������ѭ������,���������ѭ��

//���ձ�Ĵ���
#define MC_ERROR_CYCLES  -1000   //��ʾ�������ѭ����������������
#define MC_ERROR_CALLOC  -999    //calloc����ռ����
#define MC_ERROR_CHNN0   -899    //û���ҵ���Ӧ��ͨ����
#define MC_ERROR_INPAR   -898    //�ӿ���δ���
#define MC_ERROR_PHYTY   -897    //������������phyType����
#define MC_ERROR_PNTAD   -896    //���ַ��������û���ҵ�
#define MC_ERROR_CAUS    -895    //�����ϵ���߼�����:����ǰ����ֵ���ܻ�ȡ�õ�
#define MC_ERROR_OVMAX   -700    //�������õ����ֵ�������ٲ���

//�˴�Ϊ��ʵ����㶨���һ������id�ṹ,ע:��ʽ����ʱ��Ҫ�滻����Ľṹ
//#pragma pack (1)
//typedef struct
//{
//    u_char B_Confidence; //���ݿ��Ŷ�
//    u_char B_Interval;   //���ݵ�ʱ����
//    u_char B_Height;     //���ݵ����ڸ߶�
//    u_char B_Source;     //������Դ
//    u_char B_Kind;       //��������
//    u_char B_Length;     //��������ֽڳ���
//    u_char B_Amount;     //���ֽ�������Ϊ���ݼ�ʱ��Ч���������ݼ���Ԫ�صĸ���
//    u_short  W_Encoding; //���������豸���
//    u_char B_Type;       // ��������
//    u_char B_Catalog;    //���ݾ���Ĵ���࣬�����ļ���sql����������ݼ�
//} DATA_ID;
//#pragma pack()


typedef u_int        PNT_ADDR_TYPE;    //���ַ��������
//typedef DATA_ID      PHY_DATA_ID;       //����id����
typedef T_snnUsrData COM_OBJ_TYPE;      //������Э����ص�ͨ����Ϣ����

//���������Ͷ���
typedef u_int PHY_UNIT_TYPE;

#define MC_INVALID_PHTYPE  0


//������ֵ���㷨
typedef enum
{
    PHY_CALC_INVALID = 0,     //Invalid  //��Чֵ
    //.. ����㷨�ڴ����
    //...
    PHY_CALC_SAVE,             //ֱ�ӱ���
    PHY_CALC_CCNT,             //����ֵ�����õ�˳������
	PHY_CALC_SUM,              //�ۼ�������
	PHY_CALC_CCNT_AND_SUM,     //���㰴���õ�˳��ƴ�Ӳ��ۼ�������
	PHY_CALC_CCNT_AND_BIT,     //���㰴����˳�򣨰�λƴ�ӣ� 
    PHY_CALC_LAST              //��������ڴ�ֵ֮ǰ���,��ֵ��ʾ�㷨�ĸ���
} PHY_CALC_METHD;  //physical quantity, method of calculation


//�������ַ��
typedef struct
{
    //+u_int       addrNumS;    /*�������ַ�ΰ����ĵ��ַ����*/
    PNT_ADDR_TYPE  beginAddrVal;/*��ʼ���ֵַ*/
    PNT_ADDR_TYPE  endAddrVal;  /*�������ֵַ*/
    //u_int        begineIndex; /*��ʼ��ַ�������±�*/
} T_contPntAdd;



//���������ַ�����ýṹ
typedef struct
{
    T_contPntAdd* contPntAdd_p;/*������ַ�α�ͷ*/
    u_int num;                 /*������������ַ�εĶ���*/
} T_pntAddrCfg;


//���������ַ�������ṹ
typedef struct
{
    T_contPntAdd contPntAdd;  /*���ӵ�ַ��*/
    u_int        begineIndex; /*������ַ�ο�ʼֵ������*/
} T_contAddDex;


//��������ַ����ƽṹ
typedef struct
{
    T_contAddDex* conAddDex_p;
    u_int         num;         /*���������ӵ�ַ�εĶ���*/
} T_pntAddrCtl;


//��ʱ��洢��ͨ�ö���ṹ
//typedef struct
//{
//    COM_OBJ_TYPE  relateObj; //����ַ������ͨ�ö���
//    void*         tmStmp_p; //ʱ��
//}T_COM_OBJ_TYPE;


//������ڴ�ֵ�ṹ
typedef struct
{
    //PNT_ADDR_TYPE pntAddr; //���ַ
    //���ַ��Ӧ��������(ע:һ�����ַ��ӦΨһ������)
    PHY_UNIT_TYPE   pntUType;
    COM_OBJ_TYPE    relateObj; //����ַ������ͨ�ö���
} T_pntMemObj;



//��洢����ṹ
typedef struct
{
    //PNT_ADDR_TYPE pntAddr;     //���ַ
    //u_int         objLng;      //��������ĳ���
    //PNT_ADDR_TYPE* pntAddrHead_p;
    T_pntAddrCtl  pntAddrCtl;
    T_pntMemObj* pntMemObj_p;
    void*        tmStmp_p;      //ʱ��
} T_pntStrgObj;



//����������ַ�Ķ�Ӧ��ϵ���ýṹ
typedef struct
{
    PHY_UNIT_TYPE  phyType;
    PHY_CALC_METHD calcMethd; //������ֵ���㷨
    float          multFactor;//�˷�ϵ��
    u_char         phyValLng; //��������Ӧ��ֵ����
    //u_char         phyCatName;//�����������:��˲ʱֵ,1����ƽ����
    //void*        phyObjVal_p;  //������ֵ��ַ
    u_int          numOfAdd;  //��Ӧ���ַ�ĸ���
    PNT_ADDR_TYPE* pntAddr_p; //��Ӧ���ַ��ͷ
    void*          dataId_p;  //����id��ַ
} T_phyValCfg;


//����������ַ�Ķ�Ӧ��ϵ�洢�ṹ
typedef struct T_phyValStrg
{
    PHY_UNIT_TYPE  phyType;
    PHY_CALC_METHD calcMethd;    //������ֵ���㷨
    float          multFactor;   //�˷�ϵ��
    u_char         phyValLng;    //��������Ӧ��ֵ����
    //u_char       phyCatName;   //�����������:��˲ʱֵ,1����ƽ����
    u_char         acmTimes;     //�ۼӴ���
    bool           freshFlag;    //�����Ƿ�����:trueΪ��������
    void*          phyObjVal_p;  //��������Ӧ��ֵ
    void*          dataId_p;     //����idֵ
    u_int          numOfAdd;     //��Ӧ���ַ�ĸ���
    PNT_ADDR_TYPE* pntAddr_p;    //��Ӧ���ַ��ͷ
    //u_int*         addIndex_p; //���ַ��ֵ��Ӧ���ڴ��ַ���������
    T_phyValStrg*  next_p;       //ͬ���ĵ��Ӧ���������ʱ��next_pָ����һ��������
} T_phyValStrg;


//����������ַ�Ķ�Ӧ��ϵ��ȡֵ�ṹ
typedef struct
{
    PHY_UNIT_TYPE  phyType;
    PHY_CALC_METHD calcMethd;   //������ֵ���㷨
    float          multFactor;  //�˷�ϵ��
    u_char         phyValLng;   //��������Ӧ��ֵ����
    u_char         tmStmpLng;   //ʱ�곤��
    //u_char         phyCatName;//�����������:��˲ʱֵ,1����ƽ����
    u_char         acmTimes;     //�ۼӴ���
    bool           freshFlag;    //�����Ƿ�����:trueΪ��������
    void*          phyObjVal_p; //��������Ӧ��ֵ��ַ
    u_int          numOfAdd;    //��Ӧ���ַ�ĸ���
    void*          pntObj_p;    //ÿ�����ַ�����Ķ�����ɵ�����pntObj_p[]
    u_int          pntObjLng;   //ÿ��pntObj_p[i]�ĳ���
} T_phyObjVal;



//�����ۼ�����������ṹ
typedef struct
{
    int            categoryName;//�������:��1����ֵΪ1
    //u_int          maxNum;      //head_p���������
    u_int          curNum;      //head_p��ǰ���õ�����
    PHY_UNIT_TYPE* head_p;
} T_phyCat;


//ͨ��ʱ��ṹ
typedef struct
{
    u_char tmStmpLng; //ʱ�곤��
    void*  tmStmp_p;  //ʱ��
    void*  dataId_p;  //ʱ�������id
    bool   freshFlag; //�����Ƿ�����:trueΪ��������
} T_untChnTmStmp;


//��ʼ��ͨ��������ݽṹ
typedef struct
{
    u_int          chnNum;       //ͨ����
    u_int          pntObjLng;    //���ַ����ͨ�����ݶ���ĳ���
    u_int          dataIdLng;    //����id����
    u_char         tmStmpLng;    //ʱ�곤��
    u_int          pntAddrNum;   //���ַ����
    u_int          phyUnitNum;   //����������
    u_int          maxPhyType;   //һ��ͨ������������������ֵ
    T_pntAddrCfg   pntAddrCfg;   //���ַ�����ýṹ
    T_phyValCfg*   phyObjVal_p;  //���������ýṹ��ͷ
    u_int          numPhyCat;    //phyCat_p[] Ԫ�ظ���
    T_phyCat*      phyCat_p;     //�����ۼ�����������ṹ
    T_untChnTmStmp untChnTmStmp; //ͨ��ʱ��
} T_initChnUnit;




//������ֵ�ṹ
typedef struct
{
    u_int         phyUnitNum;   //����������
    T_phyObjVal*  phyObjVal_p;  //��������ͷ
} T_phyNVal;


//���������ýṹ
typedef struct
{
    u_int         phyUnitNum;   //����������
    T_phyValCfg*  phyValCfg_p;  //���������ñ�ͷ
} T_phyNCfg;


/******************************************************************************/





//ͨ����(������)�ṹ
class C_chnUnitData
{
    public:
        C_chnUnitData();
        ~C_chnUnitData();

    public:
        //��ʼ��
        int initM ( const T_initChnUnit* inPar_p );

        /*����outData_p->phyTypeȡ�ض�������������ֵ
            ���:outData_p->phyType ��Ҫ���ýӿ�ǰ��ֵ
            ����:outData_p������ֵ
        */
        int getPhyCfgByType ( T_phyValCfg* outData_p
                            ) const;

        /*���ݵ��ַȡ������������ֵ
            ���:pntAddVal_p
                 outInNum ����outData_p�Ĵ�С
            ����:outData_p
                 outInNum ������ʵ�ʸ�ֵ�ĸ���
            ����ֵ:
                0: �ɹ�
                <0:ʧ��
                >0:ʵ��������������������������
        */
        int getPhyCfgByPnt ( const PNT_ADDR_TYPE* pntAddVal_p,
                             T_phyValCfg outData_p[],
                             u_int&  outInNum
                           ) const;

        /*���ݶ�����ַȡ������������(����ͬ�������Ѿ�ȥ����:
          ������ͬһ��������ֻ����һ��)
            ���:pntAddVal_pp
            ����:outNum_p   ������������
                 outPhyType_p (��ѡ)���˲�����Ϊ��ʱ���������������
        */
        int getPhyNumByNPnt ( COM_OBJ_TYPE* pntAddVal_pp[],
                              const u_int& numPnt,
                              u_int*  outNum_p,
                              PHY_UNIT_TYPE outPhyType_p[] = NULL
                            ) const;

        /*���ݶ�����ַȡ������������ֵ(����ͬ�������Ѿ�ȥ����:
          ������ͬһ��������ֻ����һ��)
            ���:pntAddVal_pp
            ����:outData_p
                 outNum_p   ����T_phyValCfg������
        */
        int getPhyCfgByNPnt ( COM_OBJ_TYPE* pntAddVal_pp[],
                              const u_int& numPnt,
                              T_phyValCfg outData_p[],
                              u_int*  outNum_p
                            ) const;

        //�������������ֵ
        int setM ( const T_phyNVal* inPar_p,
                   const u_char limitFlag = 0
                 );

        //����ͨ��ʱ��
        int setChnTmStmp ( const void* tmStmp_p );

        /*��ȡ���������ֵ
            ���:inPar_p->phyUnitNum              Ҫ��ȡ����������
                 inPar_p->phyObjVal_p[i].phyType  Ҫ��ȡ������������
            ����:inPar_p������ֵ
        */
        int getM ( T_phyNVal* inPar_p );

        //��ȡͨ��ʱ��
        int getChnTmStmp ( T_untChnTmStmp* chnTmStmp_p );

        /*��ȡ��������ֵ������Id
          ���г���,����Ҫ��,ֱ�Ӹ����
          ���:phyType[]
               num
               onePhyVsize ÿ��phyObjVal[]��Ա����Ŀռ��С
          ����:oPhyObjVal_p
               oPhyValLng[] ÿ���������ĳ��ȿ��ܲ�һ��,����������
               oDataId_p
               oDataIdLng   ÿ��������������id������һ����,������һ��ֵ��ʾ
        */
        int getPhyValADid ( const PHY_UNIT_TYPE phyType[],/*����������*/
                            const u_char& num,             /*����������*/
                            const u_char& onePhyVsize,
                            void*  oPhyObjVal_p,/*phyObjVal[],��������ֵ*/
                            u_char oPhyValLng[],/*ÿ��������ֵ�ĳ���*/
                            void*  oDataId_p,   /*dataId[],��������Ӧ������id*/
                            u_int* oDataIdLng_p   /*����id�ĳ���*/
                          );

        /*��ȡ������ֵ�ĳ���*/
        int getPhyValLng ( const PHY_UNIT_TYPE phyType[], /*����������*/
                           const u_char& num,
                           u_char oPhyValLng[]/*ÿ��������ֵ�ĳ���*/
                         ) const;

        //��ȡ����������buf�ĵ�ַ:��buf�д������Ӧ�����������������(������������)
        const  PHY_UNIT_TYPE* getPhyCatBuf ( const int& phyCatName,/*�����������*/
                                             u_int& bufLng          /*����������buf����*/
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
        //��ʼ�������ۼ����������ṹ
        int initPhyCat ( const u_int& numPhyCat,
                         const T_phyCat*     phyCat_p
                       );
        //���ݵ��ַ�ҵ��������±�����
        int findPntIndx ( const PNT_ADDR_TYPE& value,
                          u_int& retIndx
                        ) const ;

    public:
        u_int         m_chnNum;       //ͨ����

    private:
        u_int          m_dataIdLng;    //����id����
        u_char         m_tmStmpLng;    //ʱ�곤��

        T_pntStrgObj   m_pntStrgObj;   //���ַ��ͷ
        u_int          m_pntAddrNum;   //���ַ����
        u_int          m_pntObjLng;    //���ַ����ͨ�����ݶ���ĳ���

        T_phyValStrg*  m_phyToPnt_p;   //��������ͷ
        u_int          m_phyUnitNum;   //����������
        u_int          m_maxPhyType;   //���������������ֵ

        T_phyCat*      m_phyCat_p;     //�����ۼ�����������ṹ
        u_int          m_numPhyCat;    //m_phyCat_p[]Ԫ�ظ���

        T_untChnTmStmp m_untChnTmStmp; //ͨ��ʱ��

    private:
        nsCPLock::CLock m_valLock;     //������ȡֵ������
        nsCPLock::CLock m_tmStmpLock;  //ͨ��ʱ�껥����

};





//��������Ϣ����/(��ͨ����(������)�ṹ)
class C_unitInfData
{

    public:
        C_unitInfData();
        C_unitInfData ( const u_int& chnNumS  /*ͨ������*/
                      );
        ~C_unitInfData();


    public:/*****************************��ʼ��******************************/
        //һ�γ�ʼ������ͨ��
        int initM ( const u_int& chnNumS,         /*ͨ������*/
                    const T_initChnUnit* inPar_p  /*T_initChnUnit[]����*/
                  );
        //����ͨ����ʼ��
        int initM ( const T_initChnUnit* inPar_p /*T_initChnUnitָ��*/
                  );


    public:/**********************get��Ӧ������ֵ*****************************/
        /*����outData_p->phyTypeȡĳ��ͨ�����ض�������������ֵ
            ���:chnNum ͨ�����
                outData_p->phyType ��Ҫ���ýӿ�ǰ��ֵ
            ����:outData_p������ֵ
        */
        int getPhyCfgByType ( const u_int& chnNum,
                              T_phyValCfg* outData_p ) const;


        /*���ݵ��ַȡĳ��ͨ��������������ֵ
            ���:chnNum ͨ�����
                pntAddVal_p ���ַ
               outInNum ����outData_p�Ĵ�С
            ����:outData_p
                 outInNum ������ʵ�ʸ�ֵ�ĸ���
            ����ֵ:
                0: �ɹ�
                <0:ʧ��
                >0:ʵ��������������������������
            ����:outData_p
        */
        int getPhyCfgByPnt ( const u_int& chnNum,
                             const PNT_ADDR_TYPE* pntAddVal_p,
                             T_phyValCfg outData_p[],
                             u_int&  outInNum
                           ) const;

        /*���ݶ�����ַȡ������������(����ͬ�������Ѿ�ȥ����:
                  ������ͬһ��������ֻ����һ��)
                  ���:chnNum ͨ�����
                       pntAddVal_pp
                  ����:outNum_p   ������������
                       outPhyType_p (��ѡ)���˲�����Ϊ��ʱ���������������
         */
        int getPhyNumByNPnt (  const u_int& chnNum,
                               COM_OBJ_TYPE* pntAddVal_pp[],
                               const u_int& numPnt,
                               u_int*  outNum_p,
                               PHY_UNIT_TYPE outPhyType_p[] = NULL
                            ) const;

        /*���ݶ�����ַȡ������������ֵ(����ͬ�������Ѿ�ȥ����:
        ������ͬһ��������ֻ����һ��)
        ���:chnNum ͨ�����
             pntAddVal_pp
        ����:outData_p
            outNum_p   ����T_phyValCfg������
        */
        int getPhyCfgByNPnt ( const u_int& chnNum,
                              COM_OBJ_TYPE* pntAddVal_pp[],
                              const u_int& numPnt,
                              T_phyValCfg outData_p[],
                              u_int*  outNum_p
                            ) const;

        /*��ȡͨ���������Ѿ���ʼ��������*/
        void getChnNum ( u_int* chnNumS_p, /*ͨ������*/
                         u_int* curNumS_p  /*�Ѿ���ʼ��������*/
                       ) const;
        /*��ȡ������ֵ�ĳ���*/
        int getPhyValLng ( const u_int& chnNum,           /*ͨ����*/
                           const PHY_UNIT_TYPE phyType[], /*����������*/
                           const u_char& num,
                           u_char oPhyValLng[]/*ÿ��������ֵ�ĳ���*/
                         ) const;


    public:/**********************get��set��Ӧ��ֵ*****************************/
        //�������������ֵ
        int setM ( const u_int& chnNum,            /*��Ҫ��ֵ��ͨ����*/
                   const T_phyNVal* inPar_p
                 );

        /*��ȡ���������ֵ
        ���:valChnNum
            cfgChnNum
            valNData_p->phyUnitNum              Ҫ��ȡ����������
            valNData_p->phyObjVal_p[i].phyType  Ҫ��ȡ������������
            cfgNPhyCfg_p->phyUnitNum             Ҫ��ȡ����������
            cfgNPhyCfg_p->phyValCfg_p[i].phyType Ҫ��ȡ������������
        ����:valNData_p������ֵ
            cfgNPhyCfg_p������ֵ
        */
        int getM ( const u_int& valChnNum, /*Ҫȡֵ��ͨ����*/
                   const u_int& cfgChnNum,  /*Ҫȡ���õ�ͨ����*/
                   T_phyNVal*   valNData_p,/*������ֵ�ṹ*/
                   T_phyNCfg*   cfgNPhyCfg_p/*���������ýṹ*/
                 ) const;

        //����ͨ��ʱ��
        int setChnTmStmp ( const u_int& chnNum,      /*��Ҫ��ֵ��ͨ����*/
                           const void* tmStmp_p
                         );
        //��ȡͨ��ʱ��
        int getChnTmStmp ( const u_int& fromChnNum, /*Ҫ���ĸ�ͨ����ȡֵ*/
                           T_untChnTmStmp* chnTmStmp_p
                         );

        /*��ȡ��������ֵ������Id
          ���г���,����Ҫ��,ֱ�Ӹ����
          ���:chnNum
               phyType[]
               num
               onePhyVsize ÿ��phyObjVal[]��Ա����Ŀռ��С
          ����:oPhyObjVal_p
               oPhyValLng[] ÿ���������ĳ��ȿ��ܲ�һ��,����������
               oDataId_p
               oDataIdLng   ÿ��������������id������һ����,������һ��ֵ��ʾ
        */
        int getPhyValADid ( const u_int& chnNum,           /*ͨ����*/
                            const PHY_UNIT_TYPE phyType[],/*����������*/
                            const u_char& num,             /*����������*/
                            const u_char& onePhyVsize,
                            void*  oPhyObjVal_p,/*phyObjVal[],��������ֵ*/
                            u_char oPhyValLng[],/*ÿ��������ֵ�ĳ���*/
                            void*  oDataId_p,   /*dataId[],��������Ӧ������id*/
                            u_int* oDataIdLng_p   /*����id�ĳ���*/
                          ) const;

        //��ȡ����������buf�ĵ�ַ:��buf�д������Ӧ�����������������(������������)
        const  PHY_UNIT_TYPE* getPhyCatBuf ( const u_int& chnNum,    /*ͨ����*/
                                             const int& phyCatName, /*�����������*/
                                             u_int& bufLng          /*����������buf����*/
                                           ) const;

    private:
        int findChnIndx ( const u_int& chnNum ) const;

    private:
        void freeM ( void );

    private:
        u_int          m_chnNumS;       //ͨ������
        u_int          m_curNumS;       //��ǰ�Ѿ���ʼ����ͨ������
        C_chnUnitData* m_chnUnitHead_p; //ͨ����ͷ


};




#endif //MC_UNITINFMEM_H