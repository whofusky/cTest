//#include "xmlParse.h"
#include "xmlAppItfc.h"
//#include "dataRepresentation.h"
//#include "commAppRlItfc.h"
//#include "xmlDataRepresentation.h"

//应用级对外接口
namespace XML_APP_ITFC
{
    using namespace WPFC_Sys_20;
    using namespace COMM_APP_ITFC;
    using namespace DATA_REPRSENTATION;
    using namespace XML_DTATA_REPRSENTATION;

    XMLRL_PARSE_ERROR
    loadSysXmlStd ( const char* appStdFileNm, xmlStd* xmlAppStd_p )
    {
        if ( ( !appStdFileNm ) || ( !xmlAppStd_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        const char* tInAppStdFileNm = appStdFileNm;
        xmlStd* tInXmlStd_p = xmlAppStd_p;
        return tInXmlStd_p->loadXmlStd ( tInAppStdFileNm );
    }

    XMLRL_PARSE_ERROR
    chkXmlFile ( const char* tgtFileNm, xmlStd* xmlParse_p )
    {
        if ( ( !tgtFileNm ) || ( !xmlParse_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        const char* tInTgtFileNm_p = tgtFileNm;
        xmlStd* tInXmlParse_p = xmlParse_p;
        return tInXmlParse_p->chkTgtXmlFile ( tInTgtFileNm_p );
    }

    //功能：根据层号和节点名，以及它的属性名和属性取值字符串，找符合条件的所有节点，
    //      并返回它们的所有应用数据，最多支持XML_MAX_ATTRNUM + 1个节点
    //参2：输入，节点名
    //参3：输入，节点层号，0是根节点编号
    //参4：输入，节点的给定属性名
    //参5：输入，节点给定属性名的取值字符串
    //返回值：符合查找条件的所有节点的取值列表，每节点包含节点自身和所有属性的应用数据值
    ndDataLst
    getDataByAttr ( xmlStd* xmlParse, const char* ndNm, unsigned int lvlNo,
                    const char* attrNm, const char* attrNmVal )
    {
        if ( ( !xmlParse ) || ( !ndNm ) )
        {
            return NULL;
        }

        xmlStd* tInXmlParse_p = xmlParse;
        const char* tInNdNm = ndNm;
        unsigned int tInLvlNo = lvlNo;
        const char* tInAttrNm = attrNm;
        const char* tInAttrNmVal = attrNmVal;

        return tInXmlParse_p->getNdByAttr ( tInNdNm, tInLvlNo, tInAttrNm, tInAttrNmVal );
    }

    //功能：根据层号和节点名，以及父节点名为name的属性取值字符串，找符合条件的所有节点，
    //      并返回它们的所有应用数据，最多支持XML_MAX_ATTRNUM + 1个节点。不支持多个符合
    //      要求的父节点（两棵子树）： 如父亲代表子系统，属性name值是子系统的名称，则不
    //      允许目标文件中出现多个相同的子系统
    //参1：输入，节点名
    //参2：输入，节点层号
    //参3：输入，父节点名
    //参4：输入，父节点name属性取值字符串
    //返回值：符合查找条件的所有节点的取值列表，每节点包含节点自身和所有属性的应用数据值
    ndDataLst
    getDataNdByPAttrNmVal ( xmlStd* xmlParse, const char* ndNm, unsigned int lvlNo,
                            const char* pNdNm, const char* pNdNmAttrVal )
    {
        if ( !xmlParse )
        {
            return NULL;
        }

        xmlStd* tInXmlParse_p = xmlParse;
        const char* tInNdNm = ndNm;
        unsigned int tInLvlNo = lvlNo;
        const char* tInPNdNm = pNdNm;
        const char* tInPNdNmAttrVal = pNdNmAttrVal;
        return tInXmlParse_p->getNdByPAttrNmVal ( tInNdNm, tInLvlNo, tInPNdNm, tInPNdNmAttrVal );
    }

    ndDataLst getDataNdByNdID ( xmlStd* xmlParse, unsigned int ndID )
    {
        if ( !xmlParse )
        {
            return NULL;
        }

        xmlStd* tInXmlParse_p = xmlParse;
        unsigned int tInNdID = ndID;
        return tInXmlParse_p->getNdByNdID ( tInNdID );
    }
    ndDataLst
    getDataNdByPAttr ( xmlStd* xmlParse, const char* ndNm, unsigned int lvlNo,
                       const char* pNdNm, const char* pNdAttrNm, const char* pNdAttrVal )
    {
        if ( !xmlParse )
        {
            return NULL;
        }

        xmlStd* tInXmlParse_p = xmlParse;
        const char* tInNdNm = ndNm;
        unsigned int tInLvlNo = lvlNo;
        const char* tInPNdNm = pNdNm;
        const char* tInPNdAttrNm = pNdAttrNm;
        const char* tInPNdAttrVal = pNdAttrVal;
        return tInXmlParse_p->getNdByPAttr ( tInNdNm, tInLvlNo, tInPNdNm, tInPNdAttrNm,
                                             tInPNdAttrVal );
    }

    //返回节点名指针
    const char*  getNdNm ( entireData* usrData )
    {
        if ( !usrData )
        {
            return NULL;
        }

        return usrData->repData.name;
    }

    //返回节点值中简单值个数
    int  getNdPureValNum ( entireData* usrData )
    {
        if ( !usrData )
        {
            return -1;
        }

        return usrData->repData.pureValCnt;
    }

    //返回包含的分量数，-1失败，dataType:0-numeric, 1-str
    //dBufLng=0时仅返回分量数
    int  getNdVal ( entireData* usrData, int* dataType, void* dBuf, int dBufLng )
    {
        if ( !usrData )
        {
            return -1;
        }

        int tRet = usrData->repData.pureValCnt;

        if ( !dBufLng )
        {
            return tRet;
        }

        if ( ( !dBuf ) || ( !dataType ) )
        {
            return -1;
        }

        if ( dBufLng < ( int ) tRet )
        {
            //          *dBuf = NULL;
            return -1;
        }

        *dataType = usrData->repData.valType;

        if ( 1 == usrData->repData.valType ) //str
        {
            for ( int i = 0; i < tRet; i++ )
            {
                ( ( char** ) dBuf ) [i] = usrData->repData.dataBdy.usrStrData[i];
            }
        }
        else if ( !usrData->repData.valType )
        {
            for ( int i = 0; i < tRet; i++ )
            {
                ( ( int* ) dBuf ) [i] = usrData->repData.dataBdy.usrNumData[i];
            }
        }
        else
        {
            //          *dBuf = NULL;
            return -1;
        }

        return tRet;
    }

    int  getAttrNum ( entireData* usrData )
    {
        if ( !usrData )
        {
            return -1;
        }

        return usrData->dtlDataCnt;
    }

    //返回属性名字符串指针，attrOrdNo从0开始
    const char*  getAttrNm ( entireData* usrData, unsigned int attrOrdNo )
    {
        if ( ( !usrData ) || ( attrOrdNo > MAX_DSU_NUM - 1 ) )
        {
            return NULL;
        }

        return usrData->dtlData[attrOrdNo].name;
    }


    int  getAttrPureValNum ( entireData* usrData, const char* attrNm )
    {
        if ( ( !usrData ) || ( !attrNm ) )
        {
            return -1;
        }

        unsigned int i;

        for ( i = 0; i < usrData->dtlDataCnt; i++ )
        {
            if ( !strcmp ( usrData->dtlData[i].name, attrNm ) )
            {
                return usrData->dtlData[i].pureValCnt;
            }
        }

        //if (i == usrData->dtlDataCnt)
        return -1;

    }

    //返回包含的分量数，dataType:0-numeric, 1-str
    //dBufLng=0时仅返回分量数
    int  getAttrVal ( entireData* usrData, const char* attrNm, int* dataType, void* dBuf,
                      int dBufLng )
    {
        if ( ( !usrData ) || ( !attrNm ) )
        {
            return -1;
        }

        unsigned int i;

        for ( i = 0; i < usrData->dtlDataCnt; i++ )
        {
            if ( !strcmp ( usrData->dtlData[i].name, attrNm ) )
            {
                break;
            }
        }

        if ( i == usrData->dtlDataCnt )
        {
            return -1;
        }

        int tRet = usrData->dtlData[i].pureValCnt;

        if ( !dBufLng )
        {
            return tRet;
        }

        if ( ( !dBuf ) || ( !dataType ) )
        {
            return -1;
        }

        if ( dBufLng < tRet )
        {
            //          *dBuf = NULL;
            return -1;
        }

        *dataType = usrData->dtlData[i].valType;

        if ( 1 == usrData->dtlData[i].valType ) //str
        {
            for ( int j = 0; j < tRet; j++ )
            {
                ( ( char** ) dBuf ) [j] = usrData->dtlData[i].dataBdy.usrStrData[j];
            }
        }
        else if ( !usrData->dtlData[i].valType )
        {
            for ( int j = 0; j < tRet; j++ )
            {
                ( ( int* ) dBuf ) [j] = usrData->dtlData[i].dataBdy.usrNumData[j];
            }
        }
        else
        {
            //          *dBuf = NULL;
            return -1;
        }

        return tRet;
    }

    /*  //功能：根据层号和节点名，以及祖父和父节点名为name的属性取值字符串，找符合条件的所有节点，
        //      并返回它们的所有应用数据，最多支持XML_MAX_ATTRNUM + 1个节点。不支持多个符合
        //      要求的父节点（两棵子树）： 如父亲代表子系统，属性name值是子系统的名称，则不
        //      允许目标文件中出现多个相同的子系统
        //参2：输入，节点名
        //参3：输入，节点层号
        //参4：输入，父节点名
        //参5：输入，祖父节点name属性取值字符串
        //参6：输入，父节点name属性取值字符串
        //返回值：符合查找条件的所有目标节点的取值列表，这些节点包含节点自身和所有属性的应用数据值
        ndDataLst
            getFstDataNdByPPAttrNmVal(xmlStd* xmlParse, const char* ndNm, unsigned int lvlNo, const char* ppNdNm,
                const char* pNdNm, const char* ppNdNmAttrVal, const char* pNdNmAttrVal)
        {
            if ((!xmlParse) || (!ppNdNm) || (!pNdNm))
                return NULL;

            xmlStd* tInXmlParse_p = xmlParse;
            const char* tInNdNm = ndNm;
            unsigned int tInLvlNo = lvlNo;
            const char* tInPPNdNm = ppNdNm;
            const char* tInPNdNm = pNdNm;
            const char* tInPPNdNmAttrVal = ppNdNmAttrVal;
            const char* tInPNdNmAttrVal = pNdNmAttrVal;
            ndDataLst pDataLst = NULL;
            pDataLst = tInXmlParse_p->getNdByPAttrNmVal(tInPNdNm, tInLvlNo - 1, tInPPNdNm, tInPPNdNmAttrVal);

            entireData pUsrData[XML_MAX_WIDTH];
            int pUsrDataIdx[XML_MAX_WIDTH];
            memset(pUsrData, 0, sizeof(entireData));
            memset(pUsrDataIdx, -1, sizeof(int));
            int pUsrDataNum = 0;
            int dataType = 0;
            char* strVal = NULL;
            pUsrDataNum = getUsrData(pDataLst, pUsrData);
            int i;
            for (i = 0; i < pUsrDataNum; i++)
            {
                getAttrVal(&pUsrData[i], "name", &dataType, strVal, 1);
                if (!strcmp(tInPNdNmAttrVal, strVal))//找到父亲，pUsrData[i]
                    pUsrDataIdx[i] = i;//记下找到的索引i->pUsrData[i]
            }
            if (pUsrDataNum == i)
                return NULL;

            //再找到的父亲中找目标节点，找到第一个目标节点就返回，不返回所有目标节点
            for (i = 0; i < pUsrDataNum; i++)
            {
                if (-1 == pUsrDataIdx[i])
                    continue;
                if (!(tInXmlParse_p->getNdByPAttrNmVal(tInNdNm, tInLvlNo, tInPNdNm, tInPNdNmAttrVal)))
                    continue;
                else
                    return tInXmlParse_p->getNdByPAttrNmVal(tInNdNm, tInLvlNo, tInPNdNm, tInPNdNmAttrVal);
            }
            if (pUsrDataNum == i)
                return NULL;
            return NULL;
        }*/

    //返回值：0-成功；1（非0）-失败
    int setAppRlRestrict ( char** rstrctClause, char** rstrct, unsigned char clauseNum )
    {
        if ( ( !rstrctClause ) || ( !rstrct ) || ( !clauseNum ) || ( ! ( *rstrct ) )
             || ( ! ( *rstrctClause ) ) )
        {
            return 1;
        }

        char** tInRstrctClause = rstrctClause;
        char** tInRstrct = rstrct;
        unsigned char tInclauseNum = clauseNum;

        if ( 1 == tInclauseNum )
        {
            *tInRstrctClause = *tInRstrct;
        }
        else
        {
            for ( int i = 0; i < tInclauseNum; i++ )
            {
                tInRstrctClause[i] = tInRstrct[i];
            }
        }

        return 0;
    }

    unsigned int getIDsByPID ( xmlStd* xmlParse, const char* ndNm, unsigned int ele_TgtPID,
                               unsigned int* ele_TgtID_p, unsigned int IDNum )
    {
        return xmlParse->getNdIDsByPNdID ( ndNm, ele_TgtPID, ele_TgtID_p, IDNum );
    }

    unsigned int getIDsByAttr ( xmlStd* xmlParse, const char* ndNm, unsigned int lvlNo,
                                const char* attrNm,
                                const char* attrNmVal, unsigned int* ele_TgtID_p, unsigned int IDNum )
    {
        return xmlParse->getNdIDsByNDAttr ( ndNm, lvlNo, attrNm, attrNmVal, ele_TgtID_p, IDNum );
    }


}
