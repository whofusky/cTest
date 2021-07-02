#pragma once

#include "dataRepresentation.h"

#define MAX_DSU_NUM 10
#define MAX_PUREDATA_NUM 10

namespace XML_DTATA_REPRSENTATION
{
    using namespace DATA_REPRSENTATION;
    //  using namespace std;

    struct commData
    {
        const char* name;
        int valType;//0-int,1-str
        unsigned int pureValCnt;//分量数
        union commBdy
        {
            int usrNumData[MAX_PUREDATA_NUM];
            char* usrStrData[MAX_PUREDATA_NUM];
        } dataBdy;
    };

    //repType: str-
    struct entireData   //一个完整意义的数据
    {
        commData repData;
        unsigned int dtlDataCnt;    //细节数据个数和数据---对于XML文件，对应一个节点的所有属性元素
        commData dtlData[MAX_DSU_NUM];
    };

    extern int getUsrData ( ndDataLst ndData_p, entireData* usrData );
    //  extern int getNdsInfo(ndDataLst ndDataSet, const char** names, int nameNum);
}