
#pragma once

#ifndef MACRO_H
#define MACRO_H




/*****************************************MODBUS*******************************/
#define REVERS_FLAG             0   //��С���Ƿ�ת����0 ת��  1��ת��
#define ADU_CMTBL_NUM           5   //ADU����������

#define ADU_TYPE                1   //ADU����Ч��ʶ
#define PDU_TYPE                0   //PDU����Ч��ʶ

#define MAX_ARRAY_INDEX         255 //һ���ַ���������±�ֵ

/***********************MODBUS ADUϵͳ���********************/
#define ADU_MBAPH_INDEX         0   //ADU MBAP����ͷ
#define ADU_ADDR_INDEX          1   //ADU ��ַ��
#define PDU_FC_INDEX            2   //PDU������
#define PDU_DATA_INDEX          3   //PDU����
#define ADU_ECHK_INDEX          4   //ADU���У��
/************************************************************/

#define MODBUS_TID              1  //Ϊmodbus���������tid
#define MODBUS_COT_ACT          6  //Ϊmodbus���������cot
#define MODBUS_COT_ACK          7  //Ϊmodbus���������cot
/*************����Э�����************************************/
#define P_MODBUS_TCP         0x6a  //tcp Ϊż��������Ϊ����

/***************************MODBUS֡**************************/
//�Դ���·ͨ����˵ MODBUS PDU= 256 - ��������ַ��1�ֽڣ�- CRC(2�ֽ�)
#define MPDUMAX 253
//TCP MODBUS ADU= 253 + MBAP(7�ֽ�)
#define MBAPMAX 260

#define MB_REQ_F                0       //MODBUS ���� ֡
#define MB_RSP_F                1       //MODBUS ��Ӧ ֡
#define MB_EXCEP_F              3       //MODBUS �쳣��Ӧ ֡
#define UNKNOW_F                2

/*********************MODBUS��Ȧ״ֵ̬����*********************/
#define MC_COIL_STATUS_ON       1       //��Ȧ״̬ONֵ
#define MC_COIL_STATUS_OFF      0       //��Ȧ״̬OFFֵ


/********************T_snnUsrData�ṹ�д����������************/
#define PNTTYPE_ADDR            1      //��ʼ��ַ
#define PNTTYPE_NUM             2      //����д������
#define PNTTYPE_BITE_NUM        3      //�ֽ���
#define PNTTYPE_VALUE           4      //���ֵ��д��ֵ
#define PNTTYPE_MASK            5      //And_Mask��Or_Maskֵ
#define PNTTYPE_READ_REG        6      //���Ĵ���
#define PNTTYPE_WITE_REG        7      //д�Ĵ���



/**********************֡��ʽ**********************************/
#define PROTOCOL_MODBUS     0x0000  //MODBUSЭ���ʶ�ַ�

#define MODBUS_ASCII        0x00    //MODBUSЭ�����ASCII����֡��ʽ
#define MODBUS_RTU          0x01    //MODBUSЭ�����RTU����֡��ʽ
#define MODBUS_TCP          0x02    //MODBUSЭ�����TCP����֡��ʽ



///////////  �ײ���ղ�ADU������������      ///////////////
#define  MC_ADU_OK          0  // ADU���ݽ����ɹ�
#define  MC_UNKNOWN_MODE    1  // δ����MODBUSЭ���֡��ʽ
#define  MC_NATIVE_WARNING  2  // ���س�ʱ��֪����
#define  MC_REMOTE_FLOWCTRL 3  // Զ�̴������Ҫ����������
#define  MC_NATIVE_FLOWCTRL 4  // ����Ҫ����������
#define  MC_APDU_DISORDER   5  // APDU֡ʧ�����
#define  MC_APDU_CHECKSUM   6  // APDU֡У��ʧ��
#define  MC_UNKNOWN_MODBUS  7  // �޷�ʶ���֡


////////MODBUS �����붨��///////////////////////////////////
#define  MC_MBFC_0x01      0x01 //������01������Ȧ
#define  MC_MBFC_0x02      0x02 //������02������ɢ������ 
#define  MC_MBFC_0x03      0x03 //������03�������ּĴ���
#define  MC_MBFC_0x04      0x04 //������04��������Ĵ���
#define  MC_MBFC_0x05      0x05 //������05��д������Ȧ
#define  MC_MBFC_0x06      0x06 //������06��д�����Ĵ���
#define  MC_MBFC_0x0F      0x0F //������15��д�����Ȧ
#define  MC_MBFC_0x10      0x10 //������16��д����Ĵ���
#define  MC_MBFC_0x14      0x14 //������20�����ļ���¼
#define  MC_MBFC_0x15      0x15 //������21��д�ļ���¼
#define  MC_MBFC_0x16      0x16 //������22������д�Ĵ��� 
#define  MC_MBFC_0x17      0x17 //������23����д����Ĵ��� 
#define  MC_MBFC_0x2B      0x2B //������43�����豸ʶ���� 
#define  MC_MBFC_0xFE      0xFE //�Զ�����չ������0:����һ��ͨ��
#define  MC_MBFC_0xFF      0xFF //�Զ�����չ������255:��һ��ͨ�� 

//�жϹ������Ƿ��ڶ���ķ�Χ��
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






#define BIN_CNTR1               1   //�����Ƽ���������1
#define BIP_CNTR1               2   //˫���Զ����Ƽ���������1
#define BCD_CNTR1               3   //BCD����������1

#define APDU_TYPE               0   //APDU����Ч��ʶ
#define ASDU_TYPE               1   //ASDU����Ч��ʶ
#define APDUANDASDU_TYPE        2   //APDU��ASDU����Ч��ʶ




/***********************APDUϵͳ���************************/
#define APDU_APCI_INDEX         0   //Ӧ�ù�Լ���ƽӿ�
#define ASDU_TID_INDEX          1   //���ͱ�ʶ
#define ASDU_LNG_INDEX          2   //ASDU����
#define ASDU_VSQ_INDEX          3   //�ɱ�ṹ���޶���
#define ASDU_COT_INDEX          4   //����ԭ��
#define ASDU_ADDR_INDEX         5   //ASDU������ַ
#define BODY_TID_INDEX          6   //��Ϣ������
#define BODY_ADDR_INDEX         7   //��Ϣ���ַ
#define BODY_SET_INDEX          8   //��ϢԪ�ؼ�
#define BODY_TSP_INDEX          9   //��Ϣ��ʱ��
#define ASDU_TSP_INDEX          10  //ASDUʱ��
/************************************************************/




/* ����Ϊ��value��indexλ����Ϊ0��1*/
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




//�ж�tid�Ƿ�Ϊ <22...29>���ݵ�tid
#define CCIFTID(tmpTID) (tmpTID >= MC_TID_22 && tmpTID <= MC_TID_29)


//104��ϢԪ�س��ȳ���4�ֽڵ�tid
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




//��Ϣ��������
#define GINFOTYPE    0    //ͨ����Ϣ����
#define FINFOTYPE    1    //�ļ���Ϣ����

//��Ϣ����ʱ������
#define TIME_TYPE_56_2A 104 //104��ԼĬ��ʱ�����ͣ�ʱ��CP56Time2a
#define TIME_TYPE_24_2A 101 //101Ĭ��ʱ�����ͣ�ʱ��CP56TIME2a




/***************************104֡***************************/
#define APDUMAX 255
#define I_F             0       //I֡
#define S_F             1       //S֡
#define U_F             3       //U֡
#define UNKNOW_F        2

#define U_SRTDT_ACT     0x04    //U֡STARTDT����
#define U_SRTDT_CON     0x08    //U֡STARTDTȷ��
#define U_STPDT_ACT     0x10    //U֡STOPDT����
#define U_STPDT_CON     0x20    //U֡STOPDTȷ��
#define U_TST_ACT       0x40    //U֡TESTFR����
#define U_TST_CON       0x80    //U֡TESTFRȷ��

#define PNTTYPE_NULL_SET   1    //��Ϣ����û����ϢԪ�ؼ�
#define PNTTYPE_HIGH_SET   2    //��Ϣ�����ֵ����u_int��ʾ��Χ,��Ҫ��ֵ�ĸ�4�ֽ�




/***************************************************************************/
/*****************************������Ϣ*************************************/
#define CHILD_STAND_NUM 2
/**************************************************************************/
#define ACTIVE_TYPE     0   //����
#define PASSIVE_TYPE    1   //����

/**Э�����ݵķ�����**/
#define MC_CONTROL_DIRECTION 0 //���Ʒ���
#define MC_MONITOR_DIRECTION 1 //���ӷ���

/**ͻ��**/
#define MC_OUTBREAK_DIRECTION 1 //ͻ������


/**************************����Э�����************************************/
#define P_101               0x01
#define P_102               0x02
#define P_103               0x03
#define P_104               0x04
/*********************104Э�����֡����*************************************/
#define MAX104_APDU_LNG     255
#define MAX104_ASDU_LNG     249
#define MAX104_VSQ_N        2   //һ֡�����ĵ㼯����
/********************************֡��ʽ************************************/
#define F1_2                2
#define UNDEFINE            0xFF
#define SRTCHAR_F1_1        UNDEFINE
#define SGLCTLCHAR_F1_1     0x0 // SGLCTLCHAR:�������ַ�
#define SRTCHAR_F1_2_V      0x68    //F1_2��FT1.2��V���䳤��SRTCHAR��ʼ�ַ�
#define SRTCHAR_F1_2_F      0x10    //F������
#define SGLCTLCHAR1_F1_2    0xE5    // SGLCTLCHAR1:�������ַ�1
#define SGLCTLCHAR2_F1_2    0xA2    // SGLCTLCHAR2:�������ַ�2
#define ENDCHAR_F1_2        0x16        //ENDCHAR:�����ַ�
#define SRTCHAR_F2          0x27
#define SGLCTLCHAR_F2       0x14
#define SRTCHAR_F3          0x0564
#define DBLCTLCHAR_F3       0x123D  //DBLCTLCHAR: ˫�����ַ��������ַ�2

/*******************************���ܱ���**************************************/

#define STN_INIT        1       //վ��ʼ��
#define QUE_DATA        2       //��ѯ����
#define CYC_TRNS        3       //���ڴ���
#define ACQ_EVT         4       //�¼��ռ�
#define QUKCHK_EVT      5       //����¼�
#define STN_INRGTN      6       //վ�ٻ�
#define CLK_SYN         7       //ʱ��ͬ��
#define CMD_TRNS        8       //�����
#define ACMU_TRNS       9       //�ۻ�������
#define PARA_LD         10      //��������
#define TST_PRCD        11      //���Թ���
#define FILE_TRNS       12      //�ļ�����
#define ACQ_TRNSDLY     13      //�ռ������ӳ�ֵ
#define ACMU_FRZ        14      //�ۻ�������ǰ�Ķ���    �˹����Ǵ�CMD_TRNS���ܵ�
//ģʽ2������վ�ȶ�������������������
#define U_STARTDT       15      //104Э��U֡STARTDT ����ĻỰ
#define U_STOPDT        16      //104Э��U֡STOPDT ����ĻỰ
#define U_TESTFR        17      //104Э��U֡TESTFR ����ĻỰ
#define U_NEWHOLD       0xFE    //��U֡��STARTDT STOPDT TESTFR ȫΪ0 ��ʾ���µ�
//�Ự������һ��ͨ��
#define U_DELHOLD       0xFF    //��U֡��STARTDT STOPDT TESTFR ȫΪ1 ��ʾ���µ�
//�Ự����һ��ͨ��




/*****************************ģʽ��******************************************/
#define MAX_MODE_N  4       //ģʽ���������

#define AUTO        254
#define UNKNOWN     255
#define ONLY_ONE    0       //Ψһģʽ

//�ò�ѯ��ʽ�ɼ�����
#define POLL_EVT    0       //�˷�ʽ���ܲ�����������ͨ�Ź���
#define POLL_DATA   1       //�˷�ʽ�ɲ����¼�ͻ������
#define RD_DATA     2

//ʱ��ͬ��
#define CRCT_REQ    0       //����Уʱ
#define CRCT_BC     1       //�㲥Уʱ

//�����
#define CMD_SE      0       //ѡ��ģʽ������ִ�У����ɹ��Ự����ѡ��ģʽ
#define CMD_DE      1       //ִ��ģʽ���Ự��ѡ���ֱ��ִ��ģʽ�¾���ִ��
#define CMD_CNCL    2       //������ʽ���Ự����ѡ��ģʽ�·���ִ��

//�ۻ�������
#define CTLD_DRCTSONPT_TRNS        0    //ģʽ0������վֱ��ͻ������
#define CTL_DRCTREQ_TRNS           1    //ģʽ1������վֱ��������
#define CTL_FRZREQ_TRNS            2    //ģʽ2������վ�ȶ�����������
#define CTL_FRZ_CTLD_SONPT_TRNS    3    //ģʽ3������վ����󱻿�վͻ������

//װ�ز���
#define ONLY_LD         0       //ֻ���أ�ģʽ0
#define ONLYACT_LD      1       //ֻ���ģʽ1
#define AUTOACT_LD      2       //�Զ����ģʽ2




/**************************ͨ�Ų����*******************************************/
#define PHY_LVL         0x01
#define LNK_LVL         0x02
#define NAS_LVL         0x04            //������ʲ�
#define LNK_NAS_LVL     0x06

#define APP_LVL         0x08
#define LNK_APP_LVL     0x82        //=(APP_LVL<<4)|LNK_LVL
#define NAS_APP_LVL     0x83        //=(APP_LVL<<4)|NAS_LVL
#define LNK_NAS_APP_LVL 0x86        //=(APP_LVL<<4)|LNK_NAS_LVL
#define UNKNOWN_LVL     0x00

/*************************/
#define MC_SEND_WAIT_CHECK       0x11           // ���͵ȴ�����
#define MC_SEND_CNUE_EXEC        0x12           // ��������ִ��
#define MC_SEND_SSN_RETRY        0x13           // ���ͻỰ����
#define MC_SEND_SSN_FNSH         0x16           // ���ͻỰ���

#define MC_NO_SEND_WAIT_CHECK    0x01       // �����͵ȴ�����
#define MC_NO_SEND_CNUE_EXEC     0x02       // ����������ִ��
#define MC_NO_SEND_SSN_STOP      0x04       // �����ͻỰ��ֹ
#define MC_NO_SEND_SSN_FNSH      0x06       // �����ͻỰ���




/*********************************************************************/
/*                �����ڻ�������101��104��Լ���ж���               */
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
//0 δ��
#define MC_COT_1         1  //���ڡ�ѭ��;
#define MC_COT_2         2  //����ɨ��
#define MC_COT_3         3  //ͻ��(�Է�);
#define MC_COT_4         4  //��ʼ��
#define MC_COT_5         5  //������߱�����
#define MC_COT_6         6  //����վ�ٻ�;
#define MC_COT_7         7  //ȷ�ϼ���վ�ٻ�;
#define MC_COT_8         8  //ֹͣ����;
#define MC_COT_9         9  //ֹͣ����ȷ��
#define MC_COT_10        10 //������ֹ
#define MC_COT_11        11 //Զ����������ķ�����Ϣ
#define MC_COT_12        12 //������������ķ�����Ϣ
#define MC_COT_13        13 //�ļ�����
/*14...19  Ϊ���ױ�׼���ݷ�Χ����*/
#define MC_COT_14        14
#define MC_COT_15        15
#define MC_COT_16        16
#define MC_COT_17        17
#define MC_COT_18        18
#define MC_COT_19        19
#define MC_COT_20        20 //��Ӧվ�ٻ�;
#define MC_COT_21        21 //��Ӧ��1 ���ٻ�;
#define MC_COT_22        22  //��Ӧ��2 ���ٻ�
#define MC_COT_23        22  //��Ӧ��3 ���ٻ�
#define MC_COT_24        22  //��Ӧ��4 ���ٻ�
#define MC_COT_25        22  //��Ӧ��5 ���ٻ�
#define MC_COT_26        22  //��Ӧ��6 ���ٻ�
#define MC_COT_27        22  //��Ӧ��7 ���ٻ�
#define MC_COT_28        22  //��Ӧ��8 ���ٻ�
#define MC_COT_29        22  //��Ӧ��9 ���ٻ�
#define MC_COT_30        22  //��Ӧ��10���ٻ�
#define MC_COT_31        22  //��Ӧ��11���ٻ�
#define MC_COT_32        22  //��Ӧ��12���ٻ�
#define MC_COT_33        22  //��Ӧ��13���ٻ�
#define MC_COT_34        22  //��Ӧ��14���ٻ�
#define MC_COT_35        22  //��Ӧ��15���ٻ�
#define MC_COT_36        36 //��Ӧ��16���ٻ�;
#define MC_COT_37        37  //��Ӧ���������ۼ�����վ���ܣ��ٻ�
#define MC_COT_38        38  //��Ӧ��1����������ۼ������ٻ�
#define MC_COT_39        39  //��Ӧ��1����������ۼ������ٻ�
#define MC_COT_40        40  //��Ӧ��1����������ۼ������ٻ�
#define MC_COT_41        41  //��Ӧ��1����������ۼ������ٻ�
/*42...43  Ϊ���ױ�׼���ݷ�Χ����*/
#define MC_COT_41        41
#define MC_COT_42        42
#define MC_COT_43        43
#define MC_COT_44        44  //δ֪�����ͱ�ʶ
#define MC_COT_45        45  //δ֪�Ĵ���ԭ��
#define MC_COT_46        46  //δ֪��Ӧ�÷������ݵ�Ԫ������ַ
#define MC_COT_47        47  //δ֪����Ϣ�����ַ
/*48...63  ����Ӧ�ñ�����ר�÷�Χ��*/
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
//���ͱ�ʶ=UI8[1...8] <0...127>
//�ڼ��ӷ����ϵĹ�����Ϣ
//0 δ����
#define MC_TID_1         1     //������Ϣ
#define MC_TID_2         2     //��ʱ��ĵ�����Ϣ
#define MC_TID_3         3     //˫����Ϣ
#define MC_TID_4         4     //��ʱ���˫����Ϣ
#define MC_TID_5         5     //��λ�ã���λ����Ϣ
#define MC_TID_6         6     //��ʱ��Ĳ�λ�ã���λ����Ϣ
#define MC_TID_7         7     //32���ش�
#define MC_TID_8         8     //��ʱ���32���ش�
#define MC_TID_9         9     //����ֵ����һ��ֵ
#define MC_TID_10        10    //����ֵ����ʱ��Ĺ�һ��ֵ
#define MC_TID_11        11    //����ֵ����Ȼ�ֵ
#define MC_TID_12        12    //����ֵ����ʱ��ı�Ȼ�ֵ
#define MC_TID_13        13    //����ֵ���̸�����
#define MC_TID_14        14    //����ֵ����ʱ��Ķ̸�����
#define MC_TID_15        15    //�ۼ���
#define MC_TID_16        16    //��ʱ����ۼ���
#define MC_TID_17        17    //��ʱ��ļ̵籣���豸�¼�
#define MC_TID_18        18    //��ʱ��ļ̵籣���豸���������¼�
#define MC_TID_19        19    //��ʱ��ļ̵籣���豸���������·��Ϣ
#define MC_TID_20        20    //����λ����ĳ��鵥����Ϣ
#define MC_TID_21        21    //����ֵ������Ʒ�������ʵĹ�һ��ֵ
/* 22...29 Ϊ�������ݶ��屣��*/
#define MC_TID_22        22
#define MC_TID_23        23
#define MC_TID_24        24
#define MC_TID_25        25
#define MC_TID_26        26
#define MC_TID_27        27
#define MC_TID_28        28
#define MC_TID_29        29
#define MC_TID_30        30    //��CP56Time2aʱ��ĵ�����Ϣ
#define MC_TID_31        31    //��CP56Time2aʱ���˫����Ϣ
#define MC_TID_32        32    //��CP56Time2aʱ��Ĳ�λ����Ϣ
#define MC_TID_33        33    //��CP56Time2aʱ���32���ش�
#define MC_TID_34        34    //��CP56Time2aʱ��Ĳ���ֵ����һ��ֵ
#define MC_TID_35        35    //��CP56Time2aʱ��Ĳ���ֵ����Ȼ�ֵ
#define MC_TID_36        36    //��CP56Time2aʱ��Ĳ���ֵ���̸�����
#define MC_TID_37        37    //��CP56Time2aʱ����ۼ���
#define MC_TID_38        38    //��CP56Time2aʱ��ļ̵籣���豸�¼�
#define MC_TID_39        39    //��CP56Time2aʱ��ļ̵籣���豸���������¼�
#define MC_TID_40        40    //��CP56Time2aʱ��ļ̵籣���豸���������·��Ϣ
/* 41...44 Ϊ�������ݶ��屣��*/
#define MC_TID_41        41
#define MC_TID_42        42
#define MC_TID_43        43
#define MC_TID_44        44
//�ڿ��Ʒ���Ĺ�����Ϣ
#define MC_TID_45        45    //��������
#define MC_TID_46        46    //˫������
#define MC_TID_47        47    //��������
#define MC_TID_48        48    //�趨ֵ�����һ��ֵ
#define MC_TID_49        49    //�趨ֵ�����Ȼ�ֵ
#define MC_TID_50        50    //�趨ֵ����̸�����
#define MC_TID_51        51    //32���ش�
/* 52...69 Ϊ�������ݶ��屣��:104�ֶ���������58��64*/
#define MC_TID_52        52
#define MC_TID_53        53
#define MC_TID_54        54
#define MC_TID_55        55
#define MC_TID_56        56
#define MC_TID_57        57
#define MC_TID_58        58    //��ʱ��CP56Time2a�ĵ�������
#define MC_TID_59        59    //��ʱ��CP56Time2a��˫������
#define MC_TID_60        60    //��ʱ��CP56Time2a�Ĳ�������
#define MC_TID_61        61    //��ʱ��CP56Time2a���趨ֵ�����һ��ֵ
#define MC_TID_62        62    //��ʱ��CP56Time2a���趨ֵ�����Ȼ�ֵ
#define MC_TID_63        63    //��ʱ��CP56Time2a���趨ֵ����̸�����
#define MC_TID_64        64    //��ʱ��CP56Time2a��32���ش�

//�ڼ��ӷ����ϵͳ����
#define MC_TID_70        70    //��ʼ������
/* 71...99 Ϊ�������ݶ��屣��*/
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
//�ڿ��Ʒ����ϵͳ����
#define MC_TID_100       100   //վ���ܣ��ٻ�����
#define MC_TID_101       101   //�������ٻ�����
#define MC_TID_102       102   //������
#define MC_TID_103       103   //ʱ��ͬ������
#define MC_TID_104       104   //��������
#define MC_TID_105       105   //��λ��������
#define MC_TID_106       106   //��ʱ�������
/* 107...109 Ϊ�������ݶ��屣��:104�ֶ���������107*/
#define MC_TID_107       107   //��ʱ��CP56Time2a�Ĳ�������
#define MC_TID_108       108
#define MC_TID_109       109
//�ڿ��Ʒ���Ĳ�������
#define MC_TID_110       110   //������ֵ��������һ��ֵ
#define MC_TID_111       111   //������ֵ��������Ȼ�ֵ
#define MC_TID_112       112   //������ֵ�������̸�����
#define MC_TID_113       113   //��������
/* 114...119 Ϊ�������ݶ��屣��*/
#define MC_TID_114        114
#define MC_TID_115        115
#define MC_TID_116        116
#define MC_TID_117        117
#define MC_TID_118        118
#define MC_TID_119        119
//�ļ�����
#define MC_TID_120       120   //�ļ�׼������
#define MC_TID_121       121   //��׼������
#define MC_TID_122       122   //�ٻ�Ŀ¼��ѡ���ļ����ٻ��ļ����ٻ���
#define MC_TID_123       123   //���Ľڣ����Ķ�
#define MC_TID_124       124   //�Ͽɵ��ļ����ϿɵĽ�
#define MC_TID_125       125   //��
#define MC_TID_126       126   //Ŀ¼
/* 127 Ϊ�������ݶ��屣��:104�ֶ���������127*/
#define MC_TID_127       127   //��־��ѯ������浵�ļ�
/*136...255 ����Ӧ�ã�ר�÷�Χ��*/
#define MC_TID_136       136   //U֡����Ự STARTDT
#define MC_TID_137       137   //U֡����Ự STOPDT
#define MC_TID_138       138   //U֡����Ự TESTFR
#define MC_TID_139       139   //U֡����Ự NEWHOLD ����һ��ͨ��
#define MC_TID_140       140   //U֡����Ự DELHOLD ��һ��ͨ��




/******************************����Ӧ�ù���**************************************/
#define C_IC_NA         100 //�ٻ��������ͱ�ʶ(tid)
#define C_RD_NA         102 //���������ͱ�ʶ��tid��
#define C_CS_NA         103 //ʱ��ͬ���������ͱ�ʶ��tid��
#define C_TS_NA         104 //�����������ͱ�ʶ��tid��

/******************************VARIABLE STRUCTURE QUALIFIER (VSQ)***************/
#define C_N_0           0   //�ɱ�ṹ�޶���N����Ϊ0;
#define C_N_1           1   //�ɱ�ṹ�޶���N����Ϊ1;
#define C_SQ_0          0   //�ɱ�ṹ�޶���SQ����Ϊ0;
#define C_SQ_1          1   //�ɱ�ṹ�޶���SQ����Ϊ1;

/******************************����********************************************/
#define C_STEP_NO_ONE   1   //���ţ�1
#define C_STEP_NO_TWO   2   //���ţ�2
#define C_STEP_NO_THREE 3   //���ţ�3
#define C_STEP_NO_FOUR  4   //���ţ�4

/***************************************************************************/
#define TIMER_NOP   0       //��ʱ���ղ���������ԭ״̬������ͣ�ָ���
#define TIMER_START 1       //��ʱ����������
#define TIMER_PAUSE 2       //��ʱ����ֵ���������¸�λ
#define TIMER_STOP  3       //��ʱ����ͣ����

/***************************************************************************/




// վ���ɫ
#define MASTER_STATION     0x1  // ����վ����վ��
#define SLAVE_STATION      0x2  // ������վ����վ��
#define COMBINED_STATION   0x3  // ϵͳվ���ۺ�վ��

// ����վ����վ����״̬����
#define MASTER_STOPPED              0x0  // STOPPED����
#define MASTER_STARTED              0x1  // STARTED����
#define MASTER_UNCONFIRMED_STOPPED  0x2  // ����δȷ��STOPPED����
#define MASTER_PEND_STARTED         0x3  // ����STARTED����
#define MASTER_PEND_STOPPED         0x4  // ����STOPPED����

// ������վ����վ����״̬����
#define SLAVE_STOPPED               0x0  // STOPPED����
#define SLAVE_STARTED               0x1  // STARTED����
#define SLAVE_UNCONFIRMED_STOPPED   0x2  // ����δȷ��STOPPED����

// ��������֡�����͵Ĺ���
#define IEC104_TESTFR_ACK   0x83  // TESTFRȷ��
#define IEC104_TESTFR_ACT   0x43  // TESTFR����
#define IEC104_STOPDT_ACK   0x23  // STOPDTȷ��
#define IEC104_STOPDT_ACT   0x13  // STOPDT����
#define IEC104_STARTDT_ACK  0x0B  // STARTDTȷ��
#define IEC104_STARTDT_ACT  0x07  // STARTDT����

#define IEC104_NEWHOLD_ACT  0x03  // ����һ��ͨ�� ����
#define IEC104_NEWHOLD_ACK  0xAB  // ����һ��ͨ�� ȷ��
#define IEC104_DELHOLD_ACT  0xFF  // ��һ��ͨ�� ����
#define IEC104_DELHOLD_ACK  0x57  // ��һ��ͨ�� ȷ��

// ͨ��ִ�б���
#define NO_OPERATION   0x0  // ������·���д���
#define OPEN_CONNECT   0x1  // ����·
#define CLOSE_CONNECT  0x2  // �ر���·
#define RETRY_CONNECT  0x3  // ������·
#define TEST_CONNECT   0x4  // ��ȡ��·״̬��Ϣ
#define WRITE_CONNECT  0x5  // ͨ����·��������
#define READ_CONNECT   0x6  // ͨ����·��ȡ����
#define PROC_RECV_CTRL 0x7  // ͨ����·��������յĴ����������
#define PROC_RECV_ASDU 0x8  // ͨ����·��������յ�ASDU����

///////////  �ײ���ղ�APDU������������      ///////////////
#define  MC_APDU_OK         0  // APDU���ݽ����ɹ�
#define  MC_REMOTE_WARNING  1  // Զ�̴�����Ƹ�֪����
#define  MC_NATIVE_WARNING  2  // ���س�ʱ��֪����
#define  MC_REMOTE_FLOWCTRL 3  // Զ�̴������Ҫ����������
#define  MC_NATIVE_FLOWCTRL 4  // ����Ҫ����������
#define  MC_APDU_DISORDER   5  // APDU֡ʧ�����
#define  MC_APDU_CHECKSUM   6  // APDU֡У��ʧ��
#define  MC_APDU_UNKNOWN    7  // �޷�ʶ���֡

// ��׼����������֮ǰ����鷢��״̬������״̬�ı���
#define SEND_READY_STATUS       0x0  // ����׼��״̬���������Ͳ���׼��������
#define SEND_ROLE_ERROR         0x1  // վ��Ľ�ɫ����
#define SEND_FRAME_ERROR        0x2  // �Ƿ�������֡������U֡ͬʱ���ڶ��ֹ��ܣ�
#define SEND_STATUS_ERROR       0x3  // �Ƿ��ķ���״̬����״̬���в����ڵ�״̬��
#define SEND_TRANSITION_ERROR   0x4  // ״̬Ǩ����󣬼������㷢�����ݵ�������
#define SEND_TIMESCALE_ERROR    0x5  // ����ʱ�����

//�쳣����ͳһ����
#define ERR_POINTER_NULL        -0x80 //ָ�����Ϊ�ձ���
#define ERR_STEP_NO             -0x01 //���Ŵ���
#define ERR_FRAME_NO            -0x02 //֡�Ŵ���
#define ERR_COT_VAL             -0x03 //cotֵ����
#define ERR_ACT_PAS_VAL         -0x04 //�������Բ���ֵ����
#define ERR_VSQN_NO             -0x05 //�޶�����ʵ��Ԫ�ظ�������
#define ERR_TID_VAL             -0x06 //TIDֵ����
#define ERR_MAX_VAL_CH          -0x15 //���ֵ����ʧ��
#define ERR_MIN_VAL_CH          -0x16 //��Сֵ����ʧ��
#define ERR_INTE_VAL_CH         -0x17 //����ֵ����ʧ��
#define ERR_FRAME_LNG           -0x18 //���֡���ȴ���
#define ERR_ACS_MTHD            -0x19 //֡������������ô���
#define ERR_CONFIG              -0x1A //���ô���
#define ERR_TIME_TYPE           -0x1B //ʱ�����ʹ���
#define ERR_VSQN_SQ             -0x1C //�޶��ʵ�sq���ʹ���
#define ERR_PNTVAL_LNG          -0x1D //��ֵӦ�еĳ��ȴ���
#define ERR_INFOELELNG_CFG      -0x1E //��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ������ô���
#define ERR_SYSTEM_LOGIC        -0x1F //ϵͳ�߼�����
#define ERR_LOGIC_RUN           -0x20 //�߼�����
#define ERR_FC_VAL_RAN          -0x21 //�Ự�Ż�Function Code ֵ�ķ�Χ����
#define ERR_RESP_NUM            -0X22 //��Ӧ����������
#define ERR_ADDR_VAL            -0X23 //
#define ERR_INPUT_VAL           -0x24 //�����������
#define ERR_VSQN_N              -0x25 //�޶��ʵ�N���ʹ���




#ifndef FunTypeOne
    typedef void ( *FunTypeOne ) ( void* );
#endif




#endif // MACRO_H