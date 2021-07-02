#include "dataRepresentation.h"
#include <stdlib.h>
#define MAX_DATA_NUM 100
namespace DATA_REPRSENTATION
{
    memMgr* memPoolStart = NULL;
    memMgr* memPool = NULL;


    //功能：释放应用使用的内存，用于在不析构对象的情况下，释放不使用的存储空间，如应用数据
    //参数：输入，内存管理块指针，释放此块以后的存储空间（不含此块）
    //返回值：无
    void loadOffMemMgr ( void* stub, int all )
    {
        if ( !stub )
        {
            return;
        }

        memMgr* tInMemMgrPos = ( memMgr* ) stub;
        int tInAllFlg = all;
        memMgr* tMemMgrPos = tInMemMgrPos->nxt;
        memMgr* tNxtMemMgrPos = NULL;

        if ( tInAllFlg ) //全部释放!=0
        {
            while ( tMemMgrPos )
            {
                free ( tMemMgrPos->memBlk_p );
                tNxtMemMgrPos = tMemMgrPos->nxt;
                free ( tMemMgrPos );
                tMemMgrPos = tNxtMemMgrPos;
            }

            if ( tInMemMgrPos == memPoolStart )
            {
                free ( memPoolStart );
                memPoolStart = NULL;
                memPool = NULL;
            }
            else
            {
                memPool = tInMemMgrPos;
                tInMemMgrPos->nxt = NULL;
            }
        }
        else//只释放下一块
        {
            free ( tMemMgrPos->memBlk_p );
            tInMemMgrPos->nxt = tMemMgrPos->nxt;
            free ( tMemMgrPos );

            if ( !tInMemMgrPos->nxt ) //释放的是最后一块
            {
                memPool = tInMemMgrPos;
            }

            if ( tInMemMgrPos == memPoolStart )
            {
                free ( memPoolStart );
                memPoolStart = NULL;
                memPool = NULL;
            }
            else
            {
                memPool = tInMemMgrPos;
                tInMemMgrPos->nxt = NULL;
            }
        }

        return;
    }

    //功能：将零散难以集中回收的内存空间加入到内存管理块中，以便以后随时释放
    //参数：输入，内存管理块指针，指明新分配的内存空间的挂接点
    //返回值：错误码，0为成功；非0为发生错误
    void* loadIntoMemMgr ( void* memPtr )
    {
        if ( !memPtr )
        {
            return NULL;
        }

        void* tInMemPtr = memPtr;
        void* tRet = NULL;

        if ( !memPoolStart ) //建一个空头块
        {
            memPoolStart = ( memMgr* ) calloc ( 1, sizeof ( memMgr ) );

            if ( !memPoolStart )
            {
                return NULL;
            }

            memPool = memPoolStart;
        }

        tRet = memPool;
        memPool = ( memMgr* ) calloc ( 1, sizeof ( memMgr ) );

        if ( !memPool )
        {
            return NULL;
        }

        ( ( memMgr* ) tRet )->nxt = memPool;
        memPool->memBlk_p = tInMemPtr;
        return tRet;
    }


}