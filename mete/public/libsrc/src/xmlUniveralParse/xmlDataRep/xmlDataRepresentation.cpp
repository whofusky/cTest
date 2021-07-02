
#include <stdlib.h>
#include<string.h>
#include "dataRepresentation.h"
#include "xmlDataRepresentation.h"

namespace XML_DTATA_REPRSENTATION
{
    using namespace DATA_REPRSENTATION;

    //对XML的数据表示输出接口
    //输出参数为空时，功能是返回数据列表的数据存储块个数，对xml文件，一个数据存储
    //块代表一个节点，内含节点名和节点值对，以及全部的属性名和属性值对
    //失败返回-1
    int getUsrData ( ndDataLst ndData_p, entireData* usrData )
    {
        if ( !ndData_p )
        {
            return -1;
        }

        ndDataLst tInNdData_p = ndData_p;
        dataSB* tDStgBlk_p;//数据存储块指针
        stgSBBdy* tSGM_CmnBdy_p;//dataSu-dataGrp-mDataImg公共存储体指针
        mDataImg* tMStgBdy_p;//元数据存储体指针
        unsigned int tMImgNum;//元数据映像（void* mImg）数
        unsigned int tDStgBlkNum;
        entireData* tOutUsrData_p = usrData;

        tDStgBlkNum = tInNdData_p->sbNum;

        if ( !tOutUsrData_p )
        {
            return tDStgBlkNum;
        }

        for ( unsigned int i = 0; i < tDStgBlkNum; i++ ) //逐个节点处理
        {
            tDStgBlk_p = tInNdData_p->sbBdy[i];//->dStgLst[0]->GRPBdy.dStgLst[1].mImg
            unsigned int j = 0;
            tOutUsrData_p[i].dtlDataCnt = 0;
            tSGM_CmnBdy_p = tDStgBlk_p->dStgLst[0];

            //节点为非叶子其值为空，按字符串指针型处理，若节点值和属性均无，则无存储块
            if ( !tSGM_CmnBdy_p )
            {
                tOutUsrData_p[i].repData.valType = 1;//字符串
                tOutUsrData_p[i].repData.pureValCnt = 0;//值空
                tOutUsrData_p[i].repData.name = tDStgBlk_p->name;
                tOutUsrData_p[i].repData.dataBdy.usrStrData[0] = NULL;//值无效
                j = 1;
            }

            for ( ; j < tDStgBlk_p->dStgNum; j++ ) //节点内逐个存储块处理
            {
                tSGM_CmnBdy_p = tDStgBlk_p->dStgLst[j];//->GRPBdy.dStgLst[1].mImg
                tOutUsrData_p[i].dtlDataCnt = tDStgBlk_p->dStgNum - 1;//属性数

                //存储块转换为映像组
                tMImgNum = tSGM_CmnBdy_p->GRPBdy.dStgNum;

                //if (ndAppData_p->sbkType > 100)--代表一个节点，必成立，不判断
                if ( tSGM_CmnBdy_p->GRPBdy.dGType > 100 ) //列表内可能是映像（单值），也可能是映像组（多值）
                {
                    tMStgBdy_p = tSGM_CmnBdy_p->GRPBdy.dStgLst;    //->mImg
                }
                else
                {
                    tMStgBdy_p = ( mDataImg* ) tSGM_CmnBdy_p;    //->mImg
                }

                for ( unsigned int k = 0; k < tMImgNum; tMStgBdy_p++, k++ )
                {
                    if ( !j ) //保存至代表值域内，对XML每个纯值对应一块映像，映像内的元数据个数永为1
                    {
                        tOutUsrData_p[i].repData.name = tDStgBlk_p->dStgLst[j]->mImgBdy.name;
                        tOutUsrData_p[i].repData.pureValCnt = tMImgNum;

                    }
                    else//保存至细节值域内--第j个细节数据
                    {

                        tOutUsrData_p[i].dtlData[j - 1].name = tDStgBlk_p->dStgLst[j]->mImgBdy.name;
                        tOutUsrData_p[i].dtlData[j - 1].pureValCnt = tMImgNum;
                    }

                    for ( unsigned char m = 0; m < tMStgBdy_p->mNum;
                          m++ ) //对XML节点数据tMStgBdy_p->mNum永为1
                    {
                        switch ( tMStgBdy_p->mType )
                        {
                            case NUM_ZRO:
                            case NUM_POS:
                            case NUM_ZP:
                            case NUM_NEG:
                            case NUM_ALL:
                                if ( !j ) //保存至代表值域内
                                {
                                    tOutUsrData_p[i].repData.valType = 0;//numeric
                                    tOutUsrData_p[i].repData.dataBdy.usrNumData[k] = * ( ( int* ) ( tMStgBdy_p->mImg ) + m );
                                }
                                else//保存至细节值域内--第j个细节数据，对XML节点数据tMStgBdy_p->mNum永为1
                                {
                                    tOutUsrData_p[i].dtlData[j - 1].valType = 0;//numeric
                                    tOutUsrData_p[i].dtlData[j - 1].dataBdy.usrNumData[k] = * ( ( int* ) (
                                            tMStgBdy_p->mImg ) + m );
                                }

                                break;

                            case PTR_NUL:
                            case PTR_FUL:
                            case PTR_ALL:
                                if ( !j ) //保存至代表值域内
                                {
                                    tOutUsrData_p[i].repData.valType = 1;//str
                                    tOutUsrData_p[i].repData.dataBdy.usrStrData[k] = * ( ( char** ) ( tMStgBdy_p->mImg ) + m );
                                }
                                else//保存至细节值域内--第j个细节数据，对XML节点数据tMStgBdy_p->mNum永为1
                                {
                                    tOutUsrData_p[i].dtlData[j - 1].valType = 1;//str
                                    tOutUsrData_p[i].dtlData[j - 1].dataBdy.usrStrData[k] = * ( ( char** ) (
                                            tMStgBdy_p->mImg ) + m );
                                }

                                break;

                            default:
                                break;
                        }
                    }
                }
            }
        }

        loadOffMemMgr ( ndData_p->stub, 1 );
        return tDStgBlkNum;
    }

    /*  //得到节点信息：节点数和节点名列表，出错返回-1
        //nameNum=0时，仅返回节点数
        int getNdsInfo(ndDataLst ndDataSet, const char** names, int nameNum)
        {
            int tRet = ndDataSet->sbNum;
            if (!nameNum)
                return tRet;
            if (!names)
                return -1;
            for (int i = 0; i < tRet; i++)
                names[i] = ndDataSet->sbBdy[i]->name;
            return tRet;
        }*/
}