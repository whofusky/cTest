#pragma once

#include <stdlib.h>
#include "tinyxml2.h"
#include"dataRepresentation.h"
#include "commAppRlItfc.h"

#define XML_MAX_LVLNUM      (10)
#define XML_MAX_NEIGHBOR    (50)
#define XML_MAX_WIDTH       (10000)
#define XML_MAX_ATTRNUM     (10)
#define DEFAULTRL_MAX_NUM   (2)
//#define INFOFIELD_NUM     (3)
#define INDICATOR_NUM       (6)
#define STDNODE_ATTR_NUM    (2)
#define SPTR_MAX_LNG        (4)

namespace WPFC_Sys_20
{
    using namespace tinyxml2;
    using namespace std;
    using namespace DATA_REPRSENTATION;
    using namespace COMM_APP_ITFC;

    enum valType
    {
        VAL_NULL = 0,   //空值词
        NODE_SET,       //节点集合型
        NUM_WORD,       //数字型词
        STR_OPT,        //字符型选项
        STR_WORD,       //字符型词
        NUM_ARRAY,      //数字型队列
        STR_ARRAY,      //字符型队列
        MIX_ARRAY       //数字型词和字符型词组成的队列
    };
    enum ndEleType
    {
        ROOT = 0,
        NONLEAF,
        LEAF
    };
    enum XMLRL_PARSE_ERROR
    {
        XMLRL_SUCCESS = 0,

        XML_NO_ATTRIBUTE,
        XML_WRONG_ATTRIBUTE_TYPE,
        XML_ERROR_FILE_NOT_FOUND,
        XML_ERROR_FILE_COULD_NOT_BE_OPENED,
        XML_ERROR_FILE_READ_ERROR,
        UNUSED_XML_ERROR_ELEMENT_MISMATCH,  // remove at next major version
        XML_ERROR_PARSING_ELEMENT,
        XML_ERROR_PARSING_ATTRIBUTE,
        UNUSED_XML_ERROR_IDENTIFYING_TAG,   // remove at next major version
        XML_ERROR_PARSING_TEXT,
        XML_ERROR_PARSING_CDATA,
        XML_ERROR_PARSING_COMMENT,
        XML_ERROR_PARSING_DECLARATION,
        XML_ERROR_PARSING_UNKNOWN,
        XML_ERROR_EMPTY_DOCUMENT,
        XML_ERROR_MISMATCHED_ELEMENT,
        XML_ERROR_PARSING,
        XML_CAN_NOT_CONVERT_TEXT,
        XML_NO_TEXT_NODE,
        XML_ERROR_COUNT,

        XMLRL_FILE_UNKNOWN = 0xFF00,    //非本系统规则文件
        XMLRL_FUN_PARAM,                //函数参数错
        XMLRL_MEM_ALLOC,                //内存分配错
        XMLRL_FMT_NAME,//格式错：节点无NAME子节点
        XMLRL_ATTR_NAME,//属性名错
        XMLRL_NAME_NULL,//NAME子节点值为空
        XMLRL_FMT_ROOT,//格式错：节点无root节点
        XMLRL_ROOT_NONAME,//根结点无名字
        XMLRL_FMT_OPT,//格式错：无OPT属性
        XMLRL_OPT_UNDEF,//OPT取值未定义
        XMLRL_ATTR_NONAME,//无属性名字
        XMLRL_ATTR_NUM,//属性数量过多
        XMLRL_LVL_LARGE,//层数过大
        XMLRL_LVL_INTERCAL,//层不连续
        XMLRL_CHILD_NAME,   //同父规则的子节点名称规则重复
        XMLRL_OBJATTR_INST,     //xmlStd类对象属性值已实例化
        XMLRL_ATTR_FMT,             //属性格式错误
        XMLRL_FMT_UNKNOWN,
        XMLRL_FILE_END,             //文件结束
        XMLRL_NODE_ATTR,            //目标节点规则（规则的属性）不存在
        XMLRL_NODE_CHKORDER,            //目标节点多个检查规则顺序错误
        XMLCHK_ERROR_EMPTY_DOCUMENT,
        XMLCHK_FILE_UNKNOWN,
        XMLCHK_CHK_NORULE,              //无规则可供检查
        XMLCHK_NODE_UNKNOWN,
        XMLCHK_FMT_ROOT,                //目标节点根节点格式错，找不到根
        XMLCHK_TYPE_NODE,               //目标节点类型错误，叶子，非叶子
        XMLCHK_NODE_NONAME,             //目标节点无名字
        XMLCHK_NODE_SAMENAME,           //目标节点同名
        XMLCHK_NODE_PARENT,             //目标节点父节点错
        XMLCHK_ROOT_ATTR,               //目标根节点不应存在属性
        XMLCHK_ROOT_VALUE,              //目标根节点不应存在简单值
        XMLCHK_STRUCT_LVL,              //目标节点层号错
        XMLCHK_ROOT_MULTI,              //目标节点出现多根
        XMLCHK_LVL_NAME,                //目标节点在规定层中不存在
        XMLCHK_STRUCT_PARENT,           //目标节点的父节点不满足规则
        XMLCHK_CHILD_LARGE,             //目标节点的子节点过多
        XMLCHK_CHILD_LACK,              //少必备目标节点
        XMLCHK_ATTR_LARGE,
        XMLCHK_NODE_LVL,
        XMLCHK_NODE_NONLEAF,            //目标节点为非叶子节点
        XMLCHK_NODE_NONVALUE,           //目标节点无简单值或目标节点值未通过检查
        XMLCHK_NODE_NAME,
        XMLCHK_NODE_VALUE,              //目标节点值未通过检查
        XMLCHK_ATTR_LACK,               //目标节点没有必备属性
        XMLCHK_ATTR_UNKNOWN,            //规则中无此属性名，无法识别目标节点属性
        XMLCHK_ATTR_NAME,               //目标节点属性名称错误
        XMLCHK_ATTR_VALUE,              //目标节点属性值错误
        XMLCHK_LVL_LARGE,               //目标节点数量超过本层允许最大数
        XMLCHK_TRCND_NONE,              //跟踪节点不存在
        XMLCHK_UNKNOWN_ERROR = 0xFFFF//未知错误
    };
    /////////////////////////////////////////////
    ////应用级通用数据访问模块
    //
    //数据原型定义，用于xml与应用程序的数据类型接口
    //以下通用数据类型结构，在XML解析中的对应关系如下：
    //1. 一个节点或多个同名节点均对应存储块：
    //    （1）groupNum，节点个数；（2）SUNum，为每个节点内部，包含的属性数与节点值的总数；
    //    （3）stgType，表示一个节点内部是否包含个属性以及节点值
    //
    //2. 一个属性或多个属性，以及一个节点均对应存储单元：
    //    （1）groupNum，属性或节点个数，若为节点只能为1；
    //    （2）fieldNum，同一属性或节点中，取值包含的分量数，目前保留，=1，为以后支持矢量类型物理量时使用，
    //                   每个分量由同一类元数据或元数据数组组成；
    //    （3）stgType，属性或节点分量值的分量类型，即用于区分fieldNum和groupNum
    //
    //3. 一个属性或节点值分量，均对应元数据簇：
    //   （1）mType，元数据类型；（2）mLng，元数据单元长度，防止因机器字长差别而改变
    //   （3）metaNum，元数据个数，当一个分量由分隔符分开时，他们变分成多个简单元数据，形成元数据数组；

    enum subSntnsType
    {
        SPTR = 0,       //单参sptr:子句
        NON_NULL,   //非空
        BANDUP,     //禁重
        NUM,        //双参num:子句
        STR,            //变重str:子句
        UNKNOWN//未知
    };

    struct valChkMethod
    {
        subSntnsType chkMethod;
        //值标准中规定的使用的分隔符（若有），值中分隔出的多个值使用同一检查参数，最长4个字符，结束带'/0'
        char valStdSprt[SPTR_MAX_LNG + 1];
        union
        {
            struct
            {
                long long minVal;
                long long maxVal;
            } numChk;
            OPTGRP strOptGrp;
        } ChkParam;
        valChkMethod* nxt;//下一子句
    };
    struct subSntnsDsrb
    {
        subSntnsType type;
        char* sntns;
        char* xprnStart;
        char* xprnEnd;
        char* owner;
        subSntnsDsrb* nxt;
    };

    struct attrRl;
    struct eleRl
    {
        const char* eleNmRl;        //元素名
        ndEleType ndType;           //节点类型，根、非叶子、叶子
        int isNeed;                 //必备性

        unsigned char attrRlNum;    //元素具有的属性规则数
        attrRl* attrRlLst;          //元素具有的属性规则列表

        valType eleValType;         //值类型
        const char* eleValRl;       //元素值规则字符串
    };
    struct attrRl
    {
        const char* atrrNmRl;   //属性名规则
        const char* atrrValRl;  //属性值规则字符串
        valType attrValType;    //属性值类型
        int isNeed;         //必备性
        eleRl* ownerRl;     //属性所属元素规则
    };

    struct tgtNd;
    struct tgtAttr
    {
        const char* tgtAtrrNm;      //目标属性名规则
        const char* tgtAtrrValStr;  //目标属性值规则字符串
        tgtNd* ownerNd;             //属性所属目标节点
    };
    struct tgtNd
    {
        const char* tgtNdNm;        //目标节点名
        ndEleType tgtNdType;        //目标节点类型，根、非叶子、叶子

        char* tgtNdValStr;  //目标节点值字符串，允许规格化
        unsigned char tgtNdAttrNum;         //目标节点属性数
        tgtAttr* tgtNdAttrLst;      //目标节点属性列表
    };
    struct ele_Std//规则元素节点
    {
        XMLElement* ele_p;
        eleRl* std_p;
        //
        ele_Std* pEleStd_p;
        //
        unsigned char childNum;
        ele_Std** cEleStd_p;//存放cEleStd的地址列表
        //
        ele_Std* nxt;
    };
    struct ele_Tgt//目标元素节点，代替tgtParseEle
    {
        XMLElement* tgtEle_p;
        tgtNd* tgtNd_p;
        ele_Std* tgtNdStd_p;
        //
        ele_Tgt* pTgtNd_p;
        //
        unsigned char childNum;
        ele_Tgt** cTgtNd_p;
        //
        dataSB ndAppData;////静态分配应用值挂接点，节点通过检查的有效数据值
        unsigned int ele_TgtID;
        ele_Tgt* nxt;
    };
    struct chkTrc
    {
        //跟踪过程数据
        ele_Tgt* trcNd_p;
        unsigned char lvlNo;
        unsigned char brotherNum;
        const char** brotherNmStr;
    };

    //
    ////应用级通用数据访问模块
    /////////////////////////////////////////////
    class xmlStd
    {
        private:
            XMLDocument* rlDoc_p;
            XMLDocument* tgtDoc_p;

            //记录本类内方法分配的零散内存，在不析构对象的情况下释放这些内存
            XMLRL_PARSE_ERROR chkRlDoc ( XMLDocument* stdDoc_p,
                                         const char* rlFileId,
                                         const char* tgtRlId,
                                         XMLElement** stdEle_out_p
                                       );
            XMLRL_PARSE_ERROR setOneNdStd ( XMLElement* inRlEle_p,
                                            ele_Std* parentEleStd_p,
                                            ele_Std* tgtEleStd_p
                                          );
            XMLRL_PARSE_ERROR setOneAttrStd ( eleRl* eStd_p,
                                              XMLElement* rlEle_p,
                                              attrRl* attrPos,
                                              eleRl* parentRl_p
                                            );
            XMLRL_PARSE_ERROR setNdAttrsStd ( eleRl* eStd_p,
                                              XMLElement* rlEle_p,
                                              eleRl* parentRl_p
                                            );
            XMLRL_PARSE_ERROR setRootStd ( XMLElement* fileRootEle_p,
                                           ele_Std** std_out_p
                                         );
            XMLRL_PARSE_ERROR setInfoLvlNdsStd();
            XMLRL_PARSE_ERROR setLvlNdsStd ( unsigned int stdLvl );

            //////////////////////规则文件分析部分
            unsigned char stdLvlNum;    //规则的总层次
            ele_Std ele_StdLvlHdr[XML_MAX_LVLNUM];
            chkTrc tgtChkTrc;
            unsigned char tgtLvlNum;    //目标文件总层次
            ele_Tgt ele_TgtLvlHdr[XML_MAX_LVLNUM];

            //////////////////////目标文件检查部分
            //功能：置对象实例tgtChkTrc内部元素为空，无参数，无返回值
            void clrTgtChkTrcNd() //内联
            {
                if ( tgtChkTrc.brotherNmStr )
                {
                    free ( tgtChkTrc.brotherNmStr );
                }

                tgtChkTrc.brotherNmStr = NULL;
                tgtChkTrc.brotherNum = 0;
                tgtChkTrc.lvlNo = 0;
                tgtChkTrc.trcNd_p = NULL;
            }
            XMLRL_PARSE_ERROR buildTrcNd ( XMLElement* tgtEle_p,
                                           ele_Tgt* pEleTgt_p,
                                           ele_Tgt* eleStd_p,
                                           unsigned char tgtNdLvl
                                         );
            XMLRL_PARSE_ERROR alsTrcNd();
            XMLRL_PARSE_ERROR chkTrcNdValVld();
            XMLRL_PARSE_ERROR chkTrcNdAttrsVld();

        public:
            xmlStd();
            ~xmlStd();
            XMLRL_PARSE_ERROR loadXmlStd ( const char* tgtFileNm );
            XMLRL_PARSE_ERROR chkTgtXmlFile ( const char* tgtFileNm_p );
            //根据层号和节点名和指定的属性名称及其取值，找符合条件的所有节点，并返回它们的所有数据
            ndDataLst  getNdByAttr ( const char* ndNm,
                                     unsigned int lvlNo,
                                     const char* attrNm,
                                     const char* attrNmVal
                                   );
            //根据层号和节点名，以及它的父节点的名称属性取值，找符合条件的所有节点，并返回它们的所有数据
            ndDataLst  getNdByPAttrNmVal ( const char* ndNm,
                                           unsigned int lvlNo,
                                           const char* pNdNm,
                                           const char* pNdAttrNmVal
                                         );
            ndDataLst getNdByPAttr ( const char* ndNm,
                                     unsigned int lvlNo,
                                     const char* pNdNm,
                                     const char* pNdAttrNm,
                                     const char* pNdAttrVal
                                   );
            unsigned int getNdIDsByPNdID ( const char* ndNm,
                                           unsigned int ele_TgtPID,
                                           unsigned int* ele_TgtID_p,
                                           unsigned int IDNum
                                         );
            ndDataLst getNdByNdID ( unsigned int ndID );
            unsigned int getNdIDsByNDAttr ( const char* ndNm,
                                            unsigned int lvlNo,
                                            const char* attrNm,
                                            const char* attrNmVal,
                                            unsigned int* ele_TgtID_p,
                                            unsigned int IDNum
                                          );
    };
}





