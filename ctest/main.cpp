#include <stdio.h>
#include <string.h>
#include "WriteLog.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <map>
#include <stdlib.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct cb_node
{
    char  CB_Obj[10];
    char  CB_Func[10];
    struct cb_node* CB_Next;
}CB_NODE;

std::map< std::string, CB_NODE* > mCB;

unsigned char attachEventHandler( 
        const char* event,
        const char* obj,
        const char* func 
        )
{
    std::string tEvent( event );
    std::map<std::string, CB_NODE*>::iterator it;

    PrintToStdout( LOGDEBUG, "attach event=[%s]",tEvent.c_str() );
    printf("%s:%s:%d fusktest\n",__FILE__,__FUNCTION__,__LINE__);

    for( it = mCB.begin(); it != mCB.end(); it++ )
    {
        if( it->first == tEvent )
        {
            CB_NODE* node = it->second;
            while( node->CB_Next != NULL )
            {
                node = node->CB_Next;
            }

            PrintToStdout( LOGDEBUG, "attach preNode[obj:%s,func:%s] point to cur!",
                   node->CB_Obj,node->CB_Func );

            CB_NODE* tCB_Node = (CB_NODE*) calloc( 1, sizeof( CB_NODE ) );
            strcpy(tCB_Node->CB_Obj,obj);
            strcpy(tCB_Node->CB_Func,func);
            tCB_Node->CB_Next = NULL;

            node->CB_Next     = tCB_Node;

            return 1;
        }
    }

    CB_NODE* tCB_Node = (CB_NODE*) calloc( 1,sizeof( CB_NODE ) );
    strcpy(tCB_Node->CB_Obj,obj);
    strcpy(tCB_Node->CB_Func,func);
    tCB_Node->CB_Next = NULL;

    PrintToStdout( LOGDEBUG, "attach head node[obj:%s,func:%s] to build!",
           tCB_Node->CB_Obj,tCB_Node->CB_Func );

    mCB.insert( make_pair( tEvent, tCB_Node ) );

    return 1;
}

unsigned char detachEventHandler(
        const char* event,
        const char* obj )
{
    std::string tEvent( event );
    std::map<std::string, CB_NODE*>::iterator it;

    PrintToStdout( LOGDEBUG, "detach event=[%s]",tEvent.c_str() );

    for( it = mCB.begin(); it != mCB.end(); )
    {
        if( it->first == tEvent )
        {
            CB_NODE* node    = it->second;
            CB_NODE* preNode = it->second;
            while( node != NULL )
            {
                PrintToStdout( LOGDEBUG, "detach in detach while begine" );
                if( strcmp(node->CB_Obj,obj) == 0 )
                {
                    char vHeadFlag = 0;
                    if ( node == it->second )
                    {
                        PrintToStdout( LOGDEBUG, "detach in detach define new [head]" );
                        it->second = node->CB_Next;
                        preNode    = node->CB_Next;
                        vHeadFlag  = 1;
                    }
                    else
                    {
                        PrintToStdout( LOGDEBUG, "detach preNode[obj:%s,func:%s] point to new next!",
                               preNode->CB_Obj,preNode->CB_Func );
                        preNode->CB_Next = node->CB_Next;
                    }

                    PrintToStdout( LOGDEBUG, "detach free node[obj:%s,func:%s] !",
                           node->CB_Obj,node->CB_Func );

                    free( node );
                    if ( vHeadFlag == 1 )
                    {
                        node = preNode;
                    }
                    else if ( preNode != NULL )
                    {
                        node = preNode->CB_Next;
                    }
                    else
                    {
                        node = NULL;
                    }
                    
                }
                else
                {
                    PrintToStdout( LOGDEBUG, "detach not cur node[%s,%s],to find next node",
                           node->CB_Obj,node->CB_Func );
                    preNode = node;
                    node    = node->CB_Next;
                }
            } //end  while( node != NULL && node->CB_Next != NULL )

            if( it->second == NULL )
            {
                PrintToStdout( LOGDEBUG, "detach erase map[%s]",tEvent.c_str() );
                mCB.erase( it++ );
            }
            else
            {
                PrintToStdout( LOGDEBUG, "detach not shout in here!!!");
                it++;
            }
        } //end if( it->first == tEvent )
        else
        {
            PrintToStdout( LOGDEBUG, "detach [%s] not eq [%s],to find it++", 
                    it->first.c_str(),tEvent.c_str() );
            it++;
        }
    }

    PrintToStdout( LOGDEBUG, "detach return" );

    return 1;
}

int main()
{
    /////home/fusky/mygit/cTest
    //WriteLogInit( 
    //    LOGDEBUG,
    //    "/home/fusky/mygit/cTest/log",      /*不设置时默认日志路径为程序运行路径下LOG/$YYYY/$MM*/
    //    "tt" /*当设置dirName_p时才设置此值：日志名为logPrefixName_$YYYYMMDD.log*/
    //    );

    //char tmpStr[1024] = {0};
    //int  sizeofStr = sizeof(tmpStr);
    //PrintToStdout( LOGDEBUG, "sizeof(tmpStr[1024])=[%d]", sizeofStr );

    //std::map< std::string, MY_FL_NODE* > myFMap;

    attachEventHandler ( "event1","obj0","func0");
    attachEventHandler ( "event1","obj1","func1");
    attachEventHandler ( "event1","obj2","func2");
    attachEventHandler ( "event1","obj3","func3");
    attachEventHandler ( "event1","obj4","func4");

    attachEventHandler ( "event2","obj0","func0");
    attachEventHandler ( "event2","obj1","func1");
    attachEventHandler ( "event2","obj2","func2");

    std::map<std::string, CB_NODE*>::iterator it;
    for( it = mCB.begin(); it != mCB.end(); it++ )
    {
        PrintToStdout( LOGDEBUG, "main:event------>[%s]", it->first.c_str() );
        CB_NODE* tnode = it->second;
        int i=1;
        while ( tnode != NULL )
        {
            PrintToStdout( LOGDEBUG, "main:   [%d] obj:[%s],func:[%s] !",
                   i++,tnode->CB_Obj,tnode->CB_Func );
            tnode = tnode->CB_Next;
        }
        PrintToStdout( LOGDEBUG, "main:-------------------------" );
    }

    PrintToStdout( LOGDEBUG, "main:+++++++++++++++++++++++++" );
    detachEventHandler( "event2", "obj2" );
    
    for( it = mCB.begin(); it != mCB.end(); it++ )
    {
        PrintToStdout( LOGDEBUG, "main:event------>[%s]", it->first.c_str() );
        CB_NODE* tnode = it->second;
        int i=1;
        while ( tnode != NULL )
        {
            PrintToStdout( LOGDEBUG, "main:   [%d] obj:[%s],func:[%s] !",
                   i++,tnode->CB_Obj,tnode->CB_Func );
            tnode = tnode->CB_Next;
        }
        PrintToStdout( LOGDEBUG, "main:-------------------------" );
    }

    return 0;
}
