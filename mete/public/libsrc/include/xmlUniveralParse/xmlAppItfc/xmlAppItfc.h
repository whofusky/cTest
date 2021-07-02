#pragma once
#include "xmlParse.h"
#include "dataRepresentation.h"
#include "xmlDataRepresentation.h"

extern const char* RL_ID;
extern const char* TGT_ID;
extern unsigned char infoFieldNum;//除初始化外，代替INFOFIELD_NUM，传入实际信息层节点数
extern const char* infoLvlNm[10];
extern const char* opt1_1Cnst[1];
extern const char* opt1_2Cnst[2];
extern const char* opt1_3Cnst[3];
extern const char* opt1_4Cnst[4];
extern const char* opt1_5Cnst[5];
extern const char* opt1_6Cnst[6];
extern const char* opt1_7Cnst[7];
extern const char* opt1_8Cnst[8];
extern const char* opt1_9Cnst[9];
extern const char* opt1_10Cnst[10];
extern const char* opt2_1Cnst[1];
extern const char* opt2_2Cnst[2];
extern const char* opt2_3Cnst[3];
extern const char* opt2_4Cnst[4];
extern const char* opt2_5Cnst[5];
extern const char* opt2_6Cnst[6];
extern const char* opt2_7Cnst[7];
extern const char* opt2_8Cnst[8];
extern const char* opt2_9Cnst[9];
extern const char* opt2_10Cnst[10];
extern const char* opt3_1Cnst[1];
extern const char* opt3_2Cnst[2];
extern const char* opt3_3Cnst[3];
extern const char* opt3_4Cnst[4];
extern const char* opt3_5Cnst[5];
extern const char* opt3_6Cnst[6];
extern const char* opt3_7Cnst[7];
extern const char* opt3_8Cnst[8];
extern const char* opt3_9Cnst[9];
extern const char* opt3_10Cnst[10];
extern const char* opt4_1Cnst[1];
extern const char* opt4_2Cnst[2];
extern const char* opt4_3Cnst[3];
extern const char* opt4_4Cnst[4];
extern const char* opt4_5Cnst[5];
extern const char* opt4_6Cnst[6];
extern const char* opt4_7Cnst[7];
extern const char* opt4_8Cnst[8];
extern const char* opt4_9Cnst[9];
extern const char* opt4_10Cnst[10];
extern const char* opt5_1Cnst[1];
extern const char* opt5_2Cnst[2];
extern const char* opt5_3Cnst[3];
extern const char* opt5_4Cnst[4];
extern const char* opt5_5Cnst[5];
extern const char* opt5_6Cnst[6];
extern const char* opt5_7Cnst[7];
extern const char* opt5_8Cnst[8];
extern const char* opt5_9Cnst[9];
extern const char* opt5_10Cnst[10];
extern const char* opt6_1Cnst[1];
extern const char* opt6_2Cnst[2];
extern const char* opt6_3Cnst[3];
extern const char* opt6_4Cnst[4];
extern const char* opt6_5Cnst[5];
extern const char* opt6_6Cnst[6];
extern const char* opt6_7Cnst[7];
extern const char* opt6_8Cnst[8];
extern const char* opt6_9Cnst[9];
extern const char* opt6_10Cnst[10];
extern const char* opt7_1Cnst[1];
extern const char* opt7_2Cnst[2];
extern const char* opt7_3Cnst[3];
extern const char* opt7_4Cnst[4];
extern const char* opt7_5Cnst[5];
extern const char* opt7_6Cnst[6];
extern const char* opt7_7Cnst[7];
extern const char* opt7_8Cnst[8];
extern const char* opt7_9Cnst[9];
extern const char* opt7_10Cnst[10];
extern const char* opt8_1Cnst[1];
extern const char* opt8_2Cnst[2];
extern const char* opt8_3Cnst[3];
extern const char* opt8_4Cnst[4];
extern const char* opt8_5Cnst[5];
extern const char* opt8_6Cnst[6];
extern const char* opt8_7Cnst[7];
extern const char* opt8_8Cnst[8];
extern const char* opt8_9Cnst[9];
extern const char* opt8_10Cnst[10];
extern const char* opt9_1Cnst[1];
extern const char* opt9_2Cnst[2];
extern const char* opt9_3Cnst[3];
extern const char* opt9_4Cnst[4];
extern const char* opt9_5Cnst[5];
extern const char* opt9_6Cnst[6];
extern const char* opt9_7Cnst[7];
extern const char* opt9_8Cnst[8];
extern const char* opt9_9Cnst[9];
extern const char* opt9_10Cnst[10];
extern const char* opt10_1Cnst[1];
extern const char* opt10_2Cnst[2];
extern const char* opt10_3Cnst[3];
extern const char* opt10_4Cnst[4];
extern const char* opt10_5Cnst[5];
extern const char* opt10_6Cnst[6];
extern const char* opt10_7Cnst[7];
extern const char* opt10_8Cnst[8];
extern const char* opt10_9Cnst[9];
extern const char* opt10_10Cnst[10];


namespace XML_APP_ITFC
{
    using namespace WPFC_Sys_20;
    using namespace DATA_REPRSENTATION;
    using namespace XML_DTATA_REPRSENTATION;

    //导出接口函数
    extern XMLRL_PARSE_ERROR loadSysXmlStd ( const char* appStdFileNm,
            xmlStd* xmlAppStd_p
                                           );
    extern XMLRL_PARSE_ERROR chkXmlFile ( const char* tgtFileNm,
                                          xmlStd* xmlParse_p
                                        );

    extern ndDataLst getDataByAttr ( xmlStd* xmlParse,
                                     const char* ndNm,
                                     unsigned int lvlNo,
                                     const char* attrNm,
                                     const char* attrNmVal
                                   );
    extern ndDataLst getDataNdByPAttrNmVal ( xmlStd* xmlParse,
            const char* ndNm,
            unsigned int lvlNo,
            const char* pNdNm,
            const char* pNdNmAttrVal
                                           );
    extern ndDataLst getDataNdByPAttr ( xmlStd* xmlParse,
                                        const char* ndNm,
                                        unsigned int lvlNo,
                                        const char* pNdNm,
                                        const char* pNdAttrNm,
                                        const char* pNdAttrVal
                                      );

    extern const char*  getNdNm ( entireData* usrData );
    extern int  getNdPureValNum ( entireData* usrData );
    extern int  getNdVal ( entireData* usrData,
                           int* dataType,
                           void* dBuf,
                           int dBufLng
                         );
    extern int  getAttrNum ( entireData* usrData );
    extern const char*  getAttrNm ( entireData* usrData,
                                    unsigned int attrOrdNo
                                  );
    extern int  getAttrPureValNum ( entireData* usrData,
                                    const char* attrNm
                                  );
    extern int  getAttrVal ( entireData* usrData,
                             const char* attrNm,
                             int* dataType,
                             void* dBuf,
                             int dBufLng
                           );
    extern ndDataLst getDataNdByNdID ( xmlStd* xmlParse,
                                       unsigned int ndID
                                     );
    extern unsigned int getIDsByPID ( xmlStd* xmlParse,
                                      const char* ndNm,
                                      unsigned int ele_TgtPID,
                                      unsigned int* ele_TgtID_p,
                                      unsigned int IDNum
                                    );
    extern unsigned int getIDsByAttr ( xmlStd* xmlParse,
                                       const char* ndNm,
                                       unsigned int lvlNo,
                                       const char* attrNm,
                                       const char* attrNmVal,
                                       unsigned int* ele_TgtID_p,
                                       unsigned int IDNum
                                     );
    extern int setAppRlRestrict ( char** rstrctClause,
                                  char** rstrct,
                                  unsigned char clauseNum
                                );

}

