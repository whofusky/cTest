#define _CRT_SECURE_NO_DEPRECATE

//#include "tinyxml2.h"
#include "xmlParse.h"
#include "appRlDef.h"
#include <math.h>

namespace WPFC_Sys_20
{
    using namespace tinyxml2;
    using namespace std;

    xmlStd::xmlStd()
    {
        rlDoc_p = NULL;
        tgtDoc_p = NULL;

        stdLvlNum = 0;

        //各层第一个节点ele_StdLvlHdr[i]为根节点或最左节点，它们自身为挂接点，不需另外分
        //配空间，但它的节点规则需要另外分配空间，即（ele_StdLvlHdr[i]->std_p）
        for ( int i = 0; i < XML_MAX_LVLNUM; i++ )
        {
            ele_StdLvlHdr[i].cEleStd_p = NULL;
            ele_StdLvlHdr[i].childNum = 0;
            ele_StdLvlHdr[i].ele_p = NULL;
            ele_StdLvlHdr[i].nxt = NULL;
            ele_StdLvlHdr[i].pEleStd_p = NULL;
            ele_StdLvlHdr[i].std_p = NULL;
        }

        tgtLvlNum = 0;

        for ( int i = 0; i < XML_MAX_LVLNUM; i++ )
        {
            ele_TgtLvlHdr[i].cTgtNd_p = NULL;
            ele_TgtLvlHdr[i].childNum = 0;
            ele_TgtLvlHdr[i].ndAppData.sbkType = STG_UNKNOWN;
            ele_TgtLvlHdr[i].ndAppData.dStgNum = 0;

            for ( int j = 0; j < XML_MAX_ATTRNUM + 1; j++ )
            {
                ele_TgtLvlHdr[i].ndAppData.dStgLst[j] = NULL;
            }

            ele_TgtLvlHdr[i].nxt = NULL;
            ele_TgtLvlHdr[i].pTgtNd_p = NULL;
            ele_TgtLvlHdr[i].tgtEle_p = NULL;
            ele_TgtLvlHdr[i].tgtNdStd_p = &ele_StdLvlHdr[0];
            ele_TgtLvlHdr[i].tgtNd_p = NULL;
        }

        tgtChkTrc.lvlNo = 0;
        tgtChkTrc.brotherNmStr = NULL;
        tgtChkTrc.brotherNum = 0;
        tgtChkTrc.trcNd_p = NULL;

        //分配第一个内部管理块
        //  memHdBlk_p = (memMgr*)calloc(1, sizeof(memMgr));
        //  curMemPos_p = memHdBlk_p;
    }
    xmlStd::~xmlStd() //释放分配的空间
    {
        if ( rlDoc_p )
        {
            delete rlDoc_p;
        }

        if ( tgtDoc_p )
        {
            delete tgtDoc_p;
        }

        void* tFreeBlk_p, *temp;

        for ( int i = 0; i <= stdLvlNum; i++ )
        {
            tFreeBlk_p = &ele_StdLvlHdr[i];

            while ( tFreeBlk_p )
            {
                temp = ( ( ele_Std* ) tFreeBlk_p )->nxt;

                if ( ( ( ele_Std* ) tFreeBlk_p )->std_p )
                {
                    if ( ( ( ele_Std* ) tFreeBlk_p )->std_p->attrRlLst )
                    {
                        free ( ( ( ele_Std* ) tFreeBlk_p )->std_p->attrRlLst ); //1块连续内存
                        ( ( ele_Std* ) tFreeBlk_p )->std_p->attrRlLst = NULL;
                    }

                    free ( ( ( ele_Std* ) tFreeBlk_p )->std_p );
                    ( ( ele_Std* ) tFreeBlk_p )->std_p = NULL;
                }

                if ( ( ( ele_Std* ) tFreeBlk_p )->cEleStd_p )
                {
                    free ( ( ( ele_Std* ) tFreeBlk_p )->cEleStd_p ); //1块连续内存
                    ( ( ele_Std* ) tFreeBlk_p )->cEleStd_p = NULL;
                }

                if ( tFreeBlk_p != &ele_StdLvlHdr[i] )
                {
                    free ( tFreeBlk_p );
                }

                tFreeBlk_p = temp;
            }
        }

        for ( int i = 0; i <= tgtLvlNum; i++ )
        {
            tFreeBlk_p = &ele_TgtLvlHdr[i];

            while ( tFreeBlk_p )
            {
                temp = ( ( ele_Tgt* ) tFreeBlk_p )->nxt;

                if ( ( ( ele_Tgt* ) tFreeBlk_p )->cTgtNd_p )
                {
                    free ( ( ( ele_Tgt* ) tFreeBlk_p )->cTgtNd_p );
                    ( ( ele_Tgt* ) tFreeBlk_p )->cTgtNd_p = NULL;
                }

                if ( ( ( ele_Tgt* ) tFreeBlk_p )->tgtNd_p )
                {
                    if ( ( ( ele_Tgt* ) tFreeBlk_p )->tgtNd_p->tgtNdAttrLst )
                    {
                        free ( ( ( ele_Tgt* ) tFreeBlk_p )->tgtNd_p->tgtNdAttrLst ); //一块连续内存
                        ( ( ele_Tgt* ) tFreeBlk_p )->tgtNd_p->tgtNdAttrLst = NULL;
                    }

                    free ( ( ( ele_Tgt* ) tFreeBlk_p )->tgtNd_p );
                    ( ( ele_Tgt* ) tFreeBlk_p )->tgtNd_p = NULL;
                }

                for ( unsigned int j = 0; j < ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgNum; j++ )
                {
                    if ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j] )
                    {
                        //检查通用数据按何类型存储
                        if ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->GRPBdy.dGType == STG_NVMX ) //多值内含多块元数据映像
                        {
                            mDataImg* tDImg = ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->GRPBdy.dStgLst;
                            dataGRP tGrp = ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->GRPBdy;

                            while ( tGrp.dStgNum-- )
                            {
                                //释放字符串首地址或整数的存储空间
                                if ( tDImg->mNum )
                                {
                                    free ( tDImg->mImg );
                                    tDImg++;
                                }
                            }

                            free ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->GRPBdy.dStgLst );
                        }
                        else if ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->mImgBdy.mType < STG_UNKNOWN )
                        {
                            while ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->mImgBdy.mNum-- )
                            {
                                //mImgBdy.mImg值为映像地址指针，指向的是字符串首地址空间或整数存储空间

                                free ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j]->mImgBdy.mImg );
                            }
                        }

                        free ( ( ( ele_Tgt* ) tFreeBlk_p )->ndAppData.dStgLst[j] );
                    }
                }

                if ( tFreeBlk_p != &ele_TgtLvlHdr[i] )
                {
                    free ( tFreeBlk_p );
                }

                tFreeBlk_p = temp;
            }
        }

        if ( tgtChkTrc.brotherNmStr )
        {
            free ( tgtChkTrc.brotherNmStr );
        }

        //释放零散分配的内存
        /*  memMgr* tMemGbg_p;
            tMemGbg_p = memHdBlk_p;
            while (tMemGbg_p)
            {
                curMemPos_p = tMemGbg_p->nxt;
                if (tMemGbg_p->memBlk_p)
                    free(tMemGbg_p->memBlk_p);
                free(tMemGbg_p);
                tMemGbg_p = curMemPos_p;
            }*/
    }

    //用于代替atoi()，同时遇到非数字返回：1，成功返回0，参2输出转换后的长长有符整型值，非法时值为0
    inline int numStrTonum ( char* str, long long* num_p, unsigned char posWeight )
    {
        if ( ( !str ) || ( !num_p ) || ( posWeight < 2 ) || ( posWeight > 16 ) )
        {
            return 1;
        }

        char* tInStr = str;
        long long* tNum_p = num_p;
        unsigned char tPosWeight = posWeight;
        int tRet = 1;
        unsigned int tStrLng = strlen ( tInStr );

        if ( tStrLng > 19 )
        {
            *tNum_p = 0;
            return tRet;
        }

        if ( !tStrLng ) //str = "\0" = 0
        {
            *tNum_p = 0;
            return 0;
        }

        unsigned char tCharNum = 0;
        long long tTgtNum = 0;

        //tInStr[0]|    {tInStr[1]|...|tInStr[tStrLng]}     |tInStr[tStrLng + 1] = '\0'
        for ( unsigned int i = tStrLng - 1; i > 0; i-- )
        {
            switch ( tInStr[i] )
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    tCharNum = tInStr[i] - '0';
                    break;
                }

                case 'a':
                case 'A':
                {
                    tCharNum = 10;
                    break;
                }

                case 'b':
                case 'B':
                {
                    tCharNum = 11;
                    break;
                }

                case 'c':
                case 'C':
                {
                    tCharNum = 12;
                    break;
                }

                case 'd':
                case 'D':
                {
                    tCharNum = 13;
                    break;
                }

                case 'e':
                case 'E':
                {
                    tCharNum = 14;
                    break;
                }

                case 'f':
                case 'F':
                {
                    tCharNum = 15;
                    break;
                }

                default :
                {
                    *tNum_p = 0;
                    return tRet;
                }
            }

            tTgtNum = tTgtNum + tCharNum * ( long long ) pow ( tPosWeight, ( tStrLng - 1 - i ) );
        }

        //判断是否为符号位
        //{tInStr[0]}   |tInStr[1]|...|tInStr[tStrLng]|tInStr[tStrLng + 1] = '\0'
        if ( ( 1 == tStrLng ) && ( '-' == tInStr[0] ) ) //不是数
        {
            *tNum_p = 0;
            return tRet;
        }

        //tStrLng > 1，负数，直接支持十进制，其它表示法负数形如：-0xaBCc，输入参数str应将其转换为-aBCc
        if ( '-' == tInStr[0] )
        {
            *tNum_p = 0 - tTgtNum;
            return 0;
        }

        //tStrLng > 1，非负数
        switch ( tInStr[0] )
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                tCharNum = tInStr[0] - '0';
                break;
            }

            case 'a':
            case 'A':
            {
                tCharNum = 10;
                break;
            }

            case 'b':
            case 'B':
            {
                tCharNum = 11;
                break;
            }

            case 'c':
            case 'C':
            {
                tCharNum = 12;
                break;
            }

            case 'd':
            case 'D':
            {
                tCharNum = 13;
                break;
            }

            case 'e':
            case 'E':
            {
                tCharNum = 14;
                break;
            }

            case 'f':
            case 'F':
            {
                tCharNum = 15;
                break;
            }

            default:
            {
                *tNum_p = 0;
                return tRet;
            }
        }

        *tNum_p = tTgtNum + tCharNum * ( long long ) pow ( tPosWeight, ( tStrLng - 1 ) );
        return 0;
    }


    const char* defaultRl[DEFAULTRL_MAX_NUM] = { "non-null", "" };
    //  const char* infoLvlNm[INFOFIELD_NUM] = { "sysInfo_Rl","hstInfo_Rl","appInfo_Rl" };
    const char* stdNdAttrNm[STDNODE_ATTR_NUM] = { "opt", "valCk" };
    const char oprtr[2] = { '|', ',' };
    const char delimiter[2] = { '[', ']' };
    //const char equtr = ':';
    const char subSntnsSptr = ';';
    const char* indicator[INDICATOR_NUM] = { "sptr:", "non-null", "banDup", "num:", "str:", "" };

    const char* subSentence = NULL;
    const char* expresstion = NULL;


    //功能：检查规则文件的根节点名称、属性和子节点要求是否与规则文件相符，规则文件的
    //      根节点必须为参2指定的名称，不能有兄弟，不能有属性，必须有子节点且目标文件
    //      根节点占规则必须为第一个子节点，它的名称由参3指定
    //参1：输入，规则文档对象指针，调用tinyxml2创建的文档对象；
    //参2：输入，规则文件标识字符串，用于指明规则文件的根节点名称
    //参3：输入，规则文件中的目标规则标识字符串，用于指明目标文件根节点规则的名称
    //参4：输出，如果通过检查，返回规则文件的根元素对象指针，否则返回空
    //返回值：错误码，0为成功，输出参数非空；非0为发生错误，输出参数为空
    XMLRL_PARSE_ERROR xmlStd::
    chkRlDoc ( XMLDocument* stdDoc_p, const char* rlFileId,
               const char* tgtRlId, XMLElement** rlEle_p )
    {
        if ( ( !stdDoc_p ) || ( !rlFileId ) || ( !tgtRlId ) )
        {
            return XMLRL_FUN_PARAM;
        }

        *rlEle_p = NULL;
        XMLDocument* tInStdDoc_p = stdDoc_p;
        const char* tInRlFileId = rlFileId;
        const char* tInTgtRlId = tgtRlId;
        XMLElement* tRlEle_p = tInStdDoc_p->RootElement();//{<sysCfg_Rl>...</sysCfg_Rl>}

        if ( !tRlEle_p )
        {
            return XMLCHK_ERROR_EMPTY_DOCUMENT;
        }

        if ( tRlEle_p && tRlEle_p->GetText() ) //规则根节点中不应有文本
        {
            return XMLRL_FILE_UNKNOWN;
        }

        const char* tStdNm = tRlEle_p->Name();

        if ( !tStdNm )
        {
            return XMLRL_FILE_UNKNOWN;    //规则文件根错误，节点名不得为空
        }

        if ( strcmp ( tInRlFileId, tStdNm ) )
        {
            return XMLRL_FILE_UNKNOWN;
        }

        XMLElement* tempEle_p;

        if ( tRlEle_p->NextSiblingElement() ) //不允许有兄弟
        {
            return XMLRL_FILE_UNKNOWN;
        }

        if ( tRlEle_p->FirstAttribute() ) //不允许有属性
        {
            return XMLRL_FILE_UNKNOWN;
        }

        tempEle_p = tRlEle_p->FirstChildElement();

        if ( !tempEle_p ) //不允许无子节点
        {
            return XMLRL_FILE_UNKNOWN;
        }

        if ( strcmp ( tInTgtRlId, tempEle_p->Name() ) ) //必须是标签为<root>的子节点
        {
            return XMLRL_FILE_UNKNOWN;
        }

        *rlEle_p = tRlEle_p;
        return XMLRL_SUCCESS;
    }

    //功能：通过规则文件的根节点元素得到目标文件的根节点元素，检查其属性和子节点要求是
    //      否与规则文件相符，目标根节点规则元素必须有兄弟，不能有属性，不能有子节点
    //      目标根节点规则必须有值，即必须对目标根节点的名称作出规定
    //      为目标根节点分配目标节点规则空间，挂接到元素规则块上，元素规则块为整个对象的
    //      0层头，已在对象创建时由构造函数分配
    //参1：输入，规则文件的根节点元素指针，chkRlDoc()的输出参数；
    //参2：输出，目标根节点规则的结构体指针
    //返回值：错误码，0为成功，输出参数非空；非0为发生错误，输出参数为空
    XMLRL_PARSE_ERROR xmlStd::
    setRootStd ( XMLElement* fileRootEle_p, ele_Std** eleStd_p )
    {
        //检查输入输出参数
        if ( !fileRootEle_p )
        {
            return XMLRL_FUN_PARAM;
        }

        //  XMLRL_PARSE_ERROR tRet;
        XMLElement* tInFileRootEle_p = fileRootEle_p;
        eleRl* tStd_p = NULL;
        ele_Std* tEleStd_p = NULL;
        XMLElement* tEle_p = NULL;
        *eleStd_p = NULL;

        unsigned char tTgtRootLvl = 0;
        tEle_p = tInFileRootEle_p;
        tEle_p = tEle_p->FirstChildElement();//返回规则的{<root>...</root>}

        if ( !tEle_p )
        {
            return XMLRL_FMT_ROOT;
        }

        //检查标签为<root>的节点是否与规则文件书写要求相一致
        if ( ! ( tEle_p->NextSiblingElement() ) ) //不允许无兄弟，在目标文件中是子节点
        {
            return XMLRL_FILE_UNKNOWN;
        }

        if ( tEle_p->FirstAttribute() ) //不允许有属性
        {
            return XMLRL_FILE_UNKNOWN;
        }

        if ( tEle_p->FirstChildElement() ) //不允许有子节点
        {
            return XMLRL_FILE_UNKNOWN;
        }

        //目标文件根节点元素规则
        tEleStd_p = &ele_StdLvlHdr[tTgtRootLvl];
        tEleStd_p->ele_p = tEle_p;
        tEleStd_p->pEleStd_p = NULL;
        tEleStd_p->cEleStd_p = NULL;
        tEleStd_p->childNum = 0;
        tEleStd_p->nxt = NULL;
        tStd_p = ( eleRl* ) calloc ( 1, sizeof ( eleRl ) );

        if ( !tStd_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        tEleStd_p->std_p = tStd_p;
        tStd_p->ndType = ROOT;
        tStd_p->eleNmRl = tEle_p->GetText();//目标根节点名称规则

        if ( !tStd_p->eleNmRl )
        {
            return XMLRL_ROOT_NONAME;
        }

        tStd_p->eleValRl = defaultRl[0];//"non-null"
        tStd_p->isNeed = 1;
        tStd_p->eleValType = NODE_SET;
        tStd_p->attrRlNum = 0;

        *eleStd_p = tEleStd_p;
        return XMLRL_SUCCESS;
    }

    //功能：创建和设置给定节点规则的一个属性规则（分配属性规则空间），并与给定节点
    //      规则相关联。目标属性规则节点标签为<attr>，是目标节点规则节点的子节点，
    //      可以有多个，目标属性规则节点值为目标节点的属性取值。目标属性规则节点的自身
    //      属性在目标属性规则节点的标签内，必须有两个"opt"和"valCk"，它们描述了目标属
    //      性规则节点的必要性和目标属性的取值规则
    //      每次为同一目标节点规则创建属性规则时，均为所有已分配的属性规则重新分配空间，使
    //      同一目标节点规则的属性规则列表空间连续，在释放该节点规则的所有属性规则空间时，
    //      仅给出属性规则列表头指针即可
    //参数1：给定的目标节点规则指针
    //参数2：目标节点元素指针
    //参数3：目标规则节点父规则指针
    //返回值：错误码，0为成功，目标节点规则块中的属性规则块指针非空；
    //        非0为发生错误，目标节点规则块中的属性规则块指针为空
    XMLRL_PARSE_ERROR xmlStd::
    setOneAttrStd ( eleRl* eleRl_p, XMLElement* rlEle_p, attrRl* attrPos, eleRl* parentRl_p )
    {
        //输入参数检查
        if ( !eleRl_p || !rlEle_p || !attrPos )
        {
            return XMLRL_FUN_PARAM;
        }

        //eleRl* tInEleRl_p = eleRl_p;//目标节点
        XMLElement* tInRlEle_p = rlEle_p;//目标节点的目标属性元素
        attrRl* tInAttrPos = attrPos;

        //设置规定的目标属性名--<attr>的节点取值
        tInAttrPos->atrrNmRl = tInRlEle_p->GetText();

        if ( !tInAttrPos->atrrNmRl ) //目标属性无名称，非法
        {
            return XMLRL_ATTR_NONAME;
        }

        //目标属性的自身规则："opt"和"valCk"，"opt"规则必须有，valCk可选
        const XMLAttribute* tAttrSelfStd;
        const char* tAttrSelfAttrNm;
        const char* tAttrValStr;
        int i;
        int optFlag = 0;
        int tAttrCnt = 0;
        tAttrSelfStd = tInRlEle_p->FirstAttribute();

        if ( !tAttrSelfStd )
        {
            return XMLRL_NODE_ATTR;
        }

        while ( tAttrSelfStd )
        {
            tAttrCnt++;
            tAttrSelfAttrNm = tAttrSelfStd->Name();

            for ( i = 0; i < STDNODE_ATTR_NUM; i++ )
            {
                if ( !strcmp ( stdNdAttrNm[i], tAttrSelfAttrNm ) )
                {
                    break;
                }
            }

            if ( STDNODE_ATTR_NUM == i ) //不认识的节点属性自身属性名，多了！
            {
                return XMLRL_ATTR_NAME;
            }

            tAttrValStr = tAttrSelfStd->Value();//取节点属性自身的属性值

            if ( 0 == i ) //"opt"，得到目标节点的必要性
            {
                if ( !strcmp ( "essential", tAttrValStr ) )
                {
                    tInAttrPos->isNeed = 1;
                    optFlag = 1;
                }
                else if ( !strcmp ( "non-essential", tAttrValStr ) )
                {
                    tInAttrPos->isNeed = 0;
                    optFlag = 1;
                }
                else
                {
                    return XMLRL_OPT_UNDEF;    //opt属性赋值错
                }
            }

            if ( 1 == i ) //得到目标节点属性的值检查规则
            {
                tInAttrPos->atrrValRl = tAttrValStr;
            }//{valCk="non-null";sptr:[.];num:[0,255]}

            tAttrSelfStd = tAttrSelfStd->Next();
        }

        //自身属性已全部读出
        if ( tAttrCnt > STDNODE_ATTR_NUM ) //检查是否多了
        {
            return XMLCHK_ATTR_LARGE;
        }

        if ( 1 != optFlag ) //检查是否少opt属性，valck可无，但opt必须有
        {
            return XMLCHK_ATTR_LACK;
        }

        return  XMLRL_SUCCESS;
    }

    //功能：遍历给定非根目标节点元素的有属性规则节点（<attr>），通过调用setOneAttrStd()，
    //      逐个创建和设置非根目标节点规则的所有属性规则
    //参1：给定的非根目标节点规则指针
    //参2：规则文件目标节点元素指针
    //参3：给定的非根目标节点规则的父节点规则指针
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    setNdAttrsStd ( eleRl* eleRl_p, XMLElement* rlEle_p, eleRl* parentRl_p )
    {
        XMLRL_PARSE_ERROR tRet;

        //检查输入函数
        if ( !eleRl_p || !rlEle_p || !parentRl_p )
        {
            return XMLRL_FUN_PARAM;
        }

        //接收输入参数
        eleRl* tInEleRl_p = eleRl_p;//参数为目标节点
        eleRl* tInParentRl_p = parentRl_p;
        XMLElement* tInRlEle_p = rlEle_p;
        XMLElement* tRlEle_p = NULL;

        unsigned char tAttrNum = 0;
        tRlEle_p = tInRlEle_p->FirstChildElement ( "attr" ); //得到目标节点的第1个attr规则节点

        while ( tRlEle_p ) //目标节点有属性规则节点
        {
            tAttrNum++;
            tRlEle_p = tRlEle_p->NextSiblingElement ( "attr" );
        }

        if ( !tAttrNum )
        {
            return XMLRL_SUCCESS;
        }

        tInEleRl_p->attrRlNum = tAttrNum;
        //分配属性list空间
        tInEleRl_p->attrRlLst = ( attrRl* ) calloc ( tInEleRl_p->attrRlNum, sizeof ( attrRl ) );

        if ( !tInEleRl_p->attrRlLst )
        {
            return XMLRL_MEM_ALLOC;
        }

        tAttrNum = 0;
        tRlEle_p = tInRlEle_p->FirstChildElement ( "attr" ); //得到目标节点的第1个attr规则节点

        while ( tRlEle_p ) //目标节点有属性规则节点
        {
            ( tInEleRl_p->attrRlLst + tAttrNum )->ownerRl = tInEleRl_p;
            tRet = setOneAttrStd ( tInEleRl_p, tRlEle_p, tInEleRl_p->attrRlLst + tAttrNum, tInParentRl_p );

            if ( tRet )
            {
                //free(tInEleRl_p->attrRlLst);
                return tRet;
            }

            tAttrNum++;
            tRlEle_p = tRlEle_p->NextSiblingElement ( "attr" );
        }

        return XMLRL_SUCCESS;
    }


    //功能：检查并创建和设置一个非根目标节点元素标准块内的节点值规则，将它挂接到元素标
    //      准块的节点值规则指针上，然后使自己的元素标准块成为父元素标准块的新孩子，最
    //      后将该节点的属性值规则设置到节点值规则（通过调用setNdAttrsStd()实现）中。
    //      要求目标节点必须有名称，且规则文件不能为同一目标节点规定多个名称
    //      目标节点规则元素自身必须带有必要性和值检查两个属性"opt"和"valCk"，它们在目
    //      标规则节点元素标签内，描述了目标节点的必要性和目标节点的取值规则。
    //参1：输入，目标节点元素指针
    //参2：输入，目标父节点元素标准块指针
    //参3：输出，目标节点元素标准指针
    //返回值：错误码，0为成功，目标节点元素规则块中的元素规则块指针非空；
    //        非0为发生错误，目标节点元素规则块中的元素规则块指针为空
    XMLRL_PARSE_ERROR xmlStd::
    setOneNdStd ( XMLElement* rlEle_p, ele_Std* parentEleStd_p, ele_Std* tgtEleStd_p )
    {
        //检查输入参数
        if ( ( !rlEle_p ) || ( !parentEleStd_p ) || ( !tgtEleStd_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLRL_PARSE_ERROR tRet;
        XMLElement* tInRlEle_p = rlEle_p;
        ele_Std* tInParentEleStd_p = parentEleStd_p;
        ele_Std* tInTgtEleStd_p = tgtEleStd_p;
        tInTgtEleStd_p->std_p = NULL;
        eleRl* tEleRl_p = NULL;

        //为目标节点创建规则块
        tEleRl_p = ( eleRl* ) calloc ( 1, sizeof ( eleRl ) );

        if ( !tEleRl_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        //得到目标节点的规定名
        XMLElement* tRlEleNm_p = tInRlEle_p->FirstChildElement ( "name" );

        if ( tRlEleNm_p )
        {
            tEleRl_p->eleNmRl = tRlEleNm_p->GetText();//目标节点规定名

            if ( !tEleRl_p->eleNmRl )
            {
                //free(tEleRl_p);
                return XMLRL_NAME_NULL;
            }
        }
        else
        {
            //free(tEleRl_p);
            return XMLRL_FMT_NAME;
        }

        if ( tRlEleNm_p->NextSiblingElement ( "name" ) ) //不允许为目标的信息区节点规定多个名字
        {
            //free(tEleRl_p);
            return XMLRL_FMT_NAME;
        }

        //得到目标节点自身规则（必要性和值检查）
        //非标签为<attr>的节点，而是节点规则中的属性字段
        const XMLAttribute* tNdSelfStd;
        const char* tNdSelfAttrNm;
        const char* tAttrValStr;
        int i;
        tNdSelfStd = tInRlEle_p->FirstAttribute();

        if ( !tNdSelfStd ) //目标节点自身必须只有两个规则："opt"和"valCk"
        {
            return XMLRL_NODE_ATTR;
        }

        int tAttrCnr = 0;

        while ( tNdSelfStd )
        {
            tAttrCnr++;
            tNdSelfAttrNm = tNdSelfStd->Name();

            for ( i = 0; i < STDNODE_ATTR_NUM; i++ )
            {
                if ( !strcmp ( stdNdAttrNm[i], tNdSelfAttrNm ) )
                {
                    break;
                }
            }

            if ( STDNODE_ATTR_NUM == i ) //不认识的节点自身属性名
            {
                //free(tEleRl_p);
                return XMLRL_ATTR_NAME;
            }

            tAttrValStr = tNdSelfStd->Value();//取节点自身的属性值

            if ( 0 == i ) //"opt"，得到目标节点的必要性
            {
                if ( !strcmp ( "essential", tAttrValStr ) )
                {
                    tEleRl_p->isNeed = 1;
                }
                else if ( !strcmp ( "non-essential", tAttrValStr ) )
                {
                    tEleRl_p->isNeed = 0;
                }
                else
                {
                    //free(tEleRl_p);
                    return XMLRL_OPT_UNDEF;//opt属性赋值错
                }
            }

            if ( 1 == i ) //得到目标节点的值检查规则
            {
                tEleRl_p->eleValRl = tAttrValStr;    //"valCk"
            }

            tNdSelfStd = tNdSelfStd->Next();
        }

        if ( tAttrCnr > 2 )
        {
            //free(tEleRl_p);
            return XMLCHK_ATTR_LARGE;
        }

        if ( !tEleRl_p->eleValRl )
        {
            tEleRl_p->eleValRl = defaultRl[0];    //"non-null"
        }

        //设置它的父节点元素标准块，使自己的元素标准块成为它的新孩子
        if ( tInParentEleStd_p->childNum > XML_MAX_WIDTH )
        {
            return XMLCHK_CHILD_LARGE;
        }

        ele_Std** tChildAddr = tInParentEleStd_p->cEleStd_p;

        if ( !tInParentEleStd_p->childNum ) //准备第1个孩子
        {
            tChildAddr = ( ele_Std** ) calloc ( 1, sizeof ( ele_Std* ) );

            if ( !tChildAddr )
            {
                return XMLRL_MEM_ALLOC;
            }

            tInParentEleStd_p->cEleStd_p = tChildAddr;
        }
        else
        {
            tInParentEleStd_p->cEleStd_p = ( ele_Std** ) realloc ( tInParentEleStd_p->cEleStd_p, ( tInParentEleStd_p->childNum + 1 ) * sizeof ( ele_Std* ) );

            if ( !tInParentEleStd_p->cEleStd_p )
            {
                return XMLRL_MEM_ALLOC;
            }

            //地址的地址类型取值，不设置编译的情况下，会被按字长对齐！
            tChildAddr = ( ele_Std** ) ( ( char* ) ( tInParentEleStd_p->cEleStd_p ) + tInParentEleStd_p->childNum * sizeof ( ele_Std* ) );
        }

        *tChildAddr = tInTgtEleStd_p;
        tInParentEleStd_p->childNum++;

        //设置目标节点的所有属性规则，非节点自身规则，而是规则中所有<attr>标签兄弟节点
        tRet = setNdAttrsStd ( tEleRl_p, tInRlEle_p, tInParentEleStd_p->std_p );

        if ( tRet )
        {
            return tRet;
        }

        tInTgtEleStd_p->std_p = tEleRl_p;
        return XMLRL_SUCCESS;
    }


    //功能：设置信息层（目标元素的第一层）节点元素的规则（xxx_info Lvl）。它们在规则文件中
    //      以<root>的兄弟出现，需特殊处理。生成的规则按目标元素节点规则顺序（infoLvlNm[]
    //      定义的顺序）组织，而非规则文件自身的元素节点顺序
    //      检查规则：本层所有信息节点由infoLvlNm[]指定名称，规则文件中即不能重复，也不能缺少，
    //      如果是本层第1个节点，元素标准块已分配，否则创建并设置后挂接到按层组织的元素标准块
    //      链中，元素标准块内部挂接规则，该规则需要另外分配空间（通过调用setOneNdStd()实现）
    //参数：无
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    setInfoLvlNdsStd()
    {
        XMLRL_PARSE_ERROR tRet;
        XMLElement* tParentEle_p = NULL;
        ele_Std* tParentEleStd_p = NULL;
        eleRl* tParentRl_p = NULL;
        XMLElement* tChildEle_p = NULL;
        XMLElement* tTgtEle_p = NULL;
        ele_Std* tEleStd_p = NULL;
        eleRl* tTgtRl_p = NULL;
        ele_Std* tMountPos = NULL;

        //得到目标根节点元素规则和元素节点对
        tParentEleStd_p = &ele_StdLvlHdr[0];
        tParentEle_p = tParentEleStd_p->ele_p;
        tParentRl_p = tParentEleStd_p->std_p;

        //生成信息层第一个目标节点的元素节点
        //<root>sysCfg</root>的兄弟：
        //{<sysInfo_Rl opt=...>} {<hstInfo_Rl opt=...>} {<appInfo_Rl opt=...>}...
        int hitCounter[INFOFIELD_NUM];
        int i;

        for ( i = 0; i < INFOFIELD_NUM; i++ )
        {
            hitCounter[i] = 0;
        }

        //  tMountPos = &ele_StdLvlHdr[1];

        //tParentEle_p是目标文件中的根，在规则文件中它是本层的兄弟
        tTgtEle_p = tParentEle_p->NextSiblingElement();//第一个节点，如<sysInfo_Rl>

        if ( tTgtEle_p && tTgtEle_p->GetText() ) //信息节点不应有文本
        {
            return XMLRL_FILE_UNKNOWN;
        }

        if ( !tTgtEle_p )
        {
            return XMLRL_FILE_UNKNOWN;
        }

        int hdrFlag = 1;

        while ( tTgtEle_p )
        {
            for ( i = 0; i < infoFieldNum; i++ )
            {
                if ( !strcmp ( infoLvlNm[i], tTgtEle_p->Name() ) )
                {
                    hitCounter[i]++;
                    break;
                }
            }

            if ( ( infoFieldNum == i ) || ( hitCounter[i] > 1 ) ) //未找到或多次命中
            {
                return XMLRL_FILE_UNKNOWN;
            }

            //如果是本层第1个节点，元素规则空间已分配
            //      if (tMountPos == &ele_StdLvlHdr[1])
            if ( 1 == hdrFlag )
            {
                //ele_StdLvlHdr[1].cEleStd_p;//儿子填
                //ele_StdLvlHdr[1].childNum;//儿子填
                ele_StdLvlHdr[1].ele_p = tTgtEle_p;
                ele_StdLvlHdr[1].nxt = NULL;
                ele_StdLvlHdr[1].pEleStd_p = &ele_StdLvlHdr[0];
                ele_StdLvlHdr[1].std_p = NULL;//待分配
                hdrFlag = 0;
                tMountPos = &ele_StdLvlHdr[1];
                tEleStd_p = &ele_StdLvlHdr[1];
            }
            else//不是本层第1个节点，分配元素规则块后再使用
            {
                tEleStd_p = ( ele_Std* ) calloc ( 1, sizeof ( ele_Std ) );

                if ( !tEleStd_p )
                {
                    return XMLRL_MEM_ALLOC;
                }

                //tEleStd_p->cEleStd_p;//儿子填
                //tEleStd_p->childNum;//儿子填
                tEleStd_p->ele_p = tTgtEle_p;
                tEleStd_p->nxt = NULL;
                tEleStd_p->pEleStd_p = &ele_StdLvlHdr[0];
                tEleStd_p->std_p = NULL;//待分配
                tMountPos->nxt = tEleStd_p;
                tMountPos = tEleStd_p;
            }

            //分配并设置tEleStd_p->std_p
            tRet = setOneNdStd ( tTgtEle_p, tParentEleStd_p, tEleStd_p );

            if ( tRet )
            {
                return tRet;
            }

            //继续得到它的类型规则
            tTgtRl_p = tEleStd_p->std_p;
            tChildEle_p = tTgtEle_p->FirstChildElement ( "child" );

            if ( tChildEle_p )
            {
                tTgtRl_p->ndType = NONLEAF;
            }
            else
            {
                tTgtRl_p->ndType = LEAF;
            }

            tTgtEle_p = tTgtEle_p->NextSiblingElement();//<sysInfo_Rl>{<hstInfo_Rl opt=...>}

            if ( tTgtEle_p && tTgtEle_p->GetText() ) //信息节点不应有文本
            {
                return XMLRL_FILE_UNKNOWN;
            }
        }

        //规则文件中所有信息节点均已读出，检查它们是否缺少节点
        for ( i = 0; i < infoFieldNum; i++ )
        {
            if ( !hitCounter[i] ) //存在未命中的节点名，说明规则文件中无此节点
            {
                return XMLRL_FILE_UNKNOWN;
            }
        }

        return XMLRL_SUCCESS;
    }

    //功能：设置指定非根目标节点层中每个非目标根节点元素规则，按目标元素节点规则顺序组织，
    //      而非规则文件自身的元素节点顺序；以一个目标节点规则为入口，而非规则文件的节点
    //      为入口第一个目标节点规则为目标根节点规则元素，其父规则为根节点规则
    //参数：非根目标节点层号
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    setLvlNdsStd ( unsigned int stdLvl )
    {
        //检查输入参数
        if ( ( stdLvl > XML_MAX_LVLNUM ) || ( stdLvl < 2 ) )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLRL_PARSE_ERROR tRet;
        unsigned int tInStdLvl = stdLvl;

        XMLElement* tParentEle_p = NULL;
        ele_Std* tParentEleStd_p = NULL;

        XMLElement* tTgtEle_p = NULL;
        eleRl* tTgtRl_p = NULL;
        ele_Std* tEleStd_p = NULL;

        eleRl* tSiblingRl_p = NULL;
        XMLElement* tSiblingEle_p = NULL;

        XMLElement* tChildEle_p = NULL;
        ele_Std* tFstSblng = NULL;
        ele_Std* tCurSblng = NULL;
        ele_Std* tMountAddr;
        ele_Std** tMountPos;
        tMountAddr = &ele_StdLvlHdr[tInStdLvl];
        tMountPos = &tMountAddr;
        tParentEleStd_p = &ele_StdLvlHdr[tInStdLvl - 1];

        //将上一层的第一个节点作为父亲元素标准节点，得到它的第1个孩子元素标准（当前层）
        //<sysInfo_Rl opt=...>：{<child>}<child>...<child>
        if ( !tParentEleStd_p ) //不存在父层，说明输入层数过大
        {
            return XMLCHK_LVL_LARGE;
        }

        while ( LEAF == tParentEleStd_p->std_p->ndType ) //存在父层，找第1个有孩子的父亲
        {
            tParentEleStd_p = tParentEleStd_p->nxt;//下一父亲

            if ( !tParentEleStd_p ) //已无有孩子的父亲，说明父亲所在层全部是叶子节点
            {
                return XMLRL_FILE_END;
            }
        }

        //第一个有孩子的父亲
        tParentEle_p = tParentEleStd_p->ele_p;
        tTgtEle_p = tParentEle_p->FirstChildElement ( "child" );

        if ( tTgtEle_p->GetText() ) //孩子节点不应有文本
        {
            return XMLRL_FILE_UNKNOWN;
        }

        //第1个元素标准已分配空间（tMountPos），直接设置
        tEleStd_p = tMountAddr;
        tEleStd_p->ele_p = tTgtEle_p;
        tEleStd_p->pEleStd_p = tParentEleStd_p;
        //-->tEleStd_p->std_p = tTgtRl_p;
        tRet = setOneNdStd ( tTgtEle_p, tParentEleStd_p, tEleStd_p );

        if ( tRet )
        {
            return tRet;
        }

        //继续得到它的类型规则
        tTgtRl_p = tEleStd_p->std_p;
        tChildEle_p = tTgtEle_p->FirstChildElement ( "child" ); //<child>的<child>

        if ( tChildEle_p )
        {
            tTgtRl_p->ndType = NONLEAF;
        }
        else
        {
            tTgtRl_p->ndType = LEAF;
        }

        tFstSblng = tMountAddr;
        tMountPos = & ( tEleStd_p->nxt ); //下一挂接点，未分配空间

        //遍历第一个目标元素{<child>}的所有<child>兄弟（有共同的父元素）
        while ( tParentEleStd_p ) //遍历上层某父目标元素的所有孩子
        {
            //遍历某父第一个元素{<child>}的所有<child>兄弟（有共同的父元素）
            //<hstInfo_Rl opt=...>：<child>{<child>}...<child>
            tSiblingEle_p = tTgtEle_p->NextSiblingElement ( "child" );

            if ( tTgtEle_p->GetText() ) //孩子节点不应有文本
            {
                return XMLRL_FILE_UNKNOWN;
            }

            while ( tSiblingEle_p ) //<xxxInfo_Rl opt=...> <child> { <child> }...{<child>}
            {
                //为某父节点的其它孩子创建元素标准节点
                tEleStd_p = ( ele_Std* ) calloc ( 1, sizeof ( ele_Std ) );

                if ( !tEleStd_p )
                {
                    return XMLRL_MEM_ALLOC;
                }

                tEleStd_p->ele_p = tSiblingEle_p;
                tEleStd_p->pEleStd_p = tParentEleStd_p;
                //-->tEleStd_p->std_p = tTgtRl_p;
                tRet = setOneNdStd ( tSiblingEle_p, tParentEleStd_p, tEleStd_p );

                if ( tRet )
                {
                    return tRet;
                }

                //继续得到它的类型规则
                tSiblingRl_p = tEleStd_p->std_p;
                tChildEle_p = tSiblingEle_p->FirstChildElement ( "child" ); //<child>的<child>

                if ( tChildEle_p )
                {
                    tSiblingRl_p->ndType = NONLEAF;
                }
                else
                {
                    tSiblingRl_p->ndType = LEAF;
                }

                *tMountPos = tEleStd_p;
                tMountPos = &tEleStd_p->nxt;
                //继续下一兄弟
                tSiblingEle_p = tSiblingEle_p->NextSiblingElement ( "child" );

                if ( tTgtEle_p->GetText() ) //孩子节点不应有文本
                {
                    return XMLRL_FILE_UNKNOWN;
                }
            }

            //同一父亲下的所有节点均完成，不允许兄弟间重名，即不能出现多个同名规则
            const char* tNm, *tTstNm;
            ele_Std* tstPos;
            tCurSblng = tFstSblng;

            while ( tCurSblng )
            {
                tNm = tCurSblng->std_p->eleNmRl;
                tstPos = tCurSblng->nxt;

                while ( tstPos )
                {
                    tTstNm = tstPos->std_p->eleNmRl;

                    if ( !strcmp ( tTstNm, tNm ) )
                    {
                        return XMLRL_CHILD_NAME;
                    }

                    tstPos = tstPos->nxt;
                }

                tCurSblng = tCurSblng->nxt;
            }

            //换下一个有孩子的父亲：其第一个子元素标准还未分配空间
            //<xxxInfo_Rl opt=...> {<child>}<child>...<child>
            tParentEleStd_p = tParentEleStd_p->nxt;

            if ( !tParentEleStd_p ) //父层全部完成
            {
                return XMLRL_SUCCESS;
            }

            while ( LEAF == tParentEleStd_p->std_p->ndType ) //找下一个有孩子的父亲
            {
                tParentEleStd_p = tParentEleStd_p->nxt;//下一父亲

                if ( !tParentEleStd_p ) //已无有孩子的父亲
                {
                    return XMLRL_SUCCESS;
                }
            }

            //找到下一个有孩子的父亲
            tParentEle_p = tParentEleStd_p->ele_p;
            tTgtEle_p = tParentEle_p->FirstChildElement ( "child" );
            tEleStd_p = ( ele_Std* ) calloc ( 1, sizeof ( ele_Std ) );

            if ( !tEleStd_p )
            {
                return XMLRL_MEM_ALLOC;
            }

            tEleStd_p->ele_p = tTgtEle_p;
            tEleStd_p->pEleStd_p = tParentEleStd_p;//新父亲
            //-->tEleStd_p->std_p = tTgtRl_p;
            tRet = setOneNdStd ( tTgtEle_p, tParentEleStd_p, tEleStd_p );

            if ( tRet )
            {
                return tRet;
            }

            //继续得到它的类型规则
            tChildEle_p = tTgtEle_p->FirstChildElement ( "child" ); //<child>的<child>
            tTgtRl_p = tEleStd_p->std_p;

            if ( tChildEle_p ) //非叶子节点不允许有值
            {
                if ( tTgtEle_p && tTgtEle_p->GetText() ) //非叶子规则节点不能有文本
                {
                    return XMLRL_FILE_UNKNOWN;
                }

                tTgtRl_p->ndType = NONLEAF;
            }
            else
            {
                tTgtRl_p->ndType = LEAF;
            }

            *tMountPos = tEleStd_p;
            tMountPos = &tEleStd_p->nxt;
            tFstSblng = tEleStd_p->nxt;
            //去检查新父亲的第1个孩子的所有兄弟-->
        }

        return XMLRL_SUCCESS;
    }

    //功能：加载XML文件规则
    //参数：应用规则文件名
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    loadXmlStd ( const char* appStdFileNm )
    {
        XMLRL_PARSE_ERROR tRet;

        //检查输入参数
        if ( !appStdFileNm )
        {
            return XMLRL_FUN_PARAM;
        }

        const char* tInAppStdFileNm = appStdFileNm;

        XMLDocument* tStdDoc_p = new XMLDocument();

        if ( !tStdDoc_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        tRet = ( XMLRL_PARSE_ERROR ) tStdDoc_p->LoadFile ( tInAppStdFileNm );

        if ( tRet )
        {
            return tRet;
        }

        rlDoc_p = tStdDoc_p;

        //检查是否为希望的规则文件：<sysCfg_Rl>...</sysCfg_Rl>
        XMLElement* tRlEle_p = NULL;
        tRet = chkRlDoc ( tStdDoc_p, RL_ID, TGT_ID, &tRlEle_p );

        if ( tRet )
        {
            return tRet;
        }

        //规则文件：<sysCfg_Rl>{<root>...</root>}...</sysCfg_Rl>
        unsigned int tLvlNum = 0;
        ele_Std* tEleStd_p = NULL;
        tRet = setRootStd ( tRlEle_p, &tEleStd_p ); //输出规则eleStd_Out_p：{<root>...</root>}

        if ( tRet )
        {
            return tRet;
        }

        stdLvlNum = tLvlNum;
        //进入目标节点第1层规则--配置信息：xxx_Info
        tLvlNum++;
        tRet = setInfoLvlNdsStd();

        if ( tRet )
        {
            return tRet;
        }

        stdLvlNum = tLvlNum;

        //进入目标节点第2层规则--配置信息：xxx_Info的子目标节点规则
        do
        {
            stdLvlNum = ++tLvlNum;
            tRet = setLvlNdsStd ( tLvlNum );
        } while ( !tRet ); //失败或完成跳出

        stdLvlNum--;//跳出时先计入一层
        return tRet;
    }

    //功能：得到目标元素的兄弟数和兄弟名列表，将它们直接放入指定地址处，兄弟名
    //      列表中不含目标元素自己，兄弟名列表顺序为先左兄弟且右兄弟
    //参1：输入，目标元素指针
    //参2：输出，目标元素兄弟数存储地址
    //参3：输出，目标元素兄弟名列表存储地址
    //返回值：错误码，0为成功，输出参数为实际值（含0和空）；
    //        非0为发生错误，输出兄弟数为0，兄弟名列表为空
    inline XMLRL_PARSE_ERROR
    getSblngInfo ( XMLElement* ele_p, unsigned char* sblngNum,
                   const char** * sblngNmLst )
    {
        if ( ( !ele_p ) || ( !sblngNum ) || ( !sblngNmLst ) )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLElement* tInTgtEle_p = ele_p;
        unsigned char tSblngNum = 0;
        unsigned char tLSblngNum = 0;
        unsigned char tRSblngNum = 0;
        const char** tSblngNmLst = NULL;
        *sblngNum = 0;
        *sblngNmLst = NULL;
        XMLElement* tTgtSblngEle_p = NULL;

        //右兄弟个数统计
        tTgtSblngEle_p = tInTgtEle_p->NextSiblingElement();

        while ( tTgtSblngEle_p )
        {
            tRSblngNum++;
            tSblngNum++;
            tTgtSblngEle_p = tTgtSblngEle_p->NextSiblingElement();
        }

        //左兄弟个数统计
        tTgtSblngEle_p = tInTgtEle_p->PreviousSiblingElement();

        while ( tTgtSblngEle_p )
        {
            tLSblngNum++;
            tSblngNum++;
            tTgtSblngEle_p = tTgtSblngEle_p->PreviousSiblingElement();
        }

        if ( !tSblngNum )
        {
            tSblngNmLst = NULL;
        }
        else
        {
            tSblngNmLst = ( const char** ) calloc ( tSblngNum, sizeof ( char* ) );

            if ( !tSblngNmLst )
            {
                *sblngNum = tSblngNum;
                *sblngNmLst = NULL;
                return XMLRL_MEM_ALLOC;
            }
        }

        //设置左兄弟名列表
        tTgtSblngEle_p = tInTgtEle_p->PreviousSiblingElement();

        for ( int i = 0; i < tLSblngNum; i++ )
        {
            * ( tSblngNmLst + i ) = tTgtSblngEle_p->Name();
            tTgtSblngEle_p = tTgtSblngEle_p->PreviousSiblingElement();
        }

        //设置右兄弟名列表
        tTgtSblngEle_p = tInTgtEle_p->NextSiblingElement();

        for ( int i = 0; i < tRSblngNum; i++ )
        {
            * ( tSblngNmLst + i ) = tTgtSblngEle_p->Name();
            tTgtSblngEle_p = tTgtSblngEle_p->NextSiblingElement();
        }

        *sblngNum = tSblngNum;
        *sblngNmLst = tSblngNmLst;
        return XMLRL_SUCCESS;
    }

    //功能：创建并确定元素目标块的目标节点类型信息、节点名和节点原始字符串值
    //参1：输入，目标元素指针
    //参2：输入，目标层号
    //参3：输出，目标节点存储地址的指针
    //返回值：错误码，0为成功，输出参数非空；非0为发生错误，输出参数为空
    inline XMLRL_PARSE_ERROR
    getTgtNdInfo ( XMLElement* ele_p, unsigned char tgtNdLvl, tgtNd** tgtNd_p )
    {
        if ( ( !ele_p ) || ( tgtNdLvl > XML_MAX_LVLNUM ) || ( !tgtNd_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        *tgtNd_p = NULL;
        XMLElement* tInTgtEle_p = ele_p;
        unsigned char tInTgtNdLvl = tgtNdLvl;

        tgtNd* tTgtNd_p = NULL;
        tTgtNd_p = ( tgtNd* ) calloc ( 1, sizeof ( tgtNd ) );

        if ( !tTgtNd_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        tTgtNd_p->tgtNdNm = tInTgtEle_p->Name();
        tTgtNd_p->tgtNdValStr = ( char* ) tInTgtEle_p->GetText(); //非叶子为空，叶子无值也为空

        if ( ! ( tInTgtEle_p->FirstChildElement() ) )
        {
            tTgtNd_p->tgtNdType = LEAF;
        }
        else
        {
            tTgtNd_p->tgtNdType = NONLEAF;
        }

        if ( !tInTgtNdLvl )
        {
            tTgtNd_p->tgtNdType = ROOT;
        }

        *tgtNd_p = tTgtNd_p;
        return XMLRL_SUCCESS;
    }

    //功能：得到目标元素的孩子数
    //参1：输入，目标元素指针
    //参2：输出，目标元素孩子数的存储地址
    //返回值：错误码，=0（成功），输出参数为实际值（含0）

    //debug:--------ele_Tgt* dbugFlg = (ele_Tgt*)0x1122334455667788;
    inline XMLRL_PARSE_ERROR
    getChdNdNum ( XMLElement* ele_p, ele_Tgt* eleTgt_p )
    {
        if ( ( !ele_p ) || ( !eleTgt_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLElement* tInTgtEle_p = ele_p;
        ele_Tgt* tInEleTgt_p = eleTgt_p;
        unsigned char tChdNum = 0;

        XMLElement* tTgtChdEle_p = tInTgtEle_p->FirstChildElement();

        while ( tTgtChdEle_p )
        {
            tChdNum++;

            if ( tChdNum >= XML_MAX_NEIGHBOR )
            {
                return XMLCHK_CHILD_LARGE;
            }

            tTgtChdEle_p = tTgtChdEle_p->NextSiblingElement();
        }

        //创建自己的子节点列表
        tInEleTgt_p->childNum = tChdNum;

        if ( tInEleTgt_p->childNum )
        {
            tInEleTgt_p->cTgtNd_p = ( ele_Tgt** ) calloc ( tInEleTgt_p->childNum, sizeof ( ele_Tgt* ) );

            if ( !tInEleTgt_p->cTgtNd_p )
            {
                return XMLRL_MEM_ALLOC;
            }
        }
        else
        {
            tInEleTgt_p->cTgtNd_p = NULL;
        }

        return XMLRL_SUCCESS;
    }

    //功能：创建并确定目标节点的属性信息，包括属性数和属性列表，一次性为节点分配全部属性
    //      空间
    //参1：输入，目标元素指针
    //参2：输入，目标节点地址的指针
    //返回值：错误码，0为成功，输出参数的属性相关域为实际值；
    //        非0为发生错误，输出参数的属性值域为0，属性列表域为空
    inline XMLRL_PARSE_ERROR
    getTgtAttrInfo ( XMLElement* tgtEle_p, tgtNd** tgtNd_p )
    {
        if ( ( !tgtEle_p ) || ( !tgtNd_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        ( *tgtNd_p )->tgtNdAttrLst = NULL;
        ( *tgtNd_p )->tgtNdAttrNum = 0;

        XMLElement* tInTgtEle_p = tgtEle_p;
        tgtNd** tIntgtNd_p = tgtNd_p;
        tgtNd* tTgtNd_p = NULL;
        unsigned char tAttrNum = 0;
        //tgtAttr* tTgtAttrLst = NULL;

        //计算属性个数，重新为节点分配一个整块空间
        const XMLAttribute* tTgtAttr_p = tInTgtEle_p->FirstAttribute();

        while ( tTgtAttr_p )
        {
            tAttrNum++;
            tTgtAttr_p = tTgtAttr_p->Next();

            if ( tAttrNum >= XML_MAX_ATTRNUM )
            {
                return XMLCHK_ATTR_LARGE;
            }
        }

        tTgtNd_p = *tIntgtNd_p;

        if ( !tAttrNum )
        {
            tTgtNd_p->tgtNdAttrNum = 0;
            tTgtNd_p->tgtNdAttrLst = NULL;
            return XMLRL_SUCCESS;
        }

        tTgtNd_p->tgtNdAttrNum = tAttrNum;
        tTgtNd_p->tgtNdAttrLst = ( tgtAttr* ) calloc ( tAttrNum, sizeof ( tgtAttr ) );

        //设置此节点每个属性的信息
        tTgtAttr_p = tInTgtEle_p->FirstAttribute();

        for ( int i = 0; i < tAttrNum; i++ )
        {
            ( tTgtNd_p->tgtNdAttrLst + i )->ownerNd = tTgtNd_p;
            ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrNm = tTgtAttr_p->Name();
            ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrValStr = tTgtAttr_p->Value();
            tTgtAttr_p = tTgtAttr_p->Next();
        }

        return XMLRL_SUCCESS;
    }

    //功能：
    //参1：输入，父元素目标块指针
    //参2：输入，当前元素目标块指针
    //返回值：错误码，0为成功；非0为发生错误
    inline XMLRL_PARSE_ERROR
    setPNdChd ( ele_Tgt* pEleTgt_p, ele_Tgt* eleTgt_p )
    {
        if ( ( !pEleTgt_p ) || ( !eleTgt_p ) )
        {
            return XMLRL_FUN_PARAM;
        }

        ele_Tgt* tInPEleTgt_p = pEleTgt_p;
        ele_Tgt* tInEleTgt_p = eleTgt_p;

        //设置目标父亲：将自己作为孩子设置到父节点中
        ele_Tgt** xChrgAddr = tInPEleTgt_p->cTgtNd_p;

        for ( int i = 0; i < tInPEleTgt_p->childNum; i++ )
        {
            if ( ! ( *xChrgAddr ) )
            {
                *xChrgAddr = tInEleTgt_p;
                break;
            }

            xChrgAddr++;
        }

        return XMLRL_SUCCESS;
    }

    //功能：创建一个元素目标块，挂接到指定层的元素目标块节点末尾
    //参1：输入，元素目标块节点的挂接点的头位置指针
    //参2：输入，层号
    //参3：输出，保存所创建的目标块节点地址的指针
    //返回值：错误码，0为成功，输出参数非空；非0为发生错误，输出参数为空
    inline XMLRL_PARSE_ERROR
    crtEleTgt ( ele_Tgt** eleTgtLvlHdr, unsigned char lvlNum, unsigned char usingEleTgtCnt, ele_Tgt** eleTgt_p )
    {
        if ( ( lvlNum > XML_MAX_LVLNUM ) || ( usingEleTgtCnt > XML_MAX_WIDTH ) )
        {
            return XMLRL_FUN_PARAM;
        }

        ele_Tgt** tInEleTgtLvl = eleTgtLvlHdr;
        unsigned char tInUsingEleTgtCnt = usingEleTgtCnt;
        *eleTgt_p = NULL;

        //头元素目标块已经分配，没有分配目标节点，说明该元素目标块尚未使用
        if ( !tInUsingEleTgtCnt )
        {
            *eleTgt_p = *eleTgtLvlHdr;//直接使用eleTgtLvlHdr传来的地址
            return XMLRL_SUCCESS;
        }

        ele_Tgt* tEleTgt_p = ( ele_Tgt* ) calloc ( 1, sizeof ( ele_Tgt ) );

        if ( !tEleTgt_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        while ( tInUsingEleTgtCnt-- )
        {
            tInEleTgtLvl = & ( ( *tInEleTgtLvl )->nxt );
        }

        //debug
        /*  if(*tInEleTgtLvl)
            {
                if ((*tInEleTgtLvl)->nxt)
                    return XMLRL_FMT_UNKNOWN;
            }*/
        //debug

        *tInEleTgtLvl = tEleTgt_p;
        *eleTgt_p = tEleTgt_p;
        return XMLRL_SUCCESS;
    }

    //功能：查找给定非根节点元素的元素目标块的元素标准块，即得到目标节点使用的元素节点规则，
    //      查找的匹配规则是：通过父元素标准块规则找到自己适用的规则，父元素标准块的元素节点
    //      规则中给出子节点规则集，目标节点名称必须在此规则集内,父的子节点规则集中不允许有同
    //      名节点规则，规则生成过程中已保证这一点，否则意味着当前目标节点对应多个规则，即节
    //      点规则具有不确定性，这代表着规则定义出现了问题。同一节点规则允许多个目标节点使用
    //参1：输入，目标元素指针
    //参2：输入，目标元素的父元素目标块指针
    //参3：输出，存放目标元素的元素标准块地址的指针
    //返回值：错误码，0为成功，目标元素的元素标准块地址非空；非0为发生错误，目标元素的元素标准
    //        块地址为空
    inline XMLRL_PARSE_ERROR
    getTgtNdStd ( XMLElement* tgtEle_p, ele_Tgt* pEleTgt_p, ele_Std** tgtStd_p )
    {
        if ( ( !tgtEle_p ) || ( !pEleTgt_p ) || ( !tgtStd_p ) ) //不负责目标节点是根节点情况
        {
            return XMLRL_FUN_PARAM;
        }

        ele_Tgt* tInPEleTgt_p = pEleTgt_p;
        XMLElement* tInTgtEle_p = tgtEle_p;
        *tgtStd_p = NULL;
        ele_Std* tEleStd_p = NULL;

        const char* tStdNdNm;
        ele_Std* tPTgtNdStd_p = tInPEleTgt_p->tgtNdStd_p;
        int tStdNdNum = tPTgtNdStd_p->childNum;//得到父节点规定的子节点数
        const char* tTgtNdNm = tInTgtEle_p->Name();//得到节点实际名

        if ( !tStdNdNum )
        {
            return XMLCHK_CHK_NORULE;
        }

        for ( ; tStdNdNum; tStdNdNum-- ) //在父节点规定的子节点中查找是否有实际节点规则
        {
            tStdNdNm = tPTgtNdStd_p->cEleStd_p[tStdNdNum - 1]->std_p->eleNmRl;

            if ( !strcmp ( tTgtNdNm, tStdNdNm ) ) //找到，记入跟踪节点
            {
                tEleStd_p = tPTgtNdStd_p->cEleStd_p[tStdNdNum - 1];
                break;
            }
        }

        *tgtStd_p = tEleStd_p;

        if ( !tStdNdNum ) //未找到
        {
            return XMLCHK_CHK_NORULE;
        }

        return XMLRL_SUCCESS;
    }

    //功能：设置跟踪节点，根据当前元素节点，设置跟踪结构中的当前元素目标块的兄弟数和兄弟名；
    //      设置当前元素目标块的父元素目标块指针；设置当前元素目标块的子元素目标块数；创建
    //      目标节点和目标节点包含的属性，并将它挂接到当前元素目标块中，然后给出新的目标节
    //      点地址，放入元素目标块中；设置当前元素目标块的目标元素指针；设置当前元素目标块
    //      的元素标准块指针；将自己作为孩子设置到父元素目标块中，最后，在跟踪结构中设置自
    //      己的跟踪对象为当前元素目标块
    //参1：输入，当前目标元素指针
    //参2：输入，当前元素目标块父指针
    //参3：输入，当前元素目标块指针
    //参4：输入，当前目标节点层号
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    buildTrcNd ( XMLElement* tgtEle_p, ele_Tgt* pEleTgt_p, ele_Tgt* eleTgt_p, unsigned char tgtNdLvl )
    {
        if ( ( !tgtEle_p ) || ( tgtNdLvl > XML_MAX_LVLNUM ) )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLRL_PARSE_ERROR tRet;
        XMLElement* tInTgtEle_p = tgtEle_p;
        unsigned char tIntgtNdLvl = tgtNdLvl;
        ele_Tgt* tInPEleTgt_p = pEleTgt_p;//根节点的父节点为空
        ele_Tgt* tInEleTgt_p = eleTgt_p;

        tgtChkTrc.lvlNo = tIntgtNdLvl;
        //确定元素目标块的兄弟
        tRet = getSblngInfo ( tInTgtEle_p, &tgtChkTrc.brotherNum, &tgtChkTrc.brotherNmStr );

        if ( tRet )
        {
            return tRet;
        }

        //确定元素目标块的孩子数，下一层负责设置上层的孩子
        tRet = getChdNdNum ( tInTgtEle_p, tInEleTgt_p );

        if ( tRet )
        {
            return tRet;
        }

        //创建并确定元素目标块的目标节点，及目标节点包含的属性
        tRet = getTgtNdInfo ( tInTgtEle_p, tgtChkTrc.lvlNo, &tInEleTgt_p->tgtNd_p );

        if ( tRet )
        {
            return tRet;
        }

        //tInEleTgt_p->tgtNd_p地址可能成为新值，被直接替换到tInEleTgt_p->tgtNd_p中
        tRet = getTgtAttrInfo ( tInTgtEle_p, &tInEleTgt_p->tgtNd_p );

        if ( tRet )
        {
            return tRet;
        }

        tInEleTgt_p->tgtEle_p = tInTgtEle_p;

        if ( !tgtChkTrc.lvlNo ) //根节点
        {
            tInEleTgt_p->tgtNdStd_p = ele_StdLvlHdr;
        }
        else//非根节点，在父元素标准块中找自己的所有子标准块
        {
            tRet = getTgtNdStd ( tInTgtEle_p, ( ele_Tgt* ) tInPEleTgt_p, &tInEleTgt_p->tgtNdStd_p );

            if ( tRet )
            {
                return tRet;
            }
        }

        //确定元素目标块的父亲
        tInEleTgt_p->pTgtNd_p = tInPEleTgt_p;

        if ( !tIntgtNdLvl )
        {
            tgtChkTrc.trcNd_p = tInEleTgt_p;
            return XMLRL_SUCCESS;
        }

        //将自己的元素目标块作为孩子设置到父元素目标块中
        tRet = setPNdChd ( ( ele_Tgt* ) tInPEleTgt_p, tInEleTgt_p );

        if ( tRet )
        {
            return tRet;
        }

        //确定节点的应用数据，在解析处设置值：tgtChkTrc.ndAppData
        //...

        //设置跟踪点
        tgtChkTrc.trcNd_p = tInEleTgt_p;
        return XMLRL_SUCCESS;
    }

    //功能：分析跟踪节点中给出的元素目标块，按照其内包含的目标节点标准对目标节点进行检查
    //      检查内容包括：父节点元素目标块中的规则与目标节点元素目标块指向父元素目标块中
    //      的规则是否为同一个；检查目标节点元素目标块中子节点数量和其内规则是否一致；根
    //      据目标节点元素目标块的元素标准块中规则给出的目标节点属性必要性检查目标节点是
    //      否缺少必备属性，以及目标节点是否有在属性规则中未定义的属性；根据目标节点值规
    //      则检查目标节点值是否可空；本方法只检查当前目标节点规则，因此不含需要执行元素
    //      目标块集合操作的必备性、唯一性等检查
    //参数：无
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    alsTrcNd()
    {
        ele_Tgt* tEleTgt_p = tgtChkTrc.trcNd_p;
        ele_Std* tEleStd_p = tEleTgt_p->tgtNdStd_p;
        eleRl* tStdNd_p = tEleStd_p->std_p;
        tgtNd* tTgtNd_p = tEleTgt_p->tgtNd_p;
        unsigned char tTgtNdLvl = tgtChkTrc.lvlNo;

        //非叶子结点不能存在简单值
        if ( LEAF != tTgtNd_p->tgtNdType ) //NONLEAF，ROOT
        {
            if ( tTgtNd_p->tgtNdValStr )
            {
                return XMLRL_FILE_UNKNOWN;
            }
        }

        //检查节点名称是否满足规则要求
        if ( strcmp ( tEleTgt_p->tgtNd_p->tgtNdNm, tEleTgt_p->tgtNdStd_p->std_p->eleNmRl ) )
        {
            return XMLCHK_NODE_NAME;
        }

        //检查父节点规则同一性
        if ( tTgtNdLvl ) //非根节点才有
        {
            if ( tEleTgt_p->pTgtNd_p->tgtNdStd_p->std_p != tEleStd_p->pEleStd_p->std_p )
            {
                return XMLCHK_NODE_PARENT;
            }
        }

        //检查子节点数量和节点类型规则
        if ( ( !tEleTgt_p->childNum ) && ( LEAF != tStdNd_p->ndType ) ) //规则为叶子，但目标有子节点
        {
            return XMLCHK_NODE_NONLEAF;
        }

        //检查节点属性必要性规则
        int i, j;
        int m = 0;
        const char* tTgtAttrNmStr;
        const char* tStdAttrNmStr;
        unsigned char tTgtAttrNum;
        attrRl* tStdAttr_p;

        //得到节点规则中的必备属性，必须是目标节点所有属性中的一个
        if ( tTgtNd_p->tgtNdAttrNum > tStdNd_p->attrRlNum )
        {
            return XMLCHK_ATTR_LARGE;
        }

        tTgtAttrNum = tTgtNd_p->tgtNdAttrNum;
        int* tStdNonNeedAttrInx = ( int* ) calloc ( tStdNd_p->attrRlNum, sizeof ( int ) );

        if ( !tStdNonNeedAttrInx )
        {
            return XMLRL_MEM_ALLOC;
        }

        int* tTgtNonNeedAttrInx = ( int* ) calloc ( tTgtAttrNum, sizeof ( int ) );

        if ( !tTgtNonNeedAttrInx )
        {
            free ( tStdNonNeedAttrInx );
            return XMLRL_MEM_ALLOC;
        }

        for ( i = 0; i < tTgtAttrNum; i++ ) //初始化临时信息记录
        {
            tTgtNonNeedAttrInx[i] = i;
        }

        for ( i = 0; i < tStdNd_p->attrRlNum; i++ )
        {
            tStdAttr_p = tStdNd_p->attrRlLst + i;

            if ( 1 == tStdAttr_p->isNeed )
            {
                tStdAttrNmStr = tStdAttr_p->atrrNmRl;

                if ( !tTgtAttrNum ) //目标无属性节点，但规则中要求有必备属性
                {
                    free ( tStdNonNeedAttrInx );
                    free ( tTgtNonNeedAttrInx );
                    return XMLCHK_ATTR_LACK;
                }
            }
            else//得到非必备属性索引
            {
                tStdNonNeedAttrInx[m++] = i;
                continue;
            }

            //逐一检查目标节点的实际属性是否为属性规则集中的一个
            for ( j = 0; j < tTgtAttrNum; j++ ) //所有实际属性中是否与此必备属性规则匹配
            {
                tTgtAttrNmStr = ( tTgtNd_p->tgtNdAttrLst + j )->tgtAtrrNm;

                if ( strcmp ( tStdAttrNmStr, tTgtAttrNmStr ) )
                {
                    continue;
                }
                else//找到
                {
                    tTgtNonNeedAttrInx[j] = -1;//去除目标必备属性
                    break;
                }
            }

            //在目标属性中未找到需要的必备属性
            if ( tTgtAttrNum == j )
            {
                free ( tStdNonNeedAttrInx );
                free ( tTgtNonNeedAttrInx );
                return XMLCHK_ATTR_LACK;
            }

            //for下一个规则中的必备属性
        }

        //得到节点规则中的非必备属性，目标节点所有属性必须在这些属性中
        for ( i = 0; i < tTgtAttrNum; i++ )
        {
            if ( -1 == tTgtNonNeedAttrInx[i] )
            {
                continue;
            }

            //非必备目标属性，看他们是否都存在于规则的非必备属性中
            tTgtAttrNmStr = ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrNm;

            for ( j = 0; j < m; j++ )
            {
                tStdAttr_p = tStdNd_p->attrRlLst + tStdNonNeedAttrInx[j];

                if ( !strcmp ( tStdAttr_p->atrrNmRl, tTgtAttrNmStr ) )
                {
                    break;
                }
            }

            if ( m == j ) //未找到
            {
                free ( tStdNonNeedAttrInx );
                free ( tTgtNonNeedAttrInx );
                return XMLCHK_ATTR_UNKNOWN;
            }
        }

        //检查节点的值非空规则，只适用于叶子节点，非叶子节点值必空，但有子
        if ( tStdNd_p->eleValRl ) //规则要求非空
        {
            if ( LEAF == tTgtNd_p->tgtNdType )
            {
                if ( !tTgtNd_p->tgtNdValStr )
                {
                    free ( tStdNonNeedAttrInx );
                    free ( tTgtNonNeedAttrInx );
                    return XMLCHK_NODE_NONVALUE;
                }
            }
        }
        else//规则要求空，只适用于叶子节点，目前规则中不存在此情况，相当于要求只有标签无文本
        {
            if ( tTgtNd_p->tgtNdValStr )
            {
                free ( tStdNonNeedAttrInx );
                free ( tTgtNonNeedAttrInx );
                return XMLCHK_NODE_VALUE;
            }
        }

        free ( tStdNonNeedAttrInx );
        free ( tTgtNonNeedAttrInx );
        return XMLRL_SUCCESS;
    }

    //功能：将一个节点的节点或属性的值检查规则整句分解成子句，并进行概要解析
    //参1：输入，为标准格式
    //参2：输出，子句描述指针，包含分解出的标准格式子句列链表
    //返回值：错误码，0为成功，输出参数为可空的实际子句列表；非0为发生错误，输出参数为空
    XMLRL_PARSE_ERROR
    subSntnsParse ( const char* sentence, subSntnsDsrb** subSntnsLst_out )
    {
        if ( !sentence )
        {
            return XMLRL_SUCCESS;    //无子句时，子句描述为空
        }

        //得到分解的子名
        const char* tInSentence = sentence;
        subSntnsDsrb* tOutsubSntnsDsrb = NULL;
        *subSntnsLst_out = tOutsubSntnsDsrb;

        char* tFmtSntns_p = NULL;
        unsigned long long tSntnsLng = strlen ( tInSentence );
        //unsigned int tSubSntnsNum = 0;
        int tSubSntnsNum = 0;
        //unsigned int i, j;
        int i, j;
        char* temp_p;

        if ( tSntnsLng ) //非空句子
        {
            temp_p = ( char* ) calloc ( 1, tSntnsLng + 1 );

            if ( !temp_p )
            {
                return XMLRL_MEM_ALLOC;
            }

            strcpy ( temp_p, tInSentence );
            //去连续重复的子句分隔符（重复的用空格替代）
            char* tPos = temp_p;

            while ( tPos )
            {
                tPos = strstr ( tPos, ";;" );

                if ( tPos )
                {
                    *tPos = ' ';
                    tPos++;
                }
            }

            //temp_p中不会有连续的“;”，但开头和结束可能有一个“;”，必须去除（用空格代替）
            if ( subSntnsSptr == temp_p[0] )
            {
                temp_p[0] = ' ';
            }

            if ( subSntnsSptr == temp_p[tSntnsLng - 1] )
            {
                temp_p[tSntnsLng - 1] = ' ';
            }

            //去空格和tab，将“;+去空格和tab后的句子”放至tFmtSntns_p空间
            tFmtSntns_p = ( char* ) calloc ( 1, tSntnsLng + 2 ); //含'\0'并在开关预留一个字节放子句分隔符

            if ( !tFmtSntns_p )
            {
                return XMLRL_MEM_ALLOC;
            }

            tFmtSntns_p[0] = subSntnsSptr;

            for ( i = 0, j = 1; i <= ( int ) tSntnsLng; i++ ) //将'\0'一同复制
            {
                if ( ( ' ' != temp_p[i] ) && ( '\t' != temp_p[i] ) )
                {
                    tFmtSntns_p[j++] = temp_p[i];
                }
            }

            free ( temp_p );
            tSntnsLng = j - 1;//不计'\0'

            if ( !tSntnsLng ) //全是空格，无有效字符
            {
                free ( tFmtSntns_p );
                return XMLRL_SUCCESS;
            }
        }
        else
        {
            return XMLRL_SUCCESS;    //无子句时，子句描述为空
        }

        //为子句分配空间，然后获取子句
        //一定是非空的且至少在开头有一个“;”分隔符的句子，句子尾部无“;”
        //检查子句数--查不在[]内的“;”数。
        temp_p = tFmtSntns_p;
        int tXprnFlag = 0;

        while ( *temp_p )
        {
            //去除表达式的干扰
            if ( delimiter[0] == *temp_p )
            {
                tXprnFlag++;
            }

            if ( delimiter[1] == *temp_p )
            {
                tXprnFlag--;
            }

            if ( ( !tXprnFlag ) && ( subSntnsSptr == *temp_p ) )
            {
                tSubSntnsNum++;
            }

            temp_p++;
        }

        struct fmtRcd
        {
            char* subStartPos;//子句起始位置
            char* subEndPos;  //子句结束位置
            char* xprnStartPos; //表达式开始位置
            char* xprnEndPos;   //表达式结束位置
        } *tSntnsStruct_p;

        tSntnsStruct_p = ( fmtRcd* ) calloc ( tSubSntnsNum, sizeof ( fmtRcd ) );

        if ( !tSntnsStruct_p )
        {
            if ( tFmtSntns_p )
            {
                free ( tFmtSntns_p );
            }

            return XMLRL_MEM_ALLOC;
        }

        fmtRcd* tSntnsIdxHdr_p = tSntnsStruct_p;

        //规格化后的句子：tFmtSntns_p，有效字符数tSntnsLng，子句数为tSubSntnsNum
        //将每个子句的起始和结束位置记录到tSntnsStruct_p中，第一个子句以';'开始，以'\0'结束，
        //后面子句的开始均为'\0'的后一个位置，结束在'\0'位置
        temp_p = tFmtSntns_p;
        tXprnFlag = 0;

        while ( *temp_p )
        {
            //去除表达式的干扰
            if ( delimiter[0] == *temp_p )
            {
                tXprnFlag++;
            }

            if ( delimiter[1] == *temp_p )
            {
                tXprnFlag--;
            }

            if ( ( !tXprnFlag ) && ( subSntnsSptr == *temp_p ) )
            {
                if ( tFmtSntns_p == temp_p ) //整句开始，当前子句为第一个子句
                {
                    tSntnsStruct_p->subStartPos = temp_p + 1;    //跳开始的';'
                }
                else
                {
                    tSntnsStruct_p->subEndPos = temp_p;//上一子句的结束
                    tSntnsStruct_p++;
                    tSntnsStruct_p->subStartPos = temp_p + 1;//当前子句的开始，跳';'
                    *temp_p = '\0';
                }
            }

            temp_p++;
        }

        tSntnsStruct_p->subEndPos = temp_p;

        //将表达式的起始和结束位置记录到tSntnsStruct_p中，表达式以'['的下一位置开始，以']'结束
        tXprnFlag = 0;
        temp_p = tFmtSntns_p;
        tSntnsStruct_p = tSntnsIdxHdr_p;

        for ( i = 0; i < tSubSntnsNum; i++ )
        {
            while ( *temp_p ) //第一个字符就是';'，这一点很重要，标志了第一个子句的开始
            {
                //去除表达式的干扰，并记录表达式的位置，无表达式时起始和结束位置均为NULL
                //表达式的开始为：[{x}...]，结束为：[...y{]}
                if ( delimiter[0] == *temp_p )
                {
                    tXprnFlag++;

                    if ( tXprnFlag > 1 ) //不支持'['的嵌套
                    {
                        free ( tFmtSntns_p );
                        free ( tSntnsIdxHdr_p );
                        tSntnsIdxHdr_p = NULL;
                        return XMLRL_FMT_UNKNOWN;
                    }

                    if ( 1 == tXprnFlag )
                    {
                        tSntnsStruct_p->xprnStartPos = temp_p + 1;
                    }

                    temp_p++;
                    continue;
                }

                if ( delimiter[1] == *temp_p )
                {
                    tXprnFlag--;

                    if ( tXprnFlag < 0 ) //出现先']'后'['的情况
                    {
                        free ( tFmtSntns_p );
                        free ( tSntnsIdxHdr_p );
                        tSntnsIdxHdr_p = NULL;
                        return XMLRL_FMT_UNKNOWN;
                    }

                    if ( !tXprnFlag )
                    {
                        if ( * ( temp_p + 1 ) ) //若有]必须是子句的最后一个有效字符，下一个应'\0'
                        {
                            free ( tFmtSntns_p );
                            free ( tSntnsIdxHdr_p );
                            tSntnsIdxHdr_p = NULL;
                            return XMLRL_FMT_UNKNOWN;
                        }

                        if ( temp_p > tSntnsStruct_p->xprnStartPos )
                        {
                            tSntnsStruct_p->xprnEndPos = temp_p;
                        }
                        else
                        {
                            tSntnsStruct_p->xprnStartPos = NULL;
                            tSntnsStruct_p->xprnEndPos = NULL;
                        }
                    }

                    temp_p++;
                    continue;
                }

                temp_p++;
            }

            temp_p++;//越过'/0'进入下一子句
            tSntnsStruct_p++;
        }

        if ( tXprnFlag ) //小于 0或大于 0
        {
            free ( tFmtSntns_p );
            free ( tSntnsIdxHdr_p );
            tSntnsIdxHdr_p = NULL;
            return XMLRL_FMT_UNKNOWN;
        }

        /////////////
        //规格化后的句子：tSntnsHdr_p，子句数为tSubSntnsNum
        //每个子句的结束位置为一个‘\0’，开始的';'不再使用，形成规则化后的子句
        //解析完成，准备返回空间：存在子句就一定有此结构
        tOutsubSntnsDsrb = ( subSntnsDsrb* ) calloc ( tSubSntnsNum, sizeof ( subSntnsDsrb ) );

        if ( !tOutsubSntnsDsrb )
        {
            free ( tFmtSntns_p );
            free ( tSntnsIdxHdr_p );
            tSntnsIdxHdr_p = NULL;
            return XMLRL_MEM_ALLOC;
        }

        //将tOutsubSntnsDsrb组织成链表
        subSntnsDsrb* preBlk = tOutsubSntnsDsrb;
        subSntnsDsrb* curBlk = tOutsubSntnsDsrb;

        for ( i = 0; i < tSubSntnsNum - 1; i++ )
        {
            preBlk = curBlk;
            curBlk++;
            preBlk->nxt = curBlk;
        }

        //逐个子句判断是否为分隔符子句，它的形式必须为：;{sptr:[x]};，x为表达式，x内不应再有
        //分隔符。若存在分隔符子句，存在分隔符子句说明取值为复合值，将该子句放到输出的最前面
        //同时检查除关键字和[]外不应有其它可见字符
        char* tPos;
        //目前只支持句子中有一个sptr:分句的情况
        unsigned int tSptrSubSntnsNum = 0;
        int tSptrSubSntnsID = -1;
        subSntnsDsrb* tSubSntns_p = tOutsubSntnsDsrb;

        for ( i = 0; i < tSubSntnsNum; i++ )
        {
            tPos = tSntnsIdxHdr_p[i].subStartPos;//;{s}ptr:[x];
            temp_p = strstr ( tPos, indicator[SPTR] );

            //检查指示符出现的位置是否正确
            //保证指示符出现在最前面，若有表达式其后必须是表达式的开始[A
            if ( ( temp_p == tPos ) && ( ( * ( temp_p + strlen ( indicator[SPTR] ) ) == delimiter[0] ) ) )

            {
                tOutsubSntnsDsrb->type = SPTR;
                tOutsubSntnsDsrb->sntns = tSntnsIdxHdr_p[i].subStartPos;
                tOutsubSntnsDsrb->xprnStart = tSntnsIdxHdr_p[i].xprnStartPos;
                tOutsubSntnsDsrb->xprnEnd = tSntnsIdxHdr_p[i].xprnEndPos;
                tOutsubSntnsDsrb->owner = tFmtSntns_p;
                tSubSntns_p = tOutsubSntnsDsrb->nxt;
                tSptrSubSntnsID = i;
                tSptrSubSntnsNum++;
            }

        }

        if ( tSptrSubSntnsNum > 1 ) //防分隔符子句重复
        {
            //free(tFmtSntns_p);
            free ( tSntnsIdxHdr_p );
            tSntnsIdxHdr_p = NULL;
            free ( tOutsubSntnsDsrb );
            tOutsubSntnsDsrb = NULL;
            *subSntnsLst_out = NULL;
            return XMLRL_FMT_UNKNOWN;
        }

        //////其它子句：每类子句只能出现1次，sprt在tOutsubSntnsDsrb中排在前面
        //若有分隔符，tSubSntns_p已指向下一子句描述块，否则仍指向子句描述块头
        //non-null子句若存在，必须放在分隔符子句后，其它子句前

        int tNonNullSubSntnsID = -1;
        unsigned int tNonNullSubSntnsNum = 0;

        for ( i = 0; i < tSubSntnsNum; i++ )
        {
            tPos = tSntnsIdxHdr_p[i].subStartPos;//;{n}on-null;
            temp_p = strstr ( tPos, indicator[NON_NULL] );

            //检查指示符出现的位置是否正确
            //保证指示符出现在最前面，无表达式，子句以‘\0’结束
            if ( ( temp_p == tPos ) && ( * ( temp_p + strlen ( indicator[NON_NULL] ) ) == '\0' ) )
            {
                tSubSntns_p->type = NON_NULL;
                tSubSntns_p->sntns = tSntnsIdxHdr_p[i].subStartPos;
                tSubSntns_p->xprnStart = tSntnsIdxHdr_p[i].xprnStartPos;
                tSubSntns_p->xprnEnd = tSntnsIdxHdr_p[i].xprnEndPos;
                tSubSntns_p->owner = tFmtSntns_p;
                tSubSntns_p = tSubSntns_p->nxt;
                tNonNullSubSntnsNum++;
                tNonNullSubSntnsID = i;
            }
        }

        if ( tNonNullSubSntnsNum > 1 ) //防非空子句重复
        {
            //free(tFmtSntns_p);
            free ( tSntnsIdxHdr_p );
            tSntnsIdxHdr_p = NULL;
            free ( tOutsubSntnsDsrb );
            tOutsubSntnsDsrb = NULL;
            *subSntnsLst_out = NULL;
            return XMLRL_FMT_UNKNOWN;
        }

        //若有分隔符，tSubSntns_p已指向下一子句描述块，否则仍指向子句描述块头
        //或分隔符子句的下一个位置
        //找其它子句： "sptr:", "non-null",{"banDup","num:","str:"},"";
        int tDupSubSntnsFlag[INDICATOR_NUM];//同类子句防重识别器

        for ( i = 0; i < INDICATOR_NUM; i++ )
        {
            tDupSubSntnsFlag[i] = 0;
        }

        for ( i = 0; i < tSubSntnsNum; i++ )
        {
            if ( ( tSptrSubSntnsID == i ) || ( tNonNullSubSntnsID == i ) ) //跳过分隔符子句和非空子句
            {
                continue;
            }

            tPos = tSntnsIdxHdr_p[i].subStartPos;

            //int j;
            for ( j = BANDUP; j < INDICATOR_NUM; j++ )
            {
                if ( !indicator[j] ) //不允许出现空关键字
                {
                    //free(tFmtSntns_p);
                    free ( tSntnsIdxHdr_p );
                    tSntnsIdxHdr_p = NULL;
                    free ( tOutsubSntnsDsrb );
                    tOutsubSntnsDsrb = NULL;
                    *subSntnsLst_out = NULL;
                    return XMLRL_FMT_UNKNOWN;
                }

                temp_p = strstr ( tPos, indicator[j] );

                //banDup后为'\0'，num:和str:后为'['
                if ( ( temp_p == tPos ) && ( ( * ( temp_p + strlen ( indicator[j] ) ) == delimiter[0] ) ||
                                             ( * ( temp_p + strlen ( indicator[j] ) ) == '\0' ) ) )

                {
                    tDupSubSntnsFlag[j]++;

                    if ( tDupSubSntnsFlag[j] > 1 ) //防重
                    {
                        //free(tFmtSntns_p);
                        free ( tSntnsIdxHdr_p );
                        tSntnsIdxHdr_p = NULL;
                        free ( tOutsubSntnsDsrb );
                        tOutsubSntnsDsrb = NULL;
                        *subSntnsLst_out = NULL;
                        return XMLRL_FMT_UNKNOWN;
                    }

                    tSubSntns_p->type = ( subSntnsType ) j;
                    tSubSntns_p->sntns = tSntnsIdxHdr_p[i].subStartPos;
                    tSubSntns_p->xprnStart = tSntnsIdxHdr_p[i].xprnStartPos;
                    tSubSntns_p->xprnEnd = tSntnsIdxHdr_p[i].xprnEndPos;
                    tSubSntns_p->owner = tFmtSntns_p;
                    tSubSntns_p = tSubSntns_p->nxt;
                    break;
                }
                else
                {
                    continue;    //检查标准中的下一指示符是否与第i个子句一致
                }
            }

            if ( INDICATOR_NUM == j ) //第i个子句的指示符不在标准指示符集中
            {
                //free(tFmtSntns_p);
                free ( tSntnsIdxHdr_p );
                tSntnsIdxHdr_p = NULL;
                free ( tOutsubSntnsDsrb );
                tOutsubSntnsDsrb = NULL;
                *subSntnsLst_out = NULL;
                return XMLRL_FMT_UNKNOWN;
            }
        }

        //所有子句的指示符检查完毕，释放解析时使用的内部缓存
        if ( tSntnsIdxHdr_p )
        {
            free ( tSntnsIdxHdr_p );
        }

        *subSntnsLst_out = tOutsubSntnsDsrb;//其owner域保存tFmtSntns_p空间首地址
        return XMLRL_SUCCESS;
    }

    //功能：句子中所有子句的表达式解析：最外层已无"[]"
    //      （1）单参表达式，型如："A"，A的内部可能有"[]"，但一定是成对的
    //      （2）双参表达式，型如："A1,A2"，A1和A2的内部不会有"[]"
    //      （3）变参表达式，型如："A1|A2|...|An"，Ai的内部不有"[]"
    //      以上"[]"情况由分句解析保证，即整个句子不支持"[]"的嵌套情况，否则分句解析不会通过
    //      解析后形成针对一个节点或一个属性的值检查方法链表（可无检查方法，也可有多检查方法）
    //参1：输入，子句描述指针
    //参2：输出，值检查方法链表头指针
    //返回值：0为成功，输出参数内容为可空的实际值检查方法链表头指针；
    //        非0为发生错误，输出参数为空
    XMLRL_PARSE_ERROR
    expressParse ( subSntnsDsrb* subSntns, valChkMethod** valChk_out_p )
    {
        if ( !subSntns )
        {
            return XMLRL_SUCCESS;
        }

        subSntnsDsrb*  tInSubSntns_p = subSntns;
        valChkMethod* tOutValChkHdr_p = NULL;// = valChk_out_p;
        valChkMethod* tValChk_p = NULL;
        subSntnsDsrb* tSubSntns_p = tInSubSntns_p;
        *valChk_out_p = NULL;

        int tSubSntnsNum = 0;

        while ( tSubSntns_p )
        {
            tSubSntnsNum++;
            tSubSntns_p = tSubSntns_p->nxt;
        }

        tOutValChkHdr_p = ( valChkMethod* ) calloc ( tSubSntnsNum, sizeof ( valChkMethod ) );

        if ( !tOutValChkHdr_p )
        {
            if ( tInSubSntns_p )
            {
                free ( tInSubSntns_p );
                free ( tInSubSntns_p->owner );
                return XMLRL_MEM_ALLOC;
            }
        }

        for ( int i = 0; i < tSubSntnsNum - 1; i++ )
        {
            tOutValChkHdr_p[i].nxt = &tOutValChkHdr_p[i + 1];
        }

        //开始解析表达式，无表达式时，子句描述中表达式起始和结束位置均为空
        tValChk_p = tOutValChkHdr_p;
        tSubSntns_p = tInSubSntns_p;
        subSntnsType tType;

        while ( tSubSntns_p )
        {
            tType = tSubSntns_p->type;

            switch ( tType )
            {
                case NON_NULL://目标节点或属性必须有值，对于非叶子节点，有值指的是必须有孩子，型如A
                case BANDUP://目标叶子节点的取值不能重复，型如A
                {
                    //无分隔符（在输出值检查块头）或当前子句存在表达式
                    if ( ( tOutValChkHdr_p->chkMethod != SPTR ) && ( tSubSntns_p->xprnStart ) )
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    tValChk_p->chkMethod = tType;

                    //拷贝分隔符
                    for ( int i = 0; i < SPTR_MAX_LNG; i++ )
                    {
                        tValChk_p->valStdSprt[0] = tOutValChkHdr_p->valStdSprt[i];
                    }

                    tValChk_p->ChkParam.numChk.maxVal = -1;
                    tValChk_p->ChkParam.numChk.minVal = -1;
                    tValChk_p = tValChk_p->nxt;
                    tSubSntns_p = tSubSntns_p->nxt;
                    break;
                }

                case SPTR://单参，参数为取值内部使用的分隔符，即一个属性或叶子节点的取值由分隔符分为多段
                {
                    if ( !tSubSntns_p->xprnStart ) //必须存在表达式
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    tValChk_p->chkMethod = tType;
                    unsigned char tSprtLng = ( unsigned char ) ( tSubSntns_p->xprnEnd - tSubSntns_p->xprnStart );

                    if ( tSprtLng > SPTR_MAX_LNG ) //分隔符超长
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    int i;

                    for ( i = 0; i < tSprtLng; i++ )
                    {
                        tValChk_p->valStdSprt[i] = * ( tSubSntns_p->xprnStart + i );
                    }

                    tValChk_p->valStdSprt[i] = '\0';
                    tValChk_p->ChkParam.numChk.maxVal = -1;
                    tValChk_p->ChkParam.numChk.minVal = -1;
                    tValChk_p = tValChk_p->nxt;
                    tSubSntns_p = tSubSntns_p->nxt;
                    break;
                }

                case NUM://双参，指明取值为一个非0正整数值，左参为它的取值下界，右参为它的取值上界
                {
                    if ( !tSubSntns_p->xprnStart ) //必须存在表达式
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    int i = 0;
                    int tHitNum = 0;
                    int tHitPos = 0;
                    char tChar = * ( tSubSntns_p->xprnStart );

                    while ( tChar != delimiter[1] ) //表达式的结束处为']'字符
                    {
                        if ( oprtr[1] == tChar )
                        {
                            tHitNum++;
                            tHitPos = i;
                        }

                        i++;
                        tChar = * ( tSubSntns_p->xprnStart + i );
                    }

                    if ( 1 != tHitNum )
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    //取标准规定的最小值
                    if ( !tHitPos ) //无最小值：型如",A2"或","
                    {
                        tValChk_p->ChkParam.numChk.minVal = 0x0;
                    }
                    else
                    {
                        * ( tSubSntns_p->xprnStart + tHitPos ) = '\0';

                        //tValChk_p->ChkParam.numChk.minVal = atoi(tSubSntns_p->xprnStart);
                        if ( numStrTonum ( tSubSntns_p->xprnStart, & ( tValChk_p->ChkParam.numChk.minVal ), 10 ) )
                        {
                            if ( tInSubSntns_p )
                            {
                                free ( tInSubSntns_p->owner );
                                free ( tInSubSntns_p );
                            }

                            free ( tOutValChkHdr_p );
                            return XMLRL_FMT_UNKNOWN;
                        }

                        * ( tSubSntns_p->xprnStart + tHitPos ) = oprtr[1]; //换回
                    }

                    //取标准规定的最大值
                    if ( delimiter[1] == * ( tSubSntns_p->xprnStart + tHitPos + 1 ) ) //无最大值：型如"A1,"或","
                    {
                        tValChk_p->ChkParam.numChk.maxVal = 0xFFFFFFFF;
                    }
                    else
                    {
                        * ( tSubSntns_p->xprnEnd ) = '\0'; //']'换成'\0'

                        //tValChk_p->ChkParam.numChk.maxVal = atoi((const char*)(tSubSntns_p->xprnStart + tHitPos + 1));
                        if ( numStrTonum ( tSubSntns_p->xprnStart + tHitPos + 1, & ( tValChk_p->ChkParam.numChk.maxVal ), 10 ) )
                        {
                            if ( tInSubSntns_p )
                            {
                                free ( tInSubSntns_p->owner );
                                free ( tInSubSntns_p );
                            }

                            free ( tOutValChkHdr_p );
                            return XMLRL_FMT_UNKNOWN;
                        }

                        * ( tSubSntns_p->xprnEnd ) = delimiter[1]; //换回
                    }

                    tValChk_p->chkMethod = tType;
                    tValChk_p->valStdSprt[0] = '\0';
                    tValChk_p = tValChk_p->nxt;
                    tSubSntns_p = tSubSntns_p->nxt;
                    break;
                }

                case STR://变参，指明取值为一个字符串，字符串只能为变参中给出的所有参数之一
                {
                    //必须存在表达式，且由两个或两个以上参数组成，至少有一个'|'
                    if ( !tSubSntns_p->xprnStart ) //必须存在表达式
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    //不允许出现连续'|'的情况
                    char* tempPos_p = tSubSntns_p->xprnStart;
                    char* tempPrePos_p = tSubSntns_p->xprnEnd;

                    while ( tempPos_p < tSubSntns_p->xprnEnd ) //*(tSubSntns_p->xprnEnd)=']'
                    {
                        if ( *tempPos_p == oprtr[0] )
                        {
                            if ( tempPrePos_p + 1 == tempPos_p )
                            {
                                if ( tInSubSntns_p )
                                {
                                    free ( tInSubSntns_p->owner );
                                    free ( tInSubSntns_p );
                                }

                                free ( tOutValChkHdr_p );
                                return XMLRL_FMT_UNKNOWN;
                            }

                            tempPrePos_p = tempPos_p;
                        }

                        tempPos_p++;
                    }

                    int i = 0;
                    int tHitNum = 0;
                    int tHitPos = 0;
                    char tChar = * ( tSubSntns_p->xprnStart );

                    while ( tChar )
                    {
                        if ( oprtr[0] == tChar )
                        {
                            //表达式不允许以'|'开头，也不允许以'|'结尾
                            if ( ( !i ) || ( ( tSubSntns_p->xprnStart + i + 1 ) == tSubSntns_p->xprnEnd ) )
                            {
                                if ( tInSubSntns_p )
                                {
                                    free ( tInSubSntns_p->owner );
                                    free ( tInSubSntns_p );
                                }

                                free ( tOutValChkHdr_p );
                                return XMLRL_FMT_UNKNOWN;
                            }

                            if ( !tHitNum )
                            {
                                //仅将命中第1个选项后的'|'替换为字符串结束符
                                //以后仅作开始片断的检查，不进行选项全文检查
                                * ( tSubSntns_p->xprnStart + i ) = '\0';
                                tHitPos = i;
                            }

                            tHitNum++;
                        }

                        i++;
                        tChar = * ( tSubSntns_p->xprnStart + i );
                    }

                    if ( !tHitNum ) //表达式中的字符串只能是选项，不允许无'|'
                    {
                        if ( tInSubSntns_p )
                        {
                            free ( tInSubSntns_p->owner );
                            free ( tInSubSntns_p );
                        }

                        free ( tOutValChkHdr_p );
                        return XMLRL_FMT_UNKNOWN;
                    }

                    tHitNum++;//真正选项数比'|'个数多1
                    tValChk_p->chkMethod = tType;
                    tValChk_p->valStdSprt[0] = '\0';

                    //查找关键字
                    const char* tKey;
                    char* tTst;
                    tTst = tSubSntns_p->xprnStart;//仅第1个选项以字符串形式出现

                    switch ( tHitNum ) //根据选项个数先确定可能是哪个选项列表，进而减小查找范围
                    {
                        case 2://只要标准中第1个选项在选项列表中即认为规则文件正确，不再逐一检查
                        {
                            for ( i = 0; i < tHitNum; i++ )
                            {
                                tKey = opt1_2Cnst[i];

                                if ( !strcmp ( tTst, tKey ) ) //匹配
                                {
                                    tValChk_p->ChkParam.strOptGrp = optGrp1;

                                    break;
                                }
                            }

                            if ( tHitNum == i ) //无匹配项
                            {
                                if ( tInSubSntns_p )
                                {
                                    free ( tInSubSntns_p->owner );
                                    free ( tInSubSntns_p );
                                }

                                free ( tOutValChkHdr_p );
                                return XMLRL_FMT_UNKNOWN;
                            }

                            break;
                        }

                        case 3:
                        {
                            for ( i = 0; i < tHitNum; i++ )
                            {
                                tKey = opt5_3Cnst[i];

                                if ( !strcmp ( tTst, tKey ) ) //匹配
                                {
                                    tValChk_p->ChkParam.strOptGrp = optGrp5;
                                    break;
                                }
                            }

                            if ( tHitNum == i ) //无匹配项
                            {
                                if ( tInSubSntns_p )
                                {
                                    free ( tInSubSntns_p->owner );
                                    free ( tInSubSntns_p );
                                }

                                free ( tOutValChkHdr_p );
                                return XMLRL_FMT_UNKNOWN;
                            }

                            break;
                        }

                        case 4:
                        {
                            for ( i = 0; i < tHitNum; i++ )
                            {
                                tKey = opt3_4Cnst[i];

                                if ( !strcmp ( tTst, tKey ) ) //匹配
                                {
                                    tValChk_p->ChkParam.strOptGrp = optGrp3;
                                    break;
                                }
                            }

                            if ( tHitNum == i ) //无匹配项
                            {
                                for ( i = 0; i < tHitNum; i++ )
                                {
                                    tKey = opt4_4Cnst[i];

                                    if ( !strcmp ( tTst, tKey ) ) //匹配
                                    {
                                        tValChk_p->ChkParam.strOptGrp = optGrp4;
                                        break;
                                    }
                                }

                                if ( tHitNum == i ) //仍无匹配项
                                {
                                    if ( tInSubSntns_p )
                                    {
                                        free ( tInSubSntns_p->owner );
                                        free ( tInSubSntns_p );
                                    }

                                    free ( tOutValChkHdr_p );
                                    return XMLRL_FMT_UNKNOWN;
                                }
                            }

                            break;
                        }

                        case 6:
                        {
                            for ( i = 0; i < tHitNum; i++ )
                            {
                                tKey = opt6_6Cnst[i];

                                if ( !strcmp ( tTst, tKey ) ) //匹配
                                {
                                    tValChk_p->ChkParam.strOptGrp = optGrp6;
                                    break;
                                }
                            }

                            if ( tHitNum == i ) //无匹配项
                            {
                                if ( tInSubSntns_p )
                                {
                                    free ( tInSubSntns_p->owner );
                                    free ( tInSubSntns_p );
                                }

                                free ( tOutValChkHdr_p );
                                return XMLRL_FMT_UNKNOWN;
                            }

                            break;
                        }

                        case 8:
                        {
                            for ( i = 0; i < tHitNum; i++ )
                            {
                                tKey = opt2_8Cnst[i];

                                if ( !strcmp ( tTst, tKey ) ) //匹配
                                {
                                    tValChk_p->ChkParam.strOptGrp = optGrp2;
                                    break;
                                }
                            }

                            if ( tHitNum == i ) //无匹配项
                            {
                                if ( tInSubSntns_p )
                                {
                                    free ( tInSubSntns_p->owner );
                                    free ( tInSubSntns_p );
                                }

                                free ( tOutValChkHdr_p );
                                return XMLRL_FMT_UNKNOWN;
                            }

                            break;
                        }

                        default:
                        {
                            if ( tInSubSntns_p )
                            {
                                free ( tInSubSntns_p->owner );
                                free ( tInSubSntns_p );
                            }

                            free ( tOutValChkHdr_p );
                            return XMLRL_FMT_UNKNOWN;
                        }
                    }

                    tValChk_p = tValChk_p->nxt;
                    tSubSntns_p = tSubSntns_p->nxt;
                    break;
                }

                default://未知
                {
                    tSubSntns_p = tInSubSntns_p;

                    if ( tInSubSntns_p )
                    {
                        free ( tInSubSntns_p->owner );
                        free ( tInSubSntns_p );
                    }

                    free ( tOutValChkHdr_p );
                    return XMLRL_ATTR_FMT;
                }
            }
        }

        if ( tInSubSntns_p )
        {
            free ( tInSubSntns_p->owner );
            free ( tInSubSntns_p );
        }

        *valChk_out_p = tOutValChkHdr_p;//可以为空，此时只可能是表达式为空
        return XMLRL_SUCCESS;
    }

    //去空格和tab，tgtChkTrc.valStr存放值的标准格式字符串
    //去后串仍放入str中
    inline XMLRL_PARSE_ERROR dltBlnkAndTab ( char* str )
    {
        if ( !str )
        {
            return XMLRL_SUCCESS;
        }

        char* tInStr = str;
        unsigned long long strLen;
        char* tTstStr = NULL;
        strLen = strlen ( tInStr );
        tTstStr = ( char* ) calloc ( 1, strLen + 1 );

        if ( !tTstStr )
        {
            return XMLRL_MEM_ALLOC;
        }

        strcpy ( tTstStr, tInStr );

        for ( unsigned int i = 0, j = 0; i <= strLen; i++ ) //带'/0'
        {
            if ( ( ' ' == tTstStr[i] ) || ( '\t' == tTstStr[i] ) || ( '\r' == tTstStr[i] ) || ( '\n' == tTstStr[i] ) )
            {
                continue;
            }
            else
            {
                if ( tInStr )
                {
                    tInStr[j++] = tTstStr[i];
                }
            }
        }

        free ( tTstStr );
        return XMLRL_SUCCESS;
    }

    //功能：检查跟踪的目标节点值是否有效，分配值存储空间挂接到元素目标块中，若通过检查
    //      值存储空间内存储解析后的可直接使用的应用数据。应用数据按目标节点组织，内含
    //      节点值和它的所有属性值，本方法仅分配并解析应用节点值，所有属性值检查和解析
    //      通过调用chkTrcNdAttrsVld()实现
    //参数：无
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    chkTrcNdValVld()
    {
        XMLRL_PARSE_ERROR tRet;

        ele_Tgt* tEleTgt_p = tgtChkTrc.trcNd_p;
        ele_Std* tEleStd_p = tEleTgt_p->tgtNdStd_p;
        eleRl* tEleRl_p = tEleStd_p->std_p;

        /*--------------------放入loadXMLFile()中----------------------------*/
        //
        //valChkMethod*作为ele_Std中的一个域
        //即使是非叶子结点，其值也需要检查--是否为“non-null”等非数据值要求
        //解析规则字符串语句，得到规则的标准子句
        //仅在无子句时，输出的子句描述列表才为空（tSubSntnsDsrb_p）
        const char* tNdValStd_p = NULL;
        subSntnsDsrb* tSubSntnsDsrb_p = NULL;
        tNdValStd_p = tEleRl_p->eleValRl;//无值规则valCk，已加一个默认non-null，即目标文件不能空节点
        tRet = subSntnsParse ( tNdValStd_p, &tSubSntnsDsrb_p );

        if ( tRet )
        {
            if ( tSubSntnsDsrb_p )
            {
                free ( tSubSntnsDsrb_p->owner );
                free ( tSubSntnsDsrb_p );
            }

            return tRet;
        }

        //在规则标准子句在得到检查方法
        valChkMethod* tValChkMethod_p = NULL;
        tRet = expressParse ( tSubSntnsDsrb_p, &tValChkMethod_p );

        if ( tRet )
        {
            if ( !tValChkMethod_p )
            {
                free ( tValChkMethod_p );
            }

            return tRet;
        }

        //
        /*--------------------放入loadXMLFile()中----------------------------*/

        //tValChkMethod_p为检查方法，下面按此检查方法对目标节点值执行检查
        //在无规则整句时，tValChkMethod_p才为空，此时意味着不需要检查，
        //但若有目标值仍需要规格化后取出
        tgtNd* tTgtNd_p = tEleTgt_p->tgtNd_p;

        //设置初始应用数据挂接点，并设置此节点初始应用值
        tEleTgt_p->ndAppData.sbkType = STG_NODE;
        tEleTgt_p->ndAppData.dStgNum = 0;

        //通过规则检查的目标节点数据保存在数据存储块中（dataSB类型），实体为元素目标节点的
        //ndAppData域，下面为此数据存储块分配通用存储体占用空间（stgSBBdy类型：dataSU或
        //dataGRP，亦或mDataImg类型三种类型之一），具体采用哪种类型待后面确定。每个通用存储
        //体的空间首地址保存在数据存储块（ndAppData）静态分配的ndAppData.dStgLst域内，该域
        //最大有XML_MAX_ATTRNUM个单元，用来存放各个存储体空间的首地址。这些单元的使用规则是
        //第1单元永远用于存放代表节点值的通用存储体地址，后面的单元用于存放节点属性值的通用
        //存储体地址。非叶子节点无节点值存储，因此不分配通用存储体空间，即此单元保存空地址
        if ( tEleTgt_p->ndAppData.dStgNum > XML_MAX_ATTRNUM + 1 )
        {
            return XMLRL_MEM_ALLOC;
        }

        dStgSBBdy_p tStgNdData_p = NULL;//stgSBBdy通用存储体指针类型

        if ( tEleTgt_p->childNum )
        {
            tEleTgt_p->ndAppData.dStgLst[tEleTgt_p->ndAppData.dStgNum++] = tStgNdData_p;
            tEleTgt_p->ndAppData.name = tEleTgt_p->tgtNd_p->tgtNdNm;
        }
        else
        {
            tStgNdData_p = ( dStgSBBdy_p ) calloc ( 1, sizeof ( dStgSBBdy ) );

            if ( !tStgNdData_p )
            {
                return XMLRL_MEM_ALLOC;
            }

            tEleTgt_p->ndAppData.dStgLst[tEleTgt_p->ndAppData.dStgNum++] = tStgNdData_p;
            tEleTgt_p->ndAppData.name = tEleTgt_p->tgtNd_p->tgtNdNm;
        }

        //去空格和tab，tgtChkTrc.valStr存放值的标准格式字符串
        tRet = dltBlnkAndTab ( tTgtNd_p->tgtNdValStr );

        if ( tRet )
        {
            free ( tValChkMethod_p );
            return tRet;
        }

        /*  unsigned long long strLen;
            if (!tTgtNd_p->tgtNdValStr)
                strLen = 0;
            else
                strLen = strlen(tTgtNd_p->tgtNdValStr);
            tTstVal_p = (char*)calloc(1, strLen + 1);
            if (!tTstVal_p)
            {
                free(tValChkMethod_p);
                return XMLRL_MEM_ALLOC;
            }
            if (!strLen)
                *tTstVal_p = '\0';
            else
                strcpy(tTstVal_p, tTgtNd_p->tgtNdValStr);

            for (unsigned int i = 0, j = 0; i <= strLen; i++)//带'\0'
            {
                if ((' ' == tTstVal_p[i]) || ('/t' == tTstVal_p[i]))
                    i++;
                else
                {
                    if (tTgtNd_p->tgtNdValStr)
                        tTgtNd_p->tgtNdValStr[j] = tTstVal_p[i];
                }
            }
            free(tTstVal_p);*/

        char* tTstVal_p = NULL;
        tTstVal_p = tTgtNd_p->tgtNdValStr;

        //分析并检查标准化后的节点取值字符串：tTstVal_p，
        //该串的检查方法在tValChkMethod_p中
        tEleRl_p->eleValType = STR_WORD;//设置默认节点值类型
        char** tPurValStr_p = NULL;
        int tPureValStrNum = 0;
        int tRcdValFlag = 0;//未记录
        ndSmpVal_p tempNdSmpVal_p;
        valChkMethod* tValChkMthdHdr = tValChkMethod_p;
        ndSmpVal_p tNdSmpVal_p = NULL;//mDataImg类型

        while ( tValChkMethod_p )
        {
            switch ( tValChkMethod_p->chkMethod )
            {
                //节点中NON_NULL必存在，无参数规则，叶子必须有值，非空对非叶子无作用，其前
                //可能有分隔符子句，也可能没有，若有首先进入的分支是SPTR，然后才是NON_NULL
                case NON_NULL:
                {
                    if ( !tEleTgt_p->childNum ) //叶子
                    {
                        if ( !tTstVal_p )
                        {
                            free ( tValChkMthdHdr );
                            //                          free(tStgNdData_p);
                            return XMLCHK_NODE_VALUE;
                        }

                        //tPureValStrNum = 0说明其前无SPTR子句，是一个简单值节点，否则
                        //tPurVal_p空间由SPTR分支分配，tPureValStrNum + 1为简单值个数。此处
                        //分配一个简单节点字符串值首地址空间实体，通用存储体采用元数据映像型
                        //（mDataImg），元数据映像空间（void* mImg）还未分配
                        if ( !tPureValStrNum )
                        {
                            tPurValStr_p = ( char** ) calloc ( 1, sizeof ( char* ) );
                            tPurValStr_p[tPureValStrNum++] = tTstVal_p;
                        }
                    }

                    //取此节点值的下一规则检查方法
                    tValChkMethod_p = tValChkMethod_p->nxt;
                    break;
                }

                case BANDUP://tTstVal_p形如：<xxx>{a分隔符b分隔符c}</xxx>，要求a,b,c均不同
                {
                    //若有sptr:分隔符子句，一定出现在其后，不论是否存在sptr:，一定出现在
                    //non-null子句之后，若无sptr:子句认为是单一值，不存在重复值问题，直接通过
                    if ( 1 == tPureValStrNum ) //无sptr:
                    {
                        tValChkMethod_p = tValChkMethod_p->nxt;
                        break;
                    }
                    else//有sptr:，因此已生成纯值字符串数组（tPurVal_p），检查它们是否重复
                    {
                        for ( int i = 0; i < tPureValStrNum - 1; i++ )
                        {
                            for ( int j = i + 1; j < tPureValStrNum; j++ )
                            {
                                if ( strstr ( tPurValStr_p[i], tPurValStr_p[j] ) ) //找到--重复
                                {
                                    free ( tValChkMthdHdr );
                                    //                                  free(tStgNdData_p);
                                    return XMLCHK_NODE_VALUE;
                                }
                            }
                        }

                        tValChkMethod_p = tValChkMethod_p->nxt;
                        break;
                    }
                }

                //复合值分解成多个单一值，要求其后必须是str:[...]或num:[...]
                //规则检查已保证sptr[...]一定出现在str:[...]或num:[...]之前
                //允许目标节点的实际赋值中无此分隔符
                case SPTR://必为叶子，且多分量值，若存在一定在tValChkMethod_p的第一块中允许目标文
                    //件中因只是个单值，没有书写分隔符的情况，此时只会分一个值的空间（tPurValStr_p）
                    //得到节点值的分量数：tPureValNum
                {
                    char* tPos_p, *tStartPos_p, *tEndPos_p;
                    unsigned long long stepLng = strlen ( tValChkMethod_p->valStdSprt );
                    tStartPos_p = tTstVal_p;
                    tEndPos_p = tTstVal_p + strlen ( tTstVal_p ); //'\0'处

                    while ( tStartPos_p < tEndPos_p )
                    {
                        tPos_p = strstr ( tStartPos_p, tValChkMethod_p->valStdSprt );

                        if ( tPos_p ) //找到分量
                        {
                            tPureValStrNum++;
                            tStartPos_p = tPos_p + stepLng;

                            if ( *tStartPos_p == '\0' )
                            {
                                free ( tValChkMthdHdr );
                                //                              free(tStgNdData_p);
                                return XMLCHK_NODE_VALUE;
                            }

                            continue;
                        }

                        break;
                    }

                    tPurValStr_p = ( char** ) calloc ( tPureValStrNum + 1, sizeof ( char* ) ); //数据值比隔符多1个

                    if ( !tPurValStr_p )
                    {
                        free ( tValChkMthdHdr );
                        return XMLRL_MEM_ALLOC;
                    }

                    //为节点应用值的各个分量分配存储空间
                    //分配元数据映像列表空间+元数据映像空间
                    tStgNdData_p->GRPBdy.dGType = STG_NVMX;//多值结构
                    tStgNdData_p->GRPBdy.name = tEleRl_p->eleNmRl;
                    tStgNdData_p->GRPBdy.dStgNum = tPureValStrNum + 1;//分量值个数
                    tNdSmpVal_p = ( ndSmpVal_p ) calloc ( tPureValStrNum + 1, sizeof ( mDataImg ) );

                    if ( !tNdSmpVal_p )
                    {
                        free ( tPurValStr_p );
                        free ( tValChkMthdHdr );
                        //                      free(tStgNdData_p);
                        return XMLRL_MEM_ALLOC;
                    }

                    //通用存储体采用数据组型（dataGRP），组内包含多个元数据映像块列表
                    //列表头指针保存在GRPBdy.dStgLst中，元数据映像空间（void* mImg）还未分配
                    tStgNdData_p->GRPBdy.dStgLst = tNdSmpVal_p;

                    //将目标节点字符串值（tTstVal_p）分解为简单字符串值，各简单字符串值的起始
                    //地址保存已分配空间的tPurValStr_p数组中，在目标节点字符串值中将简单字符串
                    //值的结束位置替换为'\0'（分隔符的第1个字符）
                    tStartPos_p = tTstVal_p;
                    tPureValStrNum = 0;

                    while ( tStartPos_p < tEndPos_p ) //使tPureValStrNum成为真实计数
                    {
                        tPos_p = strstr ( tStartPos_p, tValChkMethod_p->valStdSprt );

                        if ( tPos_p )
                        {
                            tPurValStr_p[tPureValStrNum++] = tStartPos_p;//后续值的起始位置
                            *tPos_p = '\0';//加结束符
                            tStartPos_p = tPos_p + stepLng;
                        }
                        else//已无分隔符，将最后一个数值记下来
                        {
                            tPurValStr_p[tPureValStrNum++] = tStartPos_p;
                            break;
                        }
                    }

                    tValChkMethod_p = tValChkMethod_p->nxt;
                    break;
                }

                case NUM://若其前没有sptr:[...]，则认为节点值为单一值，否则所有分量均此规则
                {
                    tEleRl_p->eleValType = NUM_WORD;//更新值规则类型为正整数数值型

                    //保存解析后的应用分量值，分配映像空间
                    if ( 1 == tPureValStrNum ) //单值，取元素数据映像的存储地址
                    {
                        if ( tStgNdData_p->GRPBdy.dStgLst ) //sptr分支已分，但目标文件中是个单值
                        {
                            free ( tStgNdData_p->GRPBdy.dStgLst );    //释放它，后面专门为单值分配
                        }

                        tNdSmpVal_p = &tStgNdData_p->mImgBdy;
                    }
                    else//多值，取元素数据映像块的存储地址
                    {
                        tNdSmpVal_p = ( mDataImg* ) ( tStgNdData_p->GRPBdy.dStgLst );
                    }

                    tempNdSmpVal_p = tNdSmpVal_p;
                    //分配元数据实体，规则只支持>=0的整数，且均按int型分配空间
                    long long* tImgBdy_p = NULL;

                    for ( int i = 0; i < tPureValStrNum; i++ )
                    {
                        tImgBdy_p = ( long long* ) calloc ( 1, sizeof ( long long ) );

                        if ( !tImgBdy_p )
                        {
                            free ( tPurValStr_p );
                            //if (!tNdSmpVal_p)
                            //  free(tNdSmpVal_p);
                            free ( tValChkMthdHdr );
                            //free(tStgNdData_p);
                            return XMLRL_MEM_ALLOC;
                        }

                        tempNdSmpVal_p->mImg = tImgBdy_p;//指向非负整数
                        tempNdSmpVal_p->mLng = sizeof ( long long );
                        tempNdSmpVal_p->mNum = 1;
                        tempNdSmpVal_p->mType = NUM_ZP;

                        if ( 1 == tPureValStrNum )
                        {
                            tempNdSmpVal_p->name = tEleRl_p->eleNmRl;
                        }
                        else
                        {
                            tempNdSmpVal_p->name = tEleRl_p->eleNmRl;
                        }

                        tempNdSmpVal_p++;//若为多值，取列表中下一简单值地址存储位置
                    }

                    tempNdSmpVal_p = tNdSmpVal_p;

                    for ( int i = 0; i < tPureValStrNum; i++ ) //tPureValNum已经为值的个数
                    {
                        //                      int tNumTstVal;
                        //                      tNumTstVal = atoi(tPurValStr_p[i]);//遇非数字自动结束
                        long long tNumTstVal;

                        if ( numStrTonum ( tPurValStr_p[i], &tNumTstVal, 10 ) )
                        {
                            tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                            free ( tPurValStr_p );
                            free ( tValChkMthdHdr );
                            return XMLCHK_NODE_VALUE;
                        }

                        /*                      long long lng = strlen(tPurValStr_p[i]);
                                                //检查是否存在非数字字符，最大支持10位十进制正整数
                                                if (lng > 10)
                                                {
                                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                        //                          free(tImgBdy_p);
                        //                          free(tStgNdData_p);
                                                    free(tPurValStr_p);
                                                    free(tValChkMthdHdr);
                                                    return XMLCHK_NODE_VALUE;
                                                }
                                                char restoreStr[11];
                                                _itoa(tNumTstVal, restoreStr, 10);
                                                if (strcmp(restoreStr, tPurValStr_p[i]))
                                                {
                                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                        //                          free(tImgBdy_p);
                        //                          free(tStgNdData_p);
                                                    free(tPurValStr_p);
                                                    free(tValChkMthdHdr);
                                                    return XMLCHK_NODE_VALUE;
                                                }*/
                        //进行边界检查
                        if ( ( tNumTstVal > tValChkMethod_p->ChkParam.numChk.maxVal ) ||
                             ( tNumTstVal < tValChkMethod_p->ChkParam.numChk.minVal ) )
                        {
                            tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                            //                          free(tImgBdy_p);
                            //                          free(tStgNdData_p);
                            free ( tPurValStr_p );
                            free ( tValChkMthdHdr );
                            return XMLCHK_NODE_VALUE;
                        }

                        //记录解析后的合法节点应用值
                        * ( long long* ) ( tempNdSmpVal_p->mImg ) = tNumTstVal; //存入非负整数
                        tempNdSmpVal_p++;//若为多值，取列表中下一简单值地址存储位置
                        tRcdValFlag = 1;//值已记录
                    }

                    tValChkMethod_p = tValChkMethod_p->nxt;
                    break;
                }

                case STR://若其前没有sptr:[...]，则认为节点值为单一值
                {
                    tEleRl_p->eleValType = STR_OPT;

                    //保存解析后的应用分量值，分配映像空间
                    if ( 1 == tPureValStrNum ) //单值，无分隔符，由non-null分支分配tPurVal_p空间
                    {
                        if ( tStgNdData_p->GRPBdy.dStgLst ) //sptr分支已分，但目标文件中是个单值
                        {
                            free ( tStgNdData_p->GRPBdy.dStgLst );    //释放它，后面专门为单值分配
                        }

                        tNdSmpVal_p = &tStgNdData_p->mImgBdy;
                    }
                    else
                    {
                        tNdSmpVal_p = ( mDataImg* ) ( tStgNdData_p->GRPBdy.dStgLst );
                    }

                    tempNdSmpVal_p = tNdSmpVal_p;
                    //分配元数据实体
                    char* tImgBdy_p = NULL;

                    for ( int i = 0; i < tPureValStrNum; i++ )
                    {
                        tImgBdy_p = ( char* ) calloc ( 1, sizeof ( char* ) ); //所有字符串取值均只保存指针，不另分配空间

                        if ( !tImgBdy_p )
                        {
                            free ( tPurValStr_p );
                            //if (!tNdSmpVal_p)
                            //  free(tNdSmpVal_p);
                            free ( tValChkMthdHdr );
                            //free(tStgNdData_p);
                            return XMLRL_MEM_ALLOC;
                        }

                        tempNdSmpVal_p->mImg = tImgBdy_p;//指向字符串首地址
                        tempNdSmpVal_p->mLng = sizeof ( char* );
                        tempNdSmpVal_p->mNum = 1;
                        tempNdSmpVal_p->mType = PTR_ALL;

                        if ( 1 == tPureValStrNum )
                        {
                            tempNdSmpVal_p->name = tEleRl_p->eleNmRl;
                        }
                        else
                        {
                            tempNdSmpVal_p->name = tEleRl_p->eleNmRl;
                        }

                        tempNdSmpVal_p++;
                    }

                    tempNdSmpVal_p = tNdSmpVal_p;

                    for ( int i = 0; i < tPureValStrNum; i++ ) //tPureValNum已经为值的个数
                    {
                        char* tStrTstVal = NULL;
                        tStrTstVal = tPurValStr_p[i];

                        //进行选项检查
                        switch ( tValChkMethod_p->ChkParam.strOptGrp )
                        {
                                int tOptNum;
                                int i;

                            case optGrp1:
                            {
                                tOptNum = 2;

                                for ( i = 0; i < tOptNum; i++ )
                                {
                                    if ( !strcmp ( tStrTstVal, opt1_2Cnst[i] ) )
                                    {
                                        break;
                                    }
                                }

                                if ( tOptNum == i ) //无匹配选项
                                {
                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                    //free(tNdSmpVal_p->mImg);
                                    //if (!tNdSmpVal_p)
                                    //  free(tNdSmpVal_p);
                                    //free(tStgNdData_p);
                                    free ( tPurValStr_p );
                                    free ( tValChkMthdHdr );
                                    return XMLCHK_NODE_VALUE;
                                }

                                break;
                            }

                            case optGrp2:
                            {
                                tOptNum = 8;

                                for ( i = 0; i < tOptNum; i++ )
                                {
                                    if ( !strcmp ( tStrTstVal, opt2_8Cnst[i] ) )
                                    {
                                        break;
                                    }
                                }

                                if ( tOptNum == i ) //无匹配选项
                                {
                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                    //free(tNdSmpVal_p->mImg);
                                    //if (!tNdSmpVal_p)
                                    //  free(tNdSmpVal_p);
                                    //free(tStgNdData_p);
                                    free ( tPurValStr_p );
                                    free ( tValChkMthdHdr );
                                    return XMLCHK_NODE_VALUE;
                                }

                                break;
                            }

                            case optGrp3:
                            {
                                tOptNum = 4;

                                for ( i = 0; i < tOptNum; i++ )
                                {
                                    if ( !strcmp ( tStrTstVal, opt3_4Cnst[i] ) )
                                    {
                                        break;
                                    }
                                }

                                if ( tOptNum == i ) //无匹配选项
                                {
                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                    //free(tNdSmpVal_p->mImg);
                                    //if (!tNdSmpVal_p)
                                    //  free(tNdSmpVal_p);
                                    //free(tStgNdData_p);
                                    free ( tPurValStr_p );
                                    free ( tValChkMthdHdr );
                                    return XMLCHK_NODE_VALUE;
                                }

                                break;
                            }

                            case optGrp4:
                            {
                                tOptNum = 4;

                                for ( i = 0; i < tOptNum; i++ )
                                {
                                    if ( !strcmp ( tStrTstVal, opt4_4Cnst[i] ) )
                                    {
                                        break;
                                    }
                                }

                                if ( tOptNum == i ) //无匹配选项
                                {
                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                    //free(tNdSmpVal_p->mImg);
                                    //if (!tNdSmpVal_p)
                                    //  free(tNdSmpVal_p);
                                    //free(tStgNdData_p);
                                    free ( tPurValStr_p );
                                    free ( tValChkMthdHdr );
                                    return XMLCHK_NODE_VALUE;
                                }

                                break;
                            }

                            case optGrp5:
                            {
                                tOptNum = 3;

                                for ( i = 0; i < tOptNum; i++ )
                                {
                                    if ( !strcmp ( tStrTstVal, opt5_3Cnst[i] ) )
                                    {
                                        break;
                                    }
                                }

                                if ( tOptNum == i ) //无匹配选项
                                {
                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                    //free(tNdSmpVal_p->mImg);
                                    //if (!tNdSmpVal_p)
                                    //  free(tNdSmpVal_p);
                                    //free(tStgNdData_p);
                                    free ( tPurValStr_p );
                                    free ( tValChkMthdHdr );
                                    return XMLCHK_NODE_VALUE;
                                }

                                break;
                            }

                            case optGrp6:
                            {
                                tOptNum = 6;

                                for ( i = 0; i < tOptNum; i++ )
                                {
                                    if ( !strcmp ( tStrTstVal, opt6_6Cnst[i] ) )
                                    {
                                        break;
                                    }
                                }

                                if ( tOptNum == i ) //无匹配选项
                                {
                                    tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                    //free(tNdSmpVal_p->mImg);
                                    //if (!tNdSmpVal_p)
                                    //  free(tNdSmpVal_p);
                                    //free(tStgNdData_p);
                                    free ( tPurValStr_p );
                                    free ( tValChkMthdHdr );
                                    return XMLCHK_NODE_VALUE;
                                }

                                break;
                            }

                            default:
                            {
                                tempNdSmpVal_p->mType = VAL_ILG;        //值非法
                                //free(tNdSmpVal_p->mImg);
                                //if (!tNdSmpVal_p)
                                //  free(tNdSmpVal_p);
                                //free(tStgNdData_p);
                                free ( tPurValStr_p );
                                free ( tValChkMthdHdr );
                                return XMLCHK_NODE_VALUE;
                            }
                        }

                        //记录解析后的合法节点应用值
                        * ( char** ) ( tempNdSmpVal_p->mImg ) = tStrTstVal; //存入字符串首地址
                        tempNdSmpVal_p++;
                        tRcdValFlag = 1;//值已记录
                    }

                    tValChkMethod_p = tValChkMethod_p->nxt;
                    break;
                }

                default:
                {
                    free ( tPurValStr_p );
                    free ( tValChkMthdHdr );
                    //                  free(tStgNdData_p);
                    return XMLCHK_NODE_VALUE;
                }
            }
        }

        //有值但还未记录，可能是tValChkMthdHdr空（不需要检查）
        //值可能是有分隔符的，也可能是无分隔符的，尚未分配空间
        if ( ( !tRcdValFlag ) && ( tTgtNd_p->tgtNdValStr ) )
        {
            ndSmpVal_p tNdSmpVal_p = NULL;

            if ( 1 == tPureValStrNum ) //为单分量值分配空间
            {
                tStgNdData_p->mImgBdy.mType = PTR_FUL;//单值结构
                tStgNdData_p->mImgBdy.name = tEleRl_p->eleNmRl;
                tStgNdData_p->mImgBdy.mNum = 1;//分量值个数
                tNdSmpVal_p = &tStgNdData_p->mImgBdy;
            }
            else
            {
                tNdSmpVal_p = ( mDataImg* ) ( tStgNdData_p->GRPBdy.dStgLst );
            }

            tempNdSmpVal_p = tNdSmpVal_p;
            //分配元数据实体

            for ( int i = 0; i < tPureValStrNum; i++ )
            {
                char* tImgBdy_p = NULL;
                tImgBdy_p = ( char* ) calloc ( 1, sizeof ( char* ) );

                if ( !tImgBdy_p )
                {
                    free ( tPurValStr_p );
                    free ( tValChkMthdHdr );
                    //                  free(tStgNdData_p);
                    return XMLRL_MEM_ALLOC;
                }

                tempNdSmpVal_p->mImg = tImgBdy_p;//指向字符串首地址
                tempNdSmpVal_p->mLng = sizeof ( char* );
                tempNdSmpVal_p->mNum = 1;
                tempNdSmpVal_p->mType = PTR_FUL;

                if ( 1 == tPureValStrNum )
                {
                    tempNdSmpVal_p->name = tEleRl_p->eleNmRl;
                }
                else
                {
                    tempNdSmpVal_p->name = NULL;
                }

                * ( char** ) tempNdSmpVal_p->mImg = tPurValStr_p[i]; //存入字符串首地址
                tempNdSmpVal_p++;
            }
        }

        free ( tPurValStr_p );
        free ( tValChkMthdHdr );
        return XMLRL_SUCCESS;
    }

    //功能：检查跟踪节点一个属性的有效性
    //参1：输入，属性规则列表头地址指针
    //参2：输入，规则要求的属性数
    //参3：输入，目标节点的属性列表头地址指针
    //参4：输入，属性顺序号
    //参5：输入，应用数据指针
    //返回值：0为成功；非0为发生错误
    inline XMLRL_PARSE_ERROR
    chkAttrVld ( attrRl* stdAttrLstHdr, unsigned char stdAttrNum, tgtAttr* tgtAttrLstHdr,
                 unsigned char attrID, ndData* appDataStart_p )
    {
        if ( stdAttrNum > XML_MAX_ATTRNUM )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLRL_PARSE_ERROR tRet;

        attrRl* tInStdAttrLstHdr = stdAttrLstHdr;
        unsigned char tInStdAttrNum = stdAttrNum;
        tgtAttr* tInTgtAttrLstHdr = tgtAttrLstHdr;
        ndData* tInAppData_p = appDataStart_p;
        unsigned char tInAttrID = attrID;

        tgtAttr* tTgtAttr_p = NULL;
        const char* tTgtAttrNm = NULL;
        char* tTgtAttrValStr = NULL;

        //取目标属性名和属性值
        tTgtAttr_p = tInTgtAttrLstHdr + tInAttrID;
        tTgtAttrNm = tTgtAttr_p->tgtAtrrNm;
        tTgtAttrValStr = ( char* ) tTgtAttr_p->tgtAtrrValStr;

        //得到属性值检查规则：tAttrValStd_p
        const char*  tAttrValStd_p = NULL;
        attrRl* tAttrStd_p = NULL;
        int i;

        for ( i = 0; i < tInStdAttrNum; i++ )
        {
            tAttrStd_p = tInStdAttrLstHdr + i;

            if ( !strcmp ( tAttrStd_p->atrrNmRl, tTgtAttrNm ) )
            {
                tAttrValStd_p = tAttrStd_p->atrrValRl;
                break;
            }
        }

        if ( tInStdAttrNum == i )
        {
            return XMLCHK_ATTR_NAME;    //找不到对应的规则
        }

        //生成属性值的检查方法：解析规则字符串语句，得到规则的标准子句
        subSntnsDsrb* tSubSntnsDsrb_p = NULL;
        tRet = subSntnsParse ( tAttrValStd_p, &tSubSntnsDsrb_p );

        if ( tRet )
        {
            if ( !tSubSntnsDsrb_p )
            {
                free ( tSubSntnsDsrb_p );
            }

            return tRet;
        }

        valChkMethod* tAttrValChkMethod_p = NULL;
        tRet = expressParse ( tSubSntnsDsrb_p, &tAttrValChkMethod_p );

        if ( tRet )
        {
            if ( tAttrValChkMethod_p )
            {
                free ( tAttrValChkMethod_p );
            }

            return tRet;
        }

        ////////////////////值检查：tTgtAttrValStr<- -->tAttrValStd_p，方法tAttrValChkMethodHdr_p
        tAttrStd_p->attrValType = STR_WORD;//设置此属性默认值类型为字符串型
        char* tTstAttrVal_p = NULL;
        valChkMethod* tAttrValChkMethodHdr_p = tAttrValChkMethod_p;

        //规格化目标属性值：去空格和tab
        unsigned long long strLen = strlen ( tTgtAttrValStr );
        tTstAttrVal_p = ( char* ) calloc ( 1, strLen + 1 );

        if ( !tTstAttrVal_p )
        {
            if ( tAttrValChkMethodHdr_p )
            {
                free ( tAttrValChkMethodHdr_p );
            }

            return XMLRL_MEM_ALLOC;
        }

        strcpy ( tTstAttrVal_p, tTgtAttrValStr );

        for ( unsigned int i = 0, j = 0; i <= strLen; i++ ) //带'\0'
        {
            if ( ( ' ' == tTstAttrVal_p[i] ) || ( '\t' == tTstAttrVal_p[i] ) || ( '\r' == tTstAttrVal_p[i] ) || ( '\n' == tTstAttrVal_p[i] ) )
            {
                continue;
            }
            else
            {
                tTgtAttrValStr[j++] = tTstAttrVal_p[i];
            }
        }

        free ( tTstAttrVal_p );
        tTstAttrVal_p = tTgtAttrValStr;//tTstAttrVal_p可以为'\0'

        //分配一个通用存储体块，用作属性组中的一个存储体--元数据映像（仅允许属性取单值），元数据类型尚不确定
        //若无值属性，直接设置，不分配存储空间
        if ( tInAppData_p->dStgNum > ( XML_MAX_ATTRNUM + 1 ) ) //未释放全部空间
        {
            return XMLRL_MEM_ALLOC;
        }

        attrSmpVal_p tStgAttrData_p = NULL;
        tStgAttrData_p = ( attrSmpVal_p ) calloc ( 1, sizeof ( attrSmpVal ) );

        if ( !tStgAttrData_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        tInAppData_p->dStgLst[tInAppData_p->dStgNum++] = ( dStgSBBdy_p ) tStgAttrData_p;
        tStgAttrData_p->name = tTgtAttrNm;
        int tRcdValFlag = 0;

        //本规则中不支持带分隔符的属性值，出现视为格式错误
        //本规则中不支持属性名的重复，若出现重复，属XML基本格式错误，由XML基础模块tinyxml2负责检查
        //多个规则检查子句无顺序依赖性
        while ( tAttrValChkMethod_p )
        {
            switch ( tAttrValChkMethod_p->chkMethod )
            {
                case NON_NULL://无参数
                {
                    if ( '\0' == tTstAttrVal_p[0] )
                    {
                        free ( tAttrValChkMethodHdr_p );
                        //                      free(tStgAttrData_p);
                        return XMLCHK_ATTR_VALUE;
                    }

                    tAttrValChkMethod_p = tAttrValChkMethod_p->nxt;//取此属性值的下一规则检查方法
                    break;
                }

                case NUM://目前规则要求属性只能为单一值
                {
                    tAttrStd_p->attrValType = NUM_WORD;//更新此属性值为正整数数值型
                    //为该属性应用值的各个分量分配存储空间，因只支持单值，故应为元数据映像存储
                    tStgAttrData_p->mLng = sizeof ( long long );
                    tStgAttrData_p->mNum = 1;
                    tStgAttrData_p->mType = NUM_ZP;
                    long long* tImgBdy_p = ( long long* ) calloc ( 1, sizeof ( long long ) ); //所有整数值均按int型分配空间

                    if ( !tImgBdy_p )
                    {
                        //                      free(tStgAttrData_p);
                        free ( tAttrValChkMethodHdr_p );
                        return XMLRL_MEM_ALLOC;
                    }

                    tStgAttrData_p->mImg = tImgBdy_p;//指向整数存储空间

                    //                  int tNumTstAttrVal;
                    //                  tNumTstAttrVal = atoi(tTstAttrVal_p);//遇非数字自动结束

                    long long tNumTstAttrVal;

                    if ( numStrTonum ( tTstAttrVal_p, &tNumTstAttrVal, 10 ) )
                    {
                        free ( tAttrValChkMethodHdr_p );
                        return XMLCHK_ATTR_VALUE;
                    }

                    //检查是否存在非数字字符，最大支持10位十进制正整数
                    /*                  long long lng = strlen(tTstAttrVal_p);
                                        //检查是否存在非数字字符，最大支持10位十进制正整数
                                        if (lng > 10)
                                        {
                                            tStgAttrData_p->mType = VAL_ILG;
                    //                      free(tStgAttrData_p->mImg);
                    //                      free(tStgAttrData_p);
                                            free(tAttrValChkMethodHdr_p);
                                            return XMLCHK_ATTR_VALUE;
                                        }
                                        char restoreStr[11];
                                        _itoa(tNumTstAttrVal, restoreStr, 10);
                                        if (strcmp(restoreStr, tTstAttrVal_p))
                                        {
                                            tStgAttrData_p->mType = VAL_ILG;
                    //                      free(tStgAttrData_p->mImg);
                    //                      free(tStgAttrData_p);
                                            free(tAttrValChkMethodHdr_p);
                                            return XMLCHK_ATTR_VALUE;
                                        }*/

                    //进行边界检查，空间未全部释放
                    if ( ( tNumTstAttrVal > tAttrValChkMethod_p->ChkParam.numChk.maxVal ) ||
                         ( tNumTstAttrVal < tAttrValChkMethod_p->ChkParam.numChk.minVal ) )
                    {
                        tStgAttrData_p->mType = VAL_ILG;
                        //                      free(tStgAttrData_p->mImg);
                        //                      free(tStgAttrData_p);
                        free ( tAttrValChkMethodHdr_p );
                        return XMLCHK_ATTR_VALUE;
                    }

                    //记录属性值
                    * ( long long* ) ( tStgAttrData_p->mImg ) = tNumTstAttrVal; //写入整数存储空间
                    tRcdValFlag = 1;
                    tAttrValChkMethod_p = tAttrValChkMethod_p->nxt;
                    break;
                }

                case STR://目前规则要求属性只能为单一值
                {
                    tAttrStd_p->attrValType = STR_OPT;
                    //为该属性应用值的各个分量分配存储空间，因只支持单值，故应为元数据映像存储
                    tStgAttrData_p->mLng = sizeof ( char* );
                    tStgAttrData_p->mNum = 1;
                    tStgAttrData_p->mType = PTR_FUL;//必须有值
                    char* tImgBdy_p = ( char* ) calloc ( 1, sizeof ( char* ) ); //所有字符串均只分配指针空间

                    if ( !tImgBdy_p ) //未释放全部空间
                    {
                        //                      free(tStgAttrData_p);
                        free ( tAttrValChkMethodHdr_p );
                        return XMLRL_MEM_ALLOC;
                    }

                    tStgAttrData_p->mImg = tImgBdy_p;//指向地址存储空间

                    char* tStrTstAttrVal;
                    tStrTstAttrVal = tTstAttrVal_p;

                    //进行选项检查
                    switch ( tAttrValChkMethod_p->ChkParam.strOptGrp )
                    {
                            int tOptNum;

                        case optGrp1:
                        {
                            tOptNum = 2;

                            for ( i = 0; i < tOptNum; i++ )
                            {
                                if ( !strcmp ( tStrTstAttrVal, opt1_2Cnst[i] ) )
                                {
                                    break;
                                }
                            }

                            if ( tOptNum == i ) //无匹配选项，未释放全部空间
                            {
                                tStgAttrData_p->mType = VAL_ILG;
                                //                              free(tStgAttrData_p->mImg);
                                //                              free(tStgAttrData_p);
                                free ( tAttrValChkMethodHdr_p );
                                return XMLCHK_ATTR_VALUE;
                            }

                            break;
                        }

                        case optGrp2:
                        {
                            tOptNum = 8;

                            for ( i = 0; i < tOptNum; i++ )
                            {
                                if ( !strcmp ( tStrTstAttrVal, opt2_8Cnst[i] ) )
                                {
                                    break;
                                }
                            }

                            if ( tOptNum == i ) //无匹配选项，未释放全部空间
                            {
                                tStgAttrData_p->mType = VAL_ILG;
                                //                              free(tStgAttrData_p->mImg);
                                //                              free(tStgAttrData_p);
                                free ( tAttrValChkMethodHdr_p );
                                return XMLCHK_ATTR_VALUE;
                            }

                            break;
                        }

                        case optGrp3:
                        {
                            tOptNum = 4;

                            for ( i = 0; i < tOptNum; i++ )
                            {
                                if ( !strcmp ( tStrTstAttrVal, opt3_4Cnst[i] ) )
                                {
                                    break;
                                }
                            }

                            if ( tOptNum == i ) //无匹配选项，未释放全部空间
                            {
                                tStgAttrData_p->mType = VAL_ILG;
                                //                              free(tStgAttrData_p->mImg);
                                //                              free(tStgAttrData_p);
                                free ( tAttrValChkMethodHdr_p );
                                return XMLCHK_ATTR_VALUE;
                            }

                            break;
                        }

                        case optGrp4:
                        {
                            tOptNum = 4;

                            for ( i = 0; i < tOptNum; i++ )
                            {
                                if ( !strcmp ( tStrTstAttrVal, opt4_4Cnst[i] ) )
                                {
                                    break;
                                }
                            }

                            if ( tOptNum == i ) //无匹配选项，未释放全部空间
                            {
                                tStgAttrData_p->mType = VAL_ILG;
                                //                              free(tStgAttrData_p->mImg);
                                //                              free(tStgAttrData_p);
                                free ( tAttrValChkMethodHdr_p );
                                return XMLCHK_ATTR_VALUE;
                            }

                            break;
                        }

                        case optGrp5:
                        {
                            tOptNum = 3;

                            for ( i = 0; i < tOptNum; i++ )
                            {
                                if ( !strcmp ( tStrTstAttrVal, opt5_3Cnst[i] ) )
                                {
                                    break;
                                }
                            }

                            if ( tOptNum == i ) //无匹配选项，未释放全部空间
                            {
                                tStgAttrData_p->mType = VAL_ILG;
                                //                              free(tStgAttrData_p->mImg);
                                //                              free(tStgAttrData_p);
                                free ( tAttrValChkMethodHdr_p );
                                return XMLCHK_ATTR_VALUE;
                            }

                            break;
                        }

                        case optGrp6:
                        {
                            tOptNum = 6;

                            for ( i = 0; i < tOptNum; i++ )
                            {
                                if ( !strcmp ( tStrTstAttrVal, opt6_6Cnst[i] ) )
                                {
                                    break;
                                }
                            }

                            if ( tOptNum == i ) //无匹配选项，未释放全部空间
                            {
                                tStgAttrData_p->mType = VAL_ILG;
                                //                              free(tStgAttrData_p->mImg);
                                //                              free(tStgAttrData_p);
                                free ( tAttrValChkMethodHdr_p );
                                return XMLCHK_ATTR_VALUE;
                            }

                            break;
                        }

                        default:
                        {
                            tStgAttrData_p->mType = VAL_ILG;
                            //                          free(tStgAttrData_p->mImg);
                            //                          free(tStgAttrData_p);
                            free ( tAttrValChkMethodHdr_p );
                            return XMLCHK_ATTR_VALUE;
                        }
                    }

                    //记录属性值
                    * ( char** ) tStgAttrData_p->mImg = tTstAttrVal_p; //存入字符串首地址
                    tRcdValFlag = 1;
                    tAttrValChkMethod_p = tAttrValChkMethod_p->nxt;
                    break;
                }

                default:
                {
                    tStgAttrData_p->mType = VAL_ILG;
                    //                  if(!tStgAttrData_p->mImg)
                    //                      free(tStgAttrData_p->mImg);
                    //                  free(tStgAttrData_p);
                    free ( tAttrValChkMethodHdr_p );
                    return XMLCHK_ATTR_VALUE;
                }
            }
        }

        //记录无检查要求的属性值
        if ( ( !tRcdValFlag ) && ( tTstAttrVal_p ) )
        {
            //为该属性应用值的各个分量分配存储空间，因只支持单值，故应为元数据映像存储
            tStgAttrData_p->mLng = sizeof ( char* );
            tStgAttrData_p->mNum = 1;
            tStgAttrData_p->mType = PTR_FUL;//必须有值
            char* tImgBdy_p = ( char* ) calloc ( 1, sizeof ( char* ) ); //所有字符串均只分配指针空间

            if ( !tImgBdy_p ) //未释放全部空间
            {
                //              free(tStgAttrData_p);
                free ( tAttrValChkMethodHdr_p );
                return XMLRL_MEM_ALLOC;
            }

            tStgAttrData_p->mImg = tImgBdy_p;
            * ( char** ) tStgAttrData_p->mImg = tTstAttrVal_p;
        }

        free ( tAttrValChkMethodHdr_p );
        return XMLRL_SUCCESS;
    }

    //功能：检查跟踪节点所有属性的有效性，通过调用chkAttrVld()逐一检查每个属性的有效性
    //参数：无
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    chkTrcNdAttrsVld()
    {
        XMLRL_PARSE_ERROR tRet = XMLRL_SUCCESS;

        ele_Tgt* tEleTgt_p = tgtChkTrc.trcNd_p;

        //实际节点
        if ( !tEleTgt_p )
        {
            return XMLCHK_UNKNOWN_ERROR;
        }

        tgtNd* tTgtNd_p = tEleTgt_p->tgtNd_p;
        tgtAttr* tTgtAttrLstHdr = tTgtNd_p->tgtNdAttrLst;
        unsigned char tTgtAttrNum = tTgtNd_p->tgtNdAttrNum;
        const char* tTgtAttrNm = NULL;
        //节点规则
        ele_Std* tEleStd_p = tEleTgt_p->tgtNdStd_p;

        if ( !tEleStd_p )
        {
            return XMLCHK_UNKNOWN_ERROR;
        }

        eleRl* tNdStd_p = tEleStd_p->std_p;
        attrRl* tStdAttrLstHdr = tNdStd_p->attrRlLst;
        unsigned char tStdAttrNum = tNdStd_p->attrRlNum;

        ndData* tDataStart_p = & ( tEleTgt_p->ndAppData );

        for ( unsigned char i = 0; i < tTgtAttrNum; i++ )
        {
            tTgtAttrNm = ( tTgtAttrLstHdr + i )->tgtAtrrNm;
            tRet = chkAttrVld ( tStdAttrLstHdr, tStdAttrNum, tTgtAttrLstHdr, i, tDataStart_p );

            if ( tRet )
            {
                return tRet;
            }
        }

        return tRet;
    }

    inline XMLRL_PARSE_ERROR chkSiblingNds ( unsigned char lvlNo, ele_Tgt* pEleTgt_p )
    {
        unsigned char tInLvlNo = lvlNo;
        ele_Tgt* tInPEleTgt_p = pEleTgt_p;

        if ( !tInLvlNo ) //根不检查
        {
            return XMLRL_SUCCESS;
        }

        //换父亲前检查所有目标子节点的必备性和有无未定义的目标节点
        if ( !tInPEleTgt_p->tgtNdStd_p->childNum ) //规则要求无子（无子规则）
            if ( tInPEleTgt_p->childNum ) //实际有孩子
            {
                return XMLCHK_CHILD_LARGE;
            }

        //并不是规则要求的节点在目标中必须存在，要检查它的必备性
        ele_Std* tEleStdNeed_p = NULL;
        ele_Tgt* tOneEleTgtNd_p = NULL;

        ele_Tgt** xChrgAddr = tInPEleTgt_p->cTgtNd_p;
        int i = 0, j = 0;

        for ( i = 0; i < tInPEleTgt_p->childNum; i++ ) //取实际孩子名
        {
            if ( *xChrgAddr ) //说明已将自己设置到父亲上
            {
                xChrgAddr = tInPEleTgt_p->cTgtNd_p + i;
                tOneEleTgtNd_p = *xChrgAddr;

                for ( j = 0; j < tInPEleTgt_p->tgtNdStd_p->childNum; j++ ) //取规则孩子名
                {
                    tEleStdNeed_p = tInPEleTgt_p->tgtNdStd_p->cEleStd_p[j];

                    if ( strcmp ( tEleStdNeed_p->std_p->eleNmRl, tOneEleTgtNd_p->tgtNd_p->tgtNdNm ) )
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }

                if ( tInPEleTgt_p->tgtNdStd_p->childNum == j ) //实际孩子有不在规则中的情况
                {
                    return XMLRL_CHILD_NAME;
                }
            }

        }

        //所有孩子（目标）均在规则中，检查是否少必备孩子（目标）
        tEleStdNeed_p = NULL;
        tOneEleTgtNd_p = NULL;
        xChrgAddr = tInPEleTgt_p->cTgtNd_p;

        for ( i = 0; i < tInPEleTgt_p->tgtNdStd_p->childNum; i++ ) //取规则孩子名
        {
            tEleStdNeed_p = tInPEleTgt_p->tgtNdStd_p->cEleStd_p[i];

            if ( !tEleStdNeed_p->std_p->isNeed ) //非必备
            {
                tEleStdNeed_p = NULL;
                continue;
            }

            //tEleStdNeed_p：必备，检查是否在实际子中
            for ( j = 0; j < tInPEleTgt_p->childNum; j++ )
            {
                if ( *xChrgAddr ) //说明已将自己设置到父亲上
                {
                    xChrgAddr = tInPEleTgt_p->cTgtNd_p + j;
                    tOneEleTgtNd_p = *xChrgAddr;

                    //          tOneEleTgtNd_p = tInPEleTgt_p->cTgtNd_p[j];
                    if ( strcmp ( tEleStdNeed_p->std_p->eleNmRl, tOneEleTgtNd_p->tgtNd_p->tgtNdNm ) )
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if ( tInPEleTgt_p->childNum == j ) //存在一个必备节点，不在实际节点中
            {
                return XMLCHK_CHILD_LACK;
            }
        }

        return XMLRL_SUCCESS;
    }

    //功能：检查目标文件的合法性，将解析数据记录到解析链表内，只要发生非法记录，
    //      链表全部销毁
    //参数：目标文件名
    //返回值：错误码，0为成功；非0为发生错误
    XMLRL_PARSE_ERROR xmlStd::
    chkTgtXmlFile ( const char* tgtFileNm )
    {
        //检查输入参数
        if ( !tgtFileNm )
        {
            return XMLRL_FUN_PARAM;
        }

        XMLRL_PARSE_ERROR tRet;
        const char* inTgtFileNm = tgtFileNm;

        if ( tgtDoc_p )
        {
            return XMLRL_OBJATTR_INST;
        }

        XMLDocument* tTgtdDoc_p = new XMLDocument();

        if ( !tTgtdDoc_p )
        {
            return XMLRL_MEM_ALLOC;
        }

        tTgtdDoc_p->LoadFile ( inTgtFileNm );
        tRet = ( XMLRL_PARSE_ERROR ) ( tTgtdDoc_p->ErrorID() );

        if ( tRet )
        {
            return tRet;
        }

        tgtDoc_p = tTgtdDoc_p;

        //从根节点开始
        ele_TgtLvlHdr[0].tgtEle_p = tTgtdDoc_p->RootElement();//<目标文件根节点>

        if ( !ele_TgtLvlHdr[0].tgtEle_p )
        {
            return XMLCHK_ERROR_EMPTY_DOCUMENT;
        }

        tgtLvlNum = 0;

        //创建根节点
        ele_Tgt* tPEleTgtHdr_p = NULL;  //父层头节点
        ele_Tgt* tPEleTgt_p = NULL;     //父节点
        ele_Tgt* tEleTgtHdr_p = &ele_TgtLvlHdr[0];
        ele_Tgt* tEleTgt_p = NULL;
        XMLElement* tPTgtEle_p = NULL;  //父节点xml元素
        XMLElement* tTgtEle_p = ele_TgtLvlHdr[0].tgtEle_p;
        unsigned char tUsingEleTgtCnt = 0;

        while ( tTgtEle_p )
        {
            clrTgtChkTrcNd();//清空跟踪点
            tRet = crtEleTgt ( &tEleTgtHdr_p, 0, 0, &tEleTgt_p ); //建元素目标块：tEleTgt_p

            if ( tRet )
            {
                return tRet;
            }

            tUsingEleTgtCnt++;

            if ( tUsingEleTgtCnt != 1 ) //存在多个根
            {
                return XMLRL_FMT_ROOT;
            }

            //创建跟踪点，应用数据并未设置，也未挂至管理链中
            tRet = buildTrcNd ( tTgtEle_p, tPEleTgt_p, tEleTgt_p, tgtLvlNum );

            if ( tRet )
            {
                return tRet;
            }

            tRet = alsTrcNd();

            if ( tRet )
            {
                return tRet;
            }

            tRet = chkTrcNdValVld();//分析检查点上的应用数据

            if ( tRet )
            {
                return tRet;
            }

            tRet = chkTrcNdAttrsVld();

            if ( tRet )
            {
                return tRet;
            }

            //将该父亲元素的下一个子元素作为目标元素
            tTgtEle_p = tTgtEle_p->NextSiblingElement();
        }

        //第1层开始
        tgtLvlNum = 1;
        tPEleTgtHdr_p = &ele_TgtLvlHdr[0];  //父层头节点
        tUsingEleTgtCnt = 0;

        /////////////////////开始第tTgtLvlNum，非根层
        while ( tPEleTgtHdr_p ) //遍历各层
        {
            //每层的开始
            tPEleTgt_p = tPEleTgtHdr_p;
            tPTgtEle_p = tPEleTgt_p->tgtEle_p;
            tTgtEle_p = tPTgtEle_p->FirstChildElement();
            tEleTgtHdr_p = &ele_TgtLvlHdr[tgtLvlNum];

            while ( tPEleTgt_p ) //遍历tTgtLvlNum层的所有父亲：<sysCfg>{<sysInfo>}...</sysCfg>
            {
                while ( tTgtEle_p ) //从第1个孩子开始，遍历tTgtLvlNum层1个父亲的所有孩子
                {
                    clrTgtChkTrcNd();//清空跟踪点
                    tRet = crtEleTgt ( &tEleTgtHdr_p, tgtLvlNum, tUsingEleTgtCnt, &tEleTgt_p ); //建元素目标块：tEleTgt_p

                    if ( tRet )
                    {
                        return tRet;
                    }

                    tUsingEleTgtCnt++;

                    //创建跟踪点，应用数据并未设置，也未挂至管理链中
                    tRet = buildTrcNd ( tTgtEle_p, tPEleTgt_p, tEleTgt_p, tgtLvlNum );

                    if ( tRet )
                    {
                        return tRet;
                    }

                    tRet = alsTrcNd();

                    if ( tRet )
                    {
                        return tRet;
                    }

                    tRet = chkTrcNdValVld();//分析检查点上的应用数据

                    if ( tRet )
                    {
                        return tRet;
                    }

                    tRet = chkTrcNdAttrsVld();

                    if ( tRet )
                    {
                        return tRet;
                    }

                    //将该父亲元素的下一个子元素作为目标元素
                    tTgtEle_p = tTgtEle_p->NextSiblingElement();
                }

                //换父亲前检查所有兄弟
                tRet = chkSiblingNds ( tgtLvlNum, tPEleTgt_p );

                if ( tRet )
                {
                    return tRet;
                }

                if ( !tPEleTgt_p )
                {
                    break;
                }

                tPEleTgt_p = tPEleTgt_p->nxt;//换父层的下一父亲//<sysInfo>{<hstInfo>}...

                if ( !tPEleTgt_p ) //已无父亲，去下一层
                {
                    break;
                }

                tTgtEle_p = tPEleTgt_p->tgtEle_p;
                //得到父的第1个孩子
                tTgtEle_p = tTgtEle_p->FirstChildElement();//<hstInfo>{<hstState>}...</hstInfo>
            }

            //本层完成，判断是否文件结束
            tPEleTgtHdr_p = &ele_TgtLvlHdr[tgtLvlNum];//换下一层，上一层作父亲

            while ( !tPEleTgtHdr_p->childNum ) //找第一个非叶子节点的父亲
            {
                tPEleTgtHdr_p = tPEleTgtHdr_p->nxt;

                if ( !tPEleTgtHdr_p )
                {
                    //为目标节点分配唯一标识码（ID）
                    unsigned int cntr = 0;

                    for ( int i = 0; i <= tgtLvlNum; i++ )
                    {
                        ele_TgtLvlHdr[i].ele_TgtID = cntr++;
                        tEleTgt_p = ele_TgtLvlHdr[i].nxt;

                        while ( tEleTgt_p )
                        {
                            tEleTgt_p->ele_TgtID = cntr++;
                            tEleTgt_p = tEleTgt_p->nxt;
                        }
                    }

                    return XMLRL_FILE_END;
                }
            }

            tUsingEleTgtCnt = 0;
            tgtLvlNum++;

            if ( tgtLvlNum >= XML_MAX_LVLNUM )
            {
                return XMLRL_LVL_LARGE;
            }
        }

        unsigned int cntr = 0;

        for ( int i = 0; i <= tgtLvlNum; i++ )
        {
            ele_TgtLvlHdr[i].ele_TgtID = cntr++;
            tEleTgt_p = ele_TgtLvlHdr[i].nxt;

            while ( tEleTgt_p )
            {
                tEleTgt_p->ele_TgtID = cntr++;
                tEleTgt_p = tEleTgt_p->nxt;
            }
        }

        return XMLRL_FILE_END;
    }

    //功能：根据层号和节点名，以及它的属性名和属性取值字符串，找符合条件的所有节点，
    //      并返回它们的所有应用数据，最多支持XML_MAX_ATTRNUM + 1个节点
    //参1：输入，节点名
    //参2：输入，节点层号，0是根节点编号
    //参3：输入，节点的给定属性名
    //参4：输入，节点给定属性名的取值字符串
    //返回值：符合查找条件的所有节点的取值列表，每节点包含节点自身和所有属性的应用数据值
    ndDataLst xmlStd::
    getNdByAttr ( const char* ndNm, unsigned int lvlNo, const char* attrNm,
                  const char* attrNmVal )
    {
        if ( ( !ndNm ) || ( lvlNo >= XML_MAX_LVLNUM ) )
        {
            return NULL;
        }

        if ( ( attrNm ) && ( !attrNmVal ) ) //有属性名时，不允许属性值为空
        {
            return NULL;
        }

        const char* tInNdNm = ndNm;
        int tInLvlNo = lvlNo;
        const char* tInNdAttrNmVal = attrNmVal;
        const char* tInAttrNm = attrNm;

        const char* tAttrNm = NULL;
        unsigned char tAttrNum = 0;
        tgtNd* tTgtNd_p = NULL;
        ele_Tgt* tFindTgtNd[XML_MAX_WIDTH];
        int tNum = 0;
        ele_Tgt* tEleTget_p = NULL;
        tEleTget_p = &ele_TgtLvlHdr[tInLvlNo];

        if ( tEleTget_p )
        {
            tTgtNd_p = tEleTget_p->tgtNd_p;
        }
        else
        {
            return NULL;
        }

        while ( tTgtNd_p )
        {
            if ( strcmp ( tInNdNm, tTgtNd_p->tgtNdNm ) )
            {
                tEleTget_p = tEleTget_p->nxt;

                if ( !tEleTget_p )
                {
                    if ( !tNum )
                    {
                        return NULL;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    tTgtNd_p = tEleTget_p->tgtNd_p;
                }

                continue;
            }

            if ( tInAttrNm ) //需要匹配属性
            {
                int i;
                tAttrNum = tTgtNd_p->tgtNdAttrNum;

                for ( i = 0; i < tAttrNum; i++ )
                {
                    tAttrNm = ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrNm;

                    if ( !strcmp ( tInAttrNm, tAttrNm ) )
                    {
                        break;
                    }
                }

                if ( tAttrNum == i ) //存在属性名时，无匹配属性名称
                {
                    tEleTget_p = tEleTget_p->nxt;

                    if ( !tEleTget_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTget_p->tgtNd_p;
                    }

                    continue;
                }

                //存在属性名时，匹配属性取值
                if ( strcmp ( tInNdAttrNmVal, ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrValStr ) ) //所给属性取值不一致
                {
                    tEleTget_p = tEleTget_p->nxt;

                    if ( !tEleTget_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTget_p->tgtNd_p;
                    }

                    continue;
                }
            }

            //tTgtNd_p为完全匹配的节点，记录下来，然后找下一节点
            tFindTgtNd[tNum++] = tEleTget_p;
            tEleTget_p = tEleTget_p->nxt;

            if ( !tEleTget_p )
            {
                if ( !tNum )
                {
                    return NULL;
                }
                else
                {
                    break;
                }
            }
            else
            {
                tTgtNd_p = tEleTget_p->tgtNd_p;
            }
        }

        if ( ( tNum > XML_MAX_WIDTH ) || ( !tNum ) ) //超过查找最大节点数限制或未找到
        {
            return NULL;
        }

        ndDataLst tRet_p;
        char* tMemPoolStart;
        tMemPoolStart = ( char* ) calloc ( 1, sizeof ( dataSBLst ) + tNum * sizeof ( dataSB* ) );

        if ( !tMemPoolStart )
        {
            return NULL;
        }

        //管理动态分配的内存，在对象释放时一同释放，防止用户未释放导致内存泄漏
        ndData_p* ndBuf;
        ndBuf = ( ndData_p* ) ( tMemPoolStart + sizeof ( dataSBLst ) );
        tRet_p = ( ndDataLst ) tMemPoolStart;
        tRet_p->sbNum = tNum;
        tRet_p->sbBdy = ndBuf;
        tRet_p->stub = loadIntoMemMgr ( tMemPoolStart );

        if ( !tRet_p->stub )
        {
            return NULL;
        }

        for ( int i = 0; i < tNum; i++ )
        {
            tEleTget_p = tFindTgtNd[i];
            tRet_p->sbBdy[i] = & ( tEleTget_p->ndAppData );
        }

        return tRet_p;
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
    ndDataLst xmlStd::
    getNdByPAttrNmVal ( const char* ndNm, unsigned int lvlNo, const char* pNdNm,
                        const char* pNdAttrNmVal )
    {
        if ( ( !ndNm ) || ( lvlNo >= XML_MAX_LVLNUM ) || ( !pNdNm ) || ! ( pNdAttrNmVal ) )
        {
            return NULL;
        }

        const char* tInNdNm = ndNm;
        int tInLvlNo = lvlNo;
        const char* tInPNdAttrNmVal = pNdAttrNmVal;
        const char* tInPNdNm = pNdNm;
        const char* tPAttrNm = "name";

        if ( lvlNo < 2 ) //根节点无父亲，作为父亲的根节点无属性（规则要求）
        {
            return NULL;
        }

        int tNum = 0;
        tgtNd* tTgtNd_p;
        ele_Tgt* tFindNd[XML_MAX_WIDTH];
        tgtNd* tParentTgtNd_p;
        ele_Tgt* tEleTgt_p = NULL;
        int tPAttrNum;
        const char* tAttrNm;

        tEleTgt_p = &ele_TgtLvlHdr[tInLvlNo];

        if ( tEleTgt_p )
        {
            tTgtNd_p = tEleTgt_p->tgtNd_p;
        }
        else
        {
            return NULL;
        }

        while ( tTgtNd_p )
        {
            //匹配目标节点名
            if ( strcmp ( tInNdNm, tTgtNd_p->tgtNdNm ) )
            {
                tEleTgt_p = tEleTgt_p->nxt;

                if ( !tEleTgt_p )
                {
                    if ( !tNum )
                    {
                        return NULL;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    tTgtNd_p = tEleTgt_p->tgtNd_p;
                }

                continue;
            }
            else//找到一个目标节点，再匹配它的父节点
            {
                if ( !tEleTgt_p->pTgtNd_p ) //目标节点所在层就不存在父节点
                {
                    return NULL;
                }

                //父节点名称不匹配，继续找下一个目标节点
                tParentTgtNd_p = tEleTgt_p->pTgtNd_p->tgtNd_p;

                if ( strcmp ( tParentTgtNd_p->tgtNdNm, tInPNdNm ) )
                {
                    tEleTgt_p = tEleTgt_p->nxt;

                    if ( !tEleTgt_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTgt_p->tgtNd_p;
                    }

                    continue;
                }

                //继续匹配父是否有name属性
                int i;
                tgtNd* tPrePTgtNd_p = NULL;
                tPAttrNum = tParentTgtNd_p->tgtNdAttrNum;

                for ( i = 0; i < tPAttrNum; i++ )
                {
                    tAttrNm = ( tParentTgtNd_p->tgtNdAttrLst + i )->tgtAtrrNm;

                    if ( !strcmp ( tPAttrNm, tAttrNm ) ) //name属性
                    {
                        break;
                    }
                }

                if ( i == tPAttrNum ) //父节点无name属性，继续找
                {
                    tEleTgt_p = tEleTgt_p->nxt;

                    if ( !tEleTgt_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTgt_p->tgtNd_p;
                    }

                    continue;
                }

                //继续匹配父的name属性取值
                if ( strcmp ( tInPNdAttrNmVal,      //不匹配，继续下一目标节点
                              ( tParentTgtNd_p->tgtNdAttrLst + i )->tgtAtrrValStr ) )
                {
                    tEleTgt_p = tEleTgt_p->nxt;

                    if ( !tEleTgt_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTgt_p->tgtNd_p;
                    }

                    continue;
                }

                //父的name属性取值已匹配，检查是否存在多个这样的父亲
                if ( ( tPrePTgtNd_p ) && ( tPrePTgtNd_p != tParentTgtNd_p ) )
                {
                    return NULL;    //存在多个父亲
                }

                //记录本轮循环中找到的完全符合要求的目标节点，更换下一个目标节点后继续
                tPrePTgtNd_p = tParentTgtNd_p;
                tFindNd[tNum++] = tEleTgt_p;
                tEleTgt_p = tEleTgt_p->nxt;

                if ( !tEleTgt_p )
                {
                    if ( !tNum )
                    {
                        return NULL;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    tTgtNd_p = tEleTgt_p->tgtNd_p;
                }
            }
        }

        if ( !tNum ) //未找到
        {
            return NULL;
        }

        ndDataLst tRet_p;
        char* tMemPoolStart;
        tMemPoolStart = ( char* ) calloc ( 1, sizeof ( dataSBLst ) + tNum * sizeof ( dataSB* ) );

        if ( !tMemPoolStart )
        {
            return NULL;
        }

        ndData_p* ndBuf;
        ndBuf = ( ndData_p* ) ( tMemPoolStart + sizeof ( dataSBLst ) );
        tRet_p = ( ndDataLst ) tMemPoolStart;
        tRet_p->sbNum = tNum;
        tRet_p->sbBdy = ndBuf;
        tRet_p->stub = loadIntoMemMgr ( tMemPoolStart );

        if ( !tRet_p->stub )
        {
            return NULL;
        }

        for ( int i = 0; i < tNum; i++ )
        {
            tEleTgt_p = tFindNd[i];
            tRet_p->sbBdy[i] = & ( tEleTgt_p->ndAppData );
        }

        return tRet_p;
    }

    ndDataLst xmlStd::
    getNdByPAttr ( const char* ndNm, unsigned int lvlNo, const char* pNdNm,
                   const char* pNdAttrNm, const char* pNdAttrVal )
    {
        if ( ( !ndNm ) || ( lvlNo >= XML_MAX_LVLNUM ) || ( !pNdNm ) || ( !pNdAttrNm ) || ! ( pNdAttrVal ) )
        {
            return NULL;
        }

        const char* tInNdNm = ndNm;
        int tInLvlNo = lvlNo;
        const char* tInPAttrNm = pNdAttrNm;
        const char* tInPNdAttrVal = pNdAttrVal;
        const char* tInPNdNm = pNdNm;

        if ( lvlNo < 2 ) //根节点无父亲，作为父亲的根节点无属性（规则要求）
        {
            return NULL;
        }

        int tNum = 0;
        tgtNd* tTgtNd_p;
        ele_Tgt* tFindNd[XML_MAX_WIDTH];
        tgtNd* tParentTgtNd_p;
        ele_Tgt* tEleTgt_p = NULL;
        int tPAttrNum;
        const char* tAttrNm;

        tEleTgt_p = &ele_TgtLvlHdr[tInLvlNo];

        if ( tEleTgt_p )
        {
            tTgtNd_p = tEleTgt_p->tgtNd_p;
        }
        else
        {
            return NULL;
        }

        while ( tTgtNd_p )
        {
            //匹配目标节点名
            if ( strcmp ( tInNdNm, tTgtNd_p->tgtNdNm ) )
            {
                tEleTgt_p = tEleTgt_p->nxt;

                if ( !tEleTgt_p )
                {
                    if ( !tNum )
                    {
                        return NULL;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    tTgtNd_p = tEleTgt_p->tgtNd_p;
                }

                continue;
            }
            else//找到一个目标节点，再匹配它的父节点
            {
                if ( !tEleTgt_p->pTgtNd_p ) //目标节点所在层就不存在父节点
                {
                    return NULL;
                }

                //父节点名称不匹配，继续找下一个目标节点
                tParentTgtNd_p = tEleTgt_p->pTgtNd_p->tgtNd_p;

                if ( strcmp ( tParentTgtNd_p->tgtNdNm, tInPNdNm ) )
                {
                    tEleTgt_p = tEleTgt_p->nxt;

                    if ( !tEleTgt_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTgt_p->tgtNd_p;
                    }

                    continue;
                }

                //继续匹配父是否有name属性
                int i;
                tgtNd* tPrePTgtNd_p = NULL;
                tPAttrNum = tParentTgtNd_p->tgtNdAttrNum;

                for ( i = 0; i < tPAttrNum; i++ )
                {
                    tAttrNm = ( tParentTgtNd_p->tgtNdAttrLst + i )->tgtAtrrNm;

                    if ( !strcmp ( tInPAttrNm, tAttrNm ) ) //属性
                    {
                        break;
                    }
                }

                if ( i == tPAttrNum ) //父节点无name属性，继续找
                {
                    tEleTgt_p = tEleTgt_p->nxt;

                    if ( !tEleTgt_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTgt_p->tgtNd_p;
                    }

                    continue;
                }

                //继续匹配父的name属性取值
                if ( strcmp ( tInPNdAttrVal,        //不匹配，继续下一目标节点
                              ( tParentTgtNd_p->tgtNdAttrLst + i )->tgtAtrrValStr ) )
                {
                    tEleTgt_p = tEleTgt_p->nxt;

                    if ( !tEleTgt_p )
                    {
                        if ( !tNum )
                        {
                            return NULL;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTgt_p->tgtNd_p;
                    }

                    continue;
                }

                //父的name属性取值已匹配，检查是否存在多个这样的父亲
                if ( ( tPrePTgtNd_p ) && ( tPrePTgtNd_p != tParentTgtNd_p ) )
                {
                    return NULL;    //存在多个父亲
                }

                //记录本轮循环中找到的完全符合要求的目标节点，更换下一个目标节点后继续
                tPrePTgtNd_p = tParentTgtNd_p;
                tFindNd[tNum++] = tEleTgt_p;
                tEleTgt_p = tEleTgt_p->nxt;

                if ( !tEleTgt_p )
                {
                    if ( !tNum )
                    {
                        return NULL;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    tTgtNd_p = tEleTgt_p->tgtNd_p;
                }
            }
        }

        if ( !tNum ) //未找到
        {
            return NULL;
        }

        ndDataLst tRet_p;
        char* tMemPoolStart;
        tMemPoolStart = ( char* ) calloc ( 1, sizeof ( dataSBLst ) + tNum * sizeof ( dataSB* ) );

        if ( !tMemPoolStart )
        {
            return NULL;
        }

        ndData_p* ndBuf;
        ndBuf = ( ndData_p* ) ( tMemPoolStart + sizeof ( dataSBLst ) );
        tRet_p = ( ndDataLst ) tMemPoolStart;
        tRet_p->sbNum = tNum;
        tRet_p->sbBdy = ndBuf;
        tRet_p->stub = loadIntoMemMgr ( tMemPoolStart );

        if ( !tRet_p->stub )
        {
            return NULL;
        }

        for ( int i = 0; i < tNum; i++ )
        {
            tEleTgt_p = tFindNd[i];
            tRet_p->sbBdy[i] = & ( tEleTgt_p->ndAppData );
        }

        return tRet_p;
    }

    //通过父节点ID找它的孩子ID
    //参1：输入，要求返回的子节点名称；参2：输入，父节点ID；
    //参3：输出，子节点ID缓冲区；参4：输入，子节点ID缓冲区大长
    //参1空时，返回所有子节点；参3空时，只返回子节点个数（所有子节点或指定名称的子节点）
    //返回值：子点个数
    unsigned int xmlStd::
    getNdIDsByPNdID ( const char* ndNm, unsigned int ele_TgtPID, unsigned int* ele_TgtID_p, unsigned int IDNum )
    {
        const char* tInNdNm = ndNm;
        unsigned int tInEle_TgtPID = ele_TgtPID;
        unsigned int* tInEle_TgtID_p = ele_TgtID_p;
        unsigned int tInIDNum = IDNum;
        int tNum;
        int tRet = 0;

        //找父节点
        ele_Tgt* tEle_Tgt_p;

        for ( int i = 0; i <= tgtLvlNum; i++ )
        {
            tEle_Tgt_p = &ele_TgtLvlHdr[i];

            while ( tEle_Tgt_p )
            {
                if ( tEle_Tgt_p->ele_TgtID == tInEle_TgtPID )
                {
                    i = tgtLvlNum;
                    break;
                }

                tEle_Tgt_p = tEle_Tgt_p->nxt;
            }
        }

        if ( !tEle_Tgt_p )
        {
            return 0;
        }

        //找子节点
        ele_Tgt* tE_T_p;
        tNum = tEle_Tgt_p->childNum;

        if ( !tInNdNm ) //无名
        {
            if ( !tInEle_TgtID_p ) //不输出ID
            {
                tRet = tNum;
            }
            else//输出ID
            {
                for ( int i = 0; i < tNum; i++ )
                {
                    tE_T_p = tEle_Tgt_p->cTgtNd_p[i];

                    if ( tRet < tInIDNum )
                    {
                        tInEle_TgtID_p[tRet++] = tE_T_p->ele_TgtID;
                    }
                    else
                    {
                        while ( --tRet )
                        {
                            tInEle_TgtID_p[tRet] = 0;
                        }

                        return 0;
                    }
                }
            }

            return tRet;
        }
        else//指定名
        {
            for ( int i = 0; i < tNum; i++ )
            {
                tE_T_p = tEle_Tgt_p->cTgtNd_p[i];

                if ( !strcmp ( tE_T_p->tgtNdStd_p->std_p->eleNmRl, tInNdNm ) )
                {
                    if ( !tInEle_TgtID_p ) //不输出ID
                    {
                        tRet++;
                    }
                    else//输出ID
                    {
                        if ( tRet < tInIDNum )
                        {
                            tInEle_TgtID_p[tRet++] = tE_T_p->ele_TgtID;
                        }
                        else
                        {
                            while ( --tRet )
                            {
                                tInEle_TgtID_p[tRet] = 0;
                            }

                            return 0;
                        }
                    }
                }
            }
        }

        return tRet;
    }

    //通过ID得到节点数据列表
    ndDataLst xmlStd::
    getNdByNdID ( unsigned int ndID )
    {
        unsigned int tInNdID = ndID;

        //找到节点
        ele_Tgt* tEle_Tgt_p;

        for ( int i = 0; i <= tgtLvlNum; i++ )
        {
            tEle_Tgt_p = &ele_TgtLvlHdr[i];

            while ( tEle_Tgt_p )
            {
                if ( tEle_Tgt_p->ele_TgtID == tInNdID )
                {
                    i = tgtLvlNum;
                    break;
                }

                tEle_Tgt_p = tEle_Tgt_p->nxt;
            }
        }

        if ( !tEle_Tgt_p )
        {
            return NULL;
        }

        //处理并返回数据
        ndDataLst tRet_p;
        char* tMemPoolStart;
        tMemPoolStart = ( char* ) calloc ( 1, sizeof ( dataSBLst ) + sizeof ( dataSB* ) );

        if ( !tMemPoolStart )
        {
            return NULL;
        }

        ndData_p* ndBuf;
        ndBuf = ( ndData_p* ) ( tMemPoolStart + sizeof ( dataSBLst ) );
        tRet_p = ( ndDataLst ) tMemPoolStart;
        tRet_p->sbNum = 1;
        tRet_p->sbBdy = ndBuf;
        tRet_p->stub = loadIntoMemMgr ( tMemPoolStart );

        if ( !tRet_p->stub )
        {
            return NULL;
        }

        tRet_p->sbBdy[0] = & ( tEle_Tgt_p->ndAppData );
        return tRet_p;
    }


    //通过节点属性得到它们的ID
    unsigned int xmlStd::
    getNdIDsByNDAttr ( const char* ndNm, unsigned int lvlNo, const char* attrNm,
                       const char* attrNmVal, unsigned int* ele_TgtID_p, unsigned int IDNum )
    {
        if ( ( !ndNm ) || ( lvlNo >= XML_MAX_LVLNUM ) )
        {
            return 0;
        }

        if ( ( attrNm ) && ( !attrNmVal ) ) //有属性名时，不允许属性值为空
        {
            return 0;
        }

        const char* tInNdNm = ndNm;
        int tInLvlNo = lvlNo;
        const char* tInNdAttrNmVal = attrNmVal;
        const char* tInAttrNm = attrNm;
        unsigned int* tInEle_TgtID_p = ele_TgtID_p;
        unsigned int tInIDNum = IDNum;

        const char* tAttrNm = NULL;
        unsigned char tAttrNum = 0;
        tgtNd* tTgtNd_p = NULL;
        ele_Tgt* tFindTgtNd[XML_MAX_WIDTH];
        int tNum = 0;
        ele_Tgt* tEleTget_p = NULL;
        tEleTget_p = &ele_TgtLvlHdr[tInLvlNo];

        if ( tEleTget_p )
        {
            tTgtNd_p = tEleTget_p->tgtNd_p;
        }
        else
        {
            return 0;
        }

        while ( tTgtNd_p )
        {
            if ( strcmp ( tInNdNm, tTgtNd_p->tgtNdNm ) )
            {
                tEleTget_p = tEleTget_p->nxt;

                if ( !tEleTget_p )
                {
                    if ( !tNum )
                    {
                        return 0;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    tTgtNd_p = tEleTget_p->tgtNd_p;
                }

                continue;
            }

            if ( tInAttrNm ) //需要匹配属性
            {
                int i;
                tAttrNum = tTgtNd_p->tgtNdAttrNum;

                for ( i = 0; i < tAttrNum; i++ )
                {
                    tAttrNm = ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrNm;

                    if ( !strcmp ( tInAttrNm, tAttrNm ) )
                    {
                        break;
                    }
                }

                if ( tAttrNum == i ) //存在属性名时，无匹配属性名称
                {
                    tEleTget_p = tEleTget_p->nxt;

                    if ( !tEleTget_p )
                    {
                        if ( !tNum )
                        {
                            return 0;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTget_p->tgtNd_p;
                    }

                    continue;
                }

                //存在属性名时，匹配属性取值
                if ( strcmp ( tInNdAttrNmVal, ( tTgtNd_p->tgtNdAttrLst + i )->tgtAtrrValStr ) ) //所给属性取值不一致
                {
                    tEleTget_p = tEleTget_p->nxt;

                    if ( !tEleTget_p )
                    {
                        if ( !tNum )
                        {
                            return 0;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        tTgtNd_p = tEleTget_p->tgtNd_p;
                    }

                    continue;
                }
            }

            //tTgtNd_p为完全匹配的节点，记录下来，然后找下一节点
            tFindTgtNd[tNum++] = tEleTget_p;
            tEleTget_p = tEleTget_p->nxt;

            if ( !tEleTget_p )
            {
                if ( !tNum )
                {
                    return 0;
                }
                else
                {
                    break;
                }
            }
            else
            {
                tTgtNd_p = tEleTget_p->tgtNd_p;
            }
        }

        if ( !tInEle_TgtID_p )
        {
            return tNum;
        }

        if ( ( tNum > XML_MAX_WIDTH ) || ( !tNum ) ) //超过查找最大节点数限制或未找到
        {
            return 0;
        }

        if ( tInIDNum < tNum )
        {
            return 0;
        }

        for ( int i = 0; i < tNum; i++ )
        {
            tEleTget_p = tFindTgtNd[i];
            tInEle_TgtID_p[i] = tEleTget_p->ele_TgtID;
        }

        return tNum;
    }
}


