#ifndef APDU1_H
#define APDU1_H
#include "MACRO.h"
#include "DataType.h"




typedef struct
{
    u_char SYS_ELENum;    //����
    ELE_TYPE* SYS_TYPE_p; //δ֪Ԫ�ظ���������
} DUT_INF;


typedef DUT_INF ASDU_DUT;
typedef DUT_INF ASDU_DUID;
typedef DUT_INF INF_BODY;
typedef DUT_INF APDU;


/*
    APDU_DSCR.InfoEleLng ����µĺ��� @20180222161842
    һ���ڷ���ռ�ʱΪ��ȷ������Ŀռ��㹻��
    1�����ɱ�ṹ�޶��ʵ�SQ = 1 ��������ʱ��
       InfoEleLng��ʾ��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ���
    2�����ɱ�ṹ�޶��ʵ�SQ = 0 �������ʱ��
       InfoEleLng ��ʾÿ����Ϣ������ֽڳ���
    ������ʵ��ʹ��APDU�Ľṹʱ������SQ = 1 �� 0
    InfoEleLng��ʾ��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ���
*/
typedef struct
{
    u_int   ASDU_TID;
    u_int   InfoEleLng;//��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ���
    u_short ASDU_CMTBL [11];
} APDU_DSCR;




/*�����ŵİ�λλ��ֵ*/
class BS8s_Ss30
{
    private:
        u_char num;//��������λ��9λλ���

        BS8* values_p;

        u_char signs;

    public:
        BS8s_Ss30 ( u_char num );

        ~BS8s_Ss30();

        //����λ�����
        u_char getNumber();

        /*bitsNoΪ�����������ʾҪ��ȡ��λ���1~8��0��ʾ��ȡ����λ�飻
          bitsNumΪ������������ӷ���λ��9λλ�������
          ����ֵ��8λֵ��ȣ�����1������λ��9λλ��ֵ��
        */
        BS8_S getValues ( u_char bitsNo );

        /*bitsNoΪ�����������ʾҪ���õ�λ���1~8��0��ʾ��������λ�飻
          bitsNumΪ�����������ʾҪ���õĸ��ӷ���λ9λλ�������
          n9BitsValueΪ�������������ֵ�ǰ���bitsNum��9λλ��Ԫ�ص����飻
          ����ֵ�����óɹ�����״̬��
        */
        u_int setValues ( u_char bitsNo, BS8_S* n9BitsValue_p );

};




class INFO_QUAL1
{
    private:
        u_char num;    //��Ϣ��洢ռ���ֽ���
        BS8* value_p;  //��Ϣ���ַ
        QUAL1* qual_p; //Ʒ��������

    public:
        INFO_QUAL1();

        ~ INFO_QUAL1 ();

        /* bodyWidthΪ�����������ʾ��Ϣ���ֽ�����
           ����ֵ����Ϣ��ָ�롣
        */
        BS8* getValue ( u_char* bodyWidth_p );

        //��ȡƷ��������
        QUAL1* getQual();

        /*������Ϣ��
          infoValue Ϊ�����������ʾ��Ϣ���ַ
          bodyWidth Ϊ�����������ʾ��Ϣ���ֽڳ���
        */
        u_int setValue ( BS8* infoValue_p, u_char* bodyWidth_p );

        /*������Ϣ��Ʒ��������
          infoQual Ϊ�����������ʾƷ�������ʵ�ַ
        */
        u_int setQual ( QUAL1* infoQual_p );
};




class COUNTER1
{
    private:
        u_char num;         //�������ֽڿ��
        u_int value;        //��32λ�������ֵ����Ч�����num��ȷ������Чֵ�͵�ַ����
        CNTR_STATE cntrSeq; //������˳���ǣ���¼�˼�������ʶ��״̬

    public:
        u_char cntrType;    //���������ͣ����궨��

    public:
        COUNTER1 ( u_char cntrType, u_char num );
        ~ COUNTER1();

        /* cntrWidthΪ�����������ʾ�����������Ĵ����ֽڿ�ȣ�
           ����ֵ�������������Ĵ���ֵ��ʵ�ʿ�����������ȷ����
           ��Чֵ�͵�ַ���ȡ�
        */
        u_int getValue ( u_char* cntrType_p, u_char* cntrWidth_p );
        u_int setValue ( u_char cntrType, u_int cntrValue, u_char cntrWidth );

        CNTR_STATE* getSeq ( u_char* cntrType_p );
        u_int setSeq ( CNTR_STATE cntrSeq );
};




class APDU1
{
    private:
        u_short DU_Lng; //���ݵ�Ԫ���ȳ���
        u_char DUType;  //ָ������ռ����;��Ϊ0ʱAPDU��Ч��1ʱASDU��Ч��2ʱ���߶���Ч

    protected:
        u_char allocFlag;
        u_char* APDU_p;     //APDU�׵�ַ
        u_char* ASDU_p;     //ASDU�׵�ַ
        APDU_DSCR DU_Info;  //APDU����
        u_char asduVsqVal;  //�ɱ�ṹ���޶��ʵ�ֵ:���ݴ�ֵ�ж���ν��װ��Ϣ�����

    public:

        /*ʹ�ö�����Ϣ��ʼ����ԱAPDU_Info������APDU�ܳ��ȣ���
          ��APDU_Lng��Ա�ͷŲ���ָ��ĵ�ַ�ռ�
          allocFlag!=0ʱ������APDU�洢�ռ䣬�ô洢�ռ��׵�ַ
         ��ָ�룩����APDU��Ա���򲻷���APDU�洢�ռ䣬��ԱAPDU
          =NULL
        */
        APDU1 ( APDU_DSCR* cstmInfo, u_char duType, u_char  allocFlag );
        ~APDU1();//�ͷŷ���ռ�

        APDU_DSCR* getDU_Info();

        /*���º�����������APDU��ô洢λ�ã�����洢������ֵ
          ��ȡ������APDU_Info.ASDU_CMTBL��δ���õĻ���Ԫ��
          ʱ������NULL��0��ʧ�ܣ�
          index ��Ϊ0ʱһ������ASDU�ж����ʱָ������ȡ�Ķ���
          ����
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

        //���APDU���ܳ���
        u_short getDU_Lng ();

        //��ȡ��ǰ���д���Ļ���������
        u_char getDUType();

        //���APDU�洢�ռ���׵�ַָ��
        u_char* getDU ( char duType );

        //���ASDU�洢�ռ���׵�ַָ��
        u_char* getDU ( u_short& apduLng, char duType );

        /*���·������ڰ���װ��ṹ�������Ԫ�أ�
          ����Ϊ���ص���װ�����µĵ�ַ�ռ��
          �����ú�Ҫ���ö�Ӧ���ͷŷ���
        */

        //��ȡ���������ݵ�Ԫ����
        ASDU_DUT*    getASDU_DUT();
        u_int freeASDU_DUT ( ASDU_DUT* pSpace );

        //��ȡ���������ݵ�Ԫ��ʶ��
        ASDU_DUID*   getASDU_DUID();
        u_int freeASDU_DUID ( ASDU_DUID* pSpace );

        //��ȡ��������Ϣ�壨����
        INF_BODY*    getINF_BODY ( u_char index = 0 );
        u_int freeINF_BODY ( INF_BODY* pSpace );

        u_int freeAPDU ( INF_BODY* pSpace );

        //��һ��洢�ռ�󶨵������ϣ����ó�ԱAPDU=memBlock�����ڽ���
        u_int bindToMem ( u_char* memBlock_p, u_char duType, u_short APDU_Lng );
};




#endif // APDU1_H