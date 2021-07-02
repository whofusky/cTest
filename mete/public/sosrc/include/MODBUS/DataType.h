

#ifndef BASE_H
#define BASE_H




typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;




typedef struct
{
    u_char value: 2;
    u_char pad: 6;
} UI2;                  //λ���2���޷�����


typedef u_char UI8;     //λ���8���޷�����


typedef struct
{
    u_char loBCD: 4;
    u_char hiBCD: 4;
} UI8BCD;                        //2λʮ���Ƶ�BCD�����޷�����


typedef UI8BCD n6UI4BCD[3];      //6λʮ���Ƶ�BCD�����޷�����


typedef n6UI4BCD UI24BCD;        //6λʮ���Ƶ�BCD�����޷�����


typedef char I8;                 //λ���8������


typedef short I16;              //λ���16������


typedef u_short UI16;           //λ���16���޷�����

typedef struct
{
    u_short value: 12;
    u_short pad: 4;
} I12Right;                     //λ���12���Ҷ�������


typedef struct
{
    u_short pad: 4;
    u_short value: 12;
} I12Left;                      //λ���12�����������


typedef struct
{
    UI8BCD n2LoBCD;
    UI8BCD n2MdBCD;
    u_char n1HiBCD: 4;
    u_char sign: 1;
    u_char pad1: 3;
    u_char pad2: 8;
} I21BCD;                      //5λʮ���Ƶ�BCD��������


typedef u_char UF8;            //λ���8���޷���һ��������


typedef char F8;               //λ���8�Ĺ�һ��������


typedef F8 UF8_1;              //2��λ���8���޷���һ��������


typedef struct
{
    u_short value: 15;
    u_short  sign: 1;
} F16;                         //λ���16�Ĺ�һ��������


typedef UF8 F8_1;              //2��λ���8�Ĺ�һ��������


typedef struct
{
    u_short value: 11;
    u_short  sign: 1;
    u_char pad1: 4;
} F12Right;                    //λ���12���Ҷ����һ��������


typedef struct
{
    u_char pad1: 4;
    u_short value: 11;
    u_short  sign: 1;
} F12Left;                     //λ���12��������һ��������


typedef float R32_23;          //��λ���32С��λ���23�ĸ�����


typedef struct
{
    u_char value: 1;
    u_char pad: 7;
} BS1;                         //λ���1�ı��ش�


typedef struct
{
    u_char value: 2;
    u_char pad: 6;
} BS2;                         //λ���2�ı��ش�


typedef BS2 n4BS2[4] ;         //4��λ���2�ı��ش�


typedef u_char BS8;            //λ���8�ı��ش�


typedef u_short BS16;          //λ���16�ı��ش�

//�����Ʊ�ŵĲ������ַ���(ע�⣺�����͵ı���������洢�ռ�)
typedef BS8* OS8i;


typedef UI2 DC;                //˫����


typedef UI2 PRC;               //������������


typedef UI2 SRC;               //����������


typedef UI2 DPI;               //˫����Ϣ


typedef UI8 n1BINUI8;          //8ȡ1��


typedef UI8 LTE250UI8;         //0~250��ֵ


typedef BS1 SC;                //������


typedef BS8 n8SINBS8;          //8����״̬�Ĵ���


typedef n4BS2 n4S_TINn4BS;     //8����״̬+˲̬���


typedef BS16 n8S_n8CDINnBS16;  //16����״̬+��λ���


typedef OS8i OS8iASCII;        //ASCII�ַ���


typedef struct
{
    u_char value: 7;
    u_char error: 1;
} UI7V_E;                      //�����ָʾ��ֵ


typedef UI7V_E UI7R120V_E;     //�����ָʾ��ΧΪ120��ֵ

typedef struct
{
    u_char value: 7;
    u_char error: 1;
} UF7V_E;                      //�����ָʾ�Ĺ�һ��ֵ


typedef struct
{
    u_char value: 6;
    u_char transient: 1;
    u_char error: 1;
} UF6V_T_E;                    //��˲̬�Ͳ��ָʾ��ֵ


typedef struct
{
    u_short overflow: 1;
    u_short error: 1;
    u_short value: 14;
} O_E_UF14V;                   //���������ָʾ��14���ع�һ��������


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
} CP48TIME1BCD;               //BCDʱ��


typedef struct
{
    u_int mScnd1;
    u_char mScnd2: 6;
    u_char RES: 1;
    u_char SU: 1;
} CP40TIME1;                 //������ʱ��1


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
} CP56TIME2a;               //������ʱ��2a


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
} CP56TIME2b;              //������ʱ��2b


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
} CP56TIME2c;             //������ʱ��2c



typedef struct
{
    BS8 value;
    u_char sign: 1;
    u_char pad: 7;
} BS8_S;                  //�����ŵİ�λBit��


typedef struct
{
    u_char OV: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} QUAL1;                  //Ʒ��������


typedef struct
{
    u_char memSQ: 5;
    u_char memCY: 1;
    u_char memCA: 1;
    u_char memIV: 1;
} CNTR_STATE;             //������˳����




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


//u_short stnAcsFlag ��λ�洢����
typedef struct
{
    //λ0 = 1�洢��ϵͳִ������ֵ�ı��棻λ1 = 1�洢��ϵͳִ������ֵ�ļ��أ�ȫ = 0,
    // do nothing��ȫ = 1�������ұ���
    u_char  ctlType: 2;
    //����λ
    u_char  ctlRes: 1;
    //APDUԪ�ص�3λASDU_VSQ_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  vsqValid: 1;
    //APDUԪ�ص�4λASDU_COT_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  cotValid: 1;
    //APDUԪ�ص�5λASDU_ADDR_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  addrValidA: 1;
    //APDUԪ�ص�6λBODY_TID_INDE ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  tidValid: 1;
    //APDUԪ�ص�7λBODY_ADDR_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  addrValidB: 1;
    //APDUԪ�ص�8λBODY_SET_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  setValid: 1;
    //APDUԪ�ص�9λBODY_TSP_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  tspValidB: 1;
    //APDUԪ�ص�10λASDU_TSP_INDEX ��=1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    u_char  tspValidA: 1;
    u_char  pad: 5;
} T_stnAcsFlag;

//u_short stnAcsFlag ��λ�洢����
typedef struct
{
    /*λ0 = 1�洢��ϵͳִ������ֵ�ı��棻
    λ1 = 1�洢��ϵͳִ������ֵ�ļ��أ�
    ȫ = 0, do nothing��ȫ = 1�������ұ���*/
    u_short  ctlType: 2;
    u_short  ctlRes: 1;     //����λ
    /*ADUԪ�ص�3λPDU_DATA_INDEX ��
    =1��ʾctlType�Ĳ����ڴ�Ԫ������Ч��
    =0Ϊ��Ч*/
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
    u_int   pntAddr;            // ���ַ
    u_int   pntVal1;            // ��ֵ�ĵ�4�ֽڣ�����ʱ������ֵ������Сֵ
    u_int   pntVal2;            // ��ֵ�ĸ�4�ֽڣ�����ʱ�������ֵ
} T_gVal_ChkStd;


typedef struct
{
    u_int pntAddr;
    u_int pntVal;               //��ֵ�õ�4�ֽڣ�����ʱ����У���
} T_fVal_ChkStd;


typedef struct
{
    //λ0 = 1 ��ʾ�����ֵ���գ�//λ 1 = 1ʱ����Сֵ����
    u_short vrfCd;
    union
    {
        T_fVal_ChkStd fchkStd;
        T_gVal_ChkStd gChkStd;
    } chkStd;
    //����״̬��//λ0 = 1 ��ʾ�����ֵ���ճɹ���//λ 1 = 1ʱ����Сֵ���ճɹ�
    int* chkRslt_p;
} vrfItem;


typedef struct
{
    u_short* TID_p;   // TID_p[]
    u_char*  TIDLng_p;//TIDLng_p[]ÿһ��tidԪ�صĳ���
    u_short COT;      // COT
    u_short MaxLen;   // ���֡����
    u_char  TidNum;
} ptclStep;


typedef struct
{
    u_short     ssnDscNo;       //�Ự��
    //�ж�ֵ�Ƿ�Ϊ0xffff ���Ϊ���ʾ�������Ϊ��(char*) ��ʾ������
    u_short     act_prio_chnlID;
    u_char      usingMode;      //ģʽ��
    ptclStep*   remoteSet_p;
    u_char      setNum;         //ptclStep����
    u_char      realTID;        //��ǰ֡ʹ�õ�tid
} T_ptclFunDsc;


////////////////////////////104Э��/////////////////////////////////////////
typedef ELE_TYPE ASDU_TID;      //���ͱ�ʶ
typedef ELE_TYPE ASDU_LNG;      //ASDU����
typedef ELE_TYPE ASDU_VSQ;      //�ɱ�ṹ�޶���
typedef ELE_TYPE ASDU_COT;      //����ԭ��
typedef ELE_TYPE ASDU_ADDR;     //ASDU������ַ
typedef ELE_TYPE BODY_TID;      //��Ϣ������
typedef ELE_TYPE BODY_ADDR;     //��Ϣ���ַ
typedef ELE_TYPE BODY_SET;      //��ϢԪ�ؼ�
typedef ELE_TYPE BODY_TSP;      //��Ϣ��ʱ��
typedef ELE_TYPE ASDU_TSP;      //ASDUʱ��
/*
�����������ݽṹ��Ӷ���˵��@20180222164533
    BODY_TID , BODY_ADDR , BODY_SET ,BODY_TSP , ASDU_TSP ��5������
    ��ASDU��ͬ�ṹʱ��Ӧ�� lng ��Ա ��ʾ�ĺ������£�
    1�����ɱ�ṹ�޶��ʵ�SQ = 1 ��������ʱ��
        lng ��ʾ���� p_Mem��Ӧ��ַ�����ݵ���������
    2�����ɱ�ṹ�޶��ʵ�SQ = 0 �������ʱ��
        lng ��ʾ���� p_Mem��Ӧ��ַ�е���Ԫ�صĳ���
*/


typedef struct
{
    BODY_TID   body_Tid;    //��Ϣ������
    BODY_ADDR  body_Addr;   //��Ϣ���ַ
    BODY_SET   body_Set;    //��ϢԪ�ؼ�
    BODY_TSP   body_Tsp;
} BODY_OBJ;


typedef struct
{
    ASDU_TID  asduTid;      //���ͱ�ʶ
    ASDU_LNG  asduLng;      //ASDU����
    ASDU_VSQ  asduVsq;      //�ɱ�ṹ�޶���
    ASDU_COT  asduCot;      //����ԭ��
    ASDU_ADDR asduAddr;     //ASDU������ַ
    BODY_OBJ* bodyObj_p;    //��Ϣ����
    ASDU_TSP  asduTsp;
} dataFrame;
////////////////////////////end 104Э��/////////////////////////////////////////



////////////////////////////modbusЭ��/////////////////////////////////////////
typedef u_short ADU_TRANS_ID; //����Ԫ��ʶ��
typedef u_short ADU_PCTL_ID;  //Э���ʶ��
typedef u_short ADU_LENGTH;   //����
typedef u_char  ADU_UNIT_ID;  //��Ԫ��ʶ��

typedef ELE_TYPE ELE_MBAPH;    //MBAP����ͷ

typedef struct
{
    ADU_TRANS_ID   trans_id;  //����Ԫ��ʶ��
    ADU_PCTL_ID    pctl_id;   //Э���ʶ��
    ADU_LENGTH     length;    //����
    ADU_UNIT_ID    unit_id;   //��Ԫ��ʶ��
} MBAP_HEADER; //MBAP����ͷ

typedef ELE_TYPE ADU_ADDR;  //ADU ��ַ��
typedef u_char   PDU_FC;    //PDU������
typedef ELE_TYPE PDU_DATA;  //PDU����
typedef ELE_TYPE ADU_ECHK;  //ADU���У��

typedef struct
{
    MBAP_HEADER mbapHeader; //MBAP����ͷ
    ADU_ADDR    aduAddr;    //ADU ��ַ��
    PDU_FC      pduFc;      //PDU������
    PDU_DATA    pduData;    //PDU����
    ADU_ECHK    aduEchk;    //ADU���У��
} T_modbusFrame;

////////////////////////////end modbusЭ��//////////////////////////////////////

typedef union//��������Э���ͨ��֡�ṹ
{
    T_modbusFrame modbus;
    //...  ����չ����Э��
} T_comFrame;




/*
    20180411144232 ע�� (*reqEleVal_pp)[11] �� (*respEleVal_pp)[11]
                   ��� comReqData_p �� comRespData_p
*/
typedef struct
{
    /*λ0 = 1�洢��ϵͳִ������ֵ�ı��棻λ1 = 1�洢��ϵͳִ������ֵ�ļ��أ�ȫ = 0,
          do nothing��ȫ = 1�������ұ���
    λ3 - 10 �ֱ��ʾAPDUԪ��3 -10��=1��ʾ��λ0��λ1�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч
    */
    u_short      stnAcsFlag;

    //����֡����
    u_char       reqAPDUNum;
    u_char*      reqBody_setNum_p;

    // reqEleVal_ppָ�����ݵ�ָ�룬���������11��Ԫ�أ�ELE_TYPE ele[i][11] ,
    //0<=i<reqASDUNum
    ELE_TYPE     ( *reqEleVal_pp ) [11];

    //104Э����
    dataFrame*    reqData_p;
    //bodbus������Э����
    T_comFrame*   comReqData_p;

    //��Ӧ֡����
    u_char       respAPDUNum;
    u_char*      respBody_setNum_p;

    //respEleVal_ppָ�����ݵ�ָ�룬���������11��Ԫ�� ELE_TYPE ele[i][11] ,
    //0<=i<reqASDUNum
    ELE_TYPE         ( *respEleVal_pp ) [11];

    //104Э����
    dataFrame*    respData_p;
    //bodbus������Э����
    T_comFrame*   comRespData_p;

} T_ssnStepCfg;




typedef struct
{
    u_char  ssnID;   //�Ự��
    u_char  actFlag; //�Ự�������ԣ�����
    u_char  infoType;//�Ự����Ϣ���ͣ�ͨ����Ϣ������ļ���Ϣ����
    u_short cmnAddr; //������ַ
    u_char  stepNum; //�����ĻỰ�ܲ���
    T_ssnStepCfg* stepCfg_p; //�Ự������ stepCfg[i], 0<=i<stepNum
} T_snnCfg; //�Ự��������Ϣ�������ò��ڵ���������Ϣ


// �Ự����
typedef struct//ͨ����Ϣ����
{
    u_char  pntType;            //ָʾ����Ϣ�������ͺ�ȡֵ��Χ //�������
    u_char  tmStmpType;         // ��Ϣ����ʱ������    ��104��101�����ʱ���ֽ�
    void* tmStmp_p;             //ʱ��
    T_gVal_ChkStd val_chkStd;   //Ԫ�ؼ�
    T_infoQDS dDsc;             // ͨ����Ϣ�����޶���
} snnPar_gInfoObj;


typedef struct ONE_STEP
{
    u_char stepNo;      //����
    u_char reqPDU_FCode;//����֡ʹ�õ�֡��ʽ���룬��2.2.5��
    u_char reqPDU_Num;  //����֡��PDU������
    //����֡APDU����ָ�������ΪreqPDU_Num��ͬ����APDU������ͬ����
    //���з�����ͬ��APDU����ָ��
    u_char* reqAPDU_Obj_p;

    //��Ӧ֡ʹ�õ�֡��ʽ���롣����Ҫ��Ӧʱ=unknow����2.2.5��
    u_char respPDU_FCode;
    //��ӦPDU������0--����Ҫ��Ӧ��255����Ҫ��ACK���϶���񶨣���Ϊ��Ӧ��
    //1~254����Ҫ��Ӧ��PDU��
    u_char respPDU_Num;
    //��Ӧ֡APDU����ָ�������ΪrespPDU_Num��ͬ����APDU������ͬ����
    //���з�����ͬ��APDU����ָ��
    u_char* respAPDU_Obj_p;
    //ר�ò���֤�飬���û�����ʱ��������ʱΪNULL��ֻʹ�����׹�Լ��Ĳ���֤����
    vrfItem* sVerifyBlock_p;
    //��һ��ָ�롣���һ��ʱ��=NULL
    ONE_STEP* nextStep_p;
} ONE_STEP;


typedef struct
{
    //ͨ�����ȷ��Զ��վ����ʹ��ͬЭ�鲻ͬ��Զ��վҲ���ܲ��ò�ʱ��Э��ʵ�ַ�ʽ��
    //�����Ҫ������ʶ
    u_int chGropNo;
    //����ţ������ܺ�
    u_char srvNo;

    /*����ģʽ�룬ģʽ������������ͬ�����ڲ�ͬ�����еĲ�ͬʵ�ַ�����
    ģʽ��ֵ��steps_p[]���������������ģʽ��=i�Ĳ���ϱ�����steps[i]��������,
    ����ͨ�Ų㱣����cmLvl[i]�У�����������stepNum[i]�С�
    ��usingMode=AUTOʱ����ʾ����ģʽ��APDU�������޷���̬��ֵ��
    ��usingMode=UNKNOWNʱ����ʾ������Լ���ṩ����ģʽ��������
    ����ʹ����һ������ĳ������������ɲ������������׹�Լ����ָ����
    ģʽ���в�ͬ�����зֱ��塣*/
    u_char usingMode;

    ONE_STEP* steps_p[4];
    //����ͨ�Ų㣬��2.2.5
    u_char cmLvl[4];
    //�����Ĳ���
    u_char stepNum[4];
} FUN_RL;


typedef struct//�ļ���Ϣ����
{
    u_char  pntType;            //ָʾ����Ϣ�������ͺ�ȡֵ��Χ
    u_char  tmStmpType;         // ��Ϣ����ʱ������
    void* tmStmp_p;             //ʱ��
    // ָʾ��T_fileDS����Ϣ���ͣ��Լ��������ݣ���У���롢�ļ����ݵ�
    T_fVal_ChkStd val_chkStd;
    T_fileDS dDsc;              // �ļ������޶���
} snnPar_fInfoObj;


typedef union
{
    snnPar_gInfoObj gData;
    snnPar_fInfoObj fData;
} T_snnUsrData;




typedef struct  //�Ự��������Ϣ�������ò��ڵ���������Ϣ����
{
    //����
    u_char stepNo;
    //=0��snnPar_gInfoObj��=1��snnPar_fInfoObj
    u_char infoType;

    //λ0 = 1�洢��ϵͳִ������ֵ�ı��棻λ1 = 1�洢��ϵͳִ������ֵ�ļ��أ�ȫ = 0,
    //      do nothing��ȫ = 1�������ұ���
    //λ3 - 10 �ֱ��ʾAPDUԪ��3 -10��=1��ʾ��λ0��λ1�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч��
    u_short stnAcsFlag;

    //�����Ա�ʶ��=0��������Э�����ڷ��͵����ݣ�=1��������Э���������յ����ݣ���ʱ
    //pntAddrOrVal��ʾ��ַ��
    u_char actFlag;

    //=������֡��
    u_char  ASDUIDNum;

    // ASDU ��ʶ�������ݵ�Ԫ���ͱ�ʶ��
    //modbus�����ڼ�¼ĳһ֡��������:������Ԫ��ʶ����2�ֽڣ�����Ԫ��ʶ����1�ֽڣ�
    //�������롱1�ֽ�
    u_int*  ASDUID_p;
    //����ԭ��
    u_short COT;
    //������ַ
    u_short cmnAddr;
    // ���ʷ����������γɿɱ�ṹ�޶���
    u_char acsMthd;
    //����Ԫ�أ���Ϣ���󣩸�����������//�ò������е���
    //���
    u_short pntNum;
    //��ǰ��ʼ��λ��
    u_short pntPos;

    /*��Ϣ����ʵ��ΪusrData[pntNum]���飬Ԫ������ΪsnnPar_gInfoObj
    ��snnPar_fInfoObj
    ����ʱ������*/
    u_char tmStmpType;

    //ʱ��
    void* tmStmp_p;

    T_snnUsrData* usrData_p;
    u_short pntStorNum;//��modbus�У���֡�����м��ж�Ҳ��д��ʱ�򣬴�ֵ��ʾд����
    u_short pntLoadNum;//��modbus�У���֡�����м��ж�Ҳ��д��ʱ�򣬴�ֵ��ʾ������
    u_short usrMaxIndex;//usrData_p����ռ�ʱ��������

} T_snnPara;




//modbus��Ҫ�ڷ���һ֡����ʱ��Ҫ��¼�ڵ㣬������֤��Ӧ�����Ƿ���ȷ
typedef struct
{
    u_short trans_id;//Transaction Identifier
    u_char  func_id; //Function Code
} T_mbsChkNode;



//modbus��Ҫ�ڷ�����������ʱ��Ҫ��¼�ڵ㣬������֤��Ӧ�����Ƿ���ȷ
typedef struct
{
    u_short    max_index;   //֧�ֵ������������������ֵ������Խ����ܣ�
    u_short    nex_index;   //��һ��������ݽ�������
    u_short    cur_index;   //��ǰ���������������
    T_mbsChkNode one_node_p[1];//Variable Length data
} T_mbsChkData;



typedef struct
{
    u_short     sendnum;
    u_short     receivenum;
    short       wValue;
    short       kValue;
    u_char      stage;
    //T_mbsChkNode�Ŀռ��¼����֡�еĹ���������ﴦ���
    //ʶ���Ա�˶Է���˷��ص���Ӧ֡
    T_mbsChkData chkData;
} realTimeValue;                        // ͨ��ʵʱ����


typedef struct
{
    // ���ݷ��͵Ŀ���ģʽ, ����3�ֹ���ģʽ��SEND_NO_CONTROL���޿���ģʽ��
    //SEND_TIMEOUT_CONTROL: ��ʱ����ģʽSEND_RETYR_CONTROL: ���Դ�������ģʽ
    u_char      Mode;                       //Ĭ������

    u_short     K;                          //Kֵ�����Χ
    u_short     W;                          //Wֵ�����Χ
} realTimeThreshold;




/****/
/*                �����ڻ�������101��104��Լ���ж���               */
/*********************************************************************/

/*****************************************MODBUS*******************************/
//typedef struct
//{
//
//    u_char        mode;////���ò���cfgParam_pָ����MODBUS���Ľṹ:RTU ASII TCP
//    u_short       W;   //Wֵ�����Χ
//}thresholdCfgPara;


/*****************************************END   *******************************/




#endif // CP_SEMAPHORE_H