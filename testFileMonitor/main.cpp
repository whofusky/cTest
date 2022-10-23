#include <stdio.h>

#include "IEventManager.h"
#include "FileMonitor.h"
#include "TestFileMonitor.h"

int main()
{
    //启动事件管理线程
    IEventManager::Initialize()->start();

    //响应文件监视器的处理线程
    TestFileMonitor::Instance()->start();

    //需要监视的路径1
    char monitorDir_1[]="/home/fusky/tmp/t";

    //定义对监视目录中出现的文件,需要监视的动作
    //  即: 如果监视目录中的文件有如下动作时则文件监视器就给响应进程发消息
    unsigned int  event = 0;
        event |= EVENT_CREATE;
        event |= EVENT_DELETE;
        event |= EVENT_MOVE_TO;
        event |= EVENT_MOVE_FROM;
        event |= EVENT_MODIFY;
        event |= EVENT_STOCK;

    //定义需要监视文件的 文件 或 前缀 和 后缀 (如果有确切的文件名则只需要配置
    // 文件即可 无需再配置 前缀和后缀
    std::vector<std::string> monFiles;
    std::vector<std::string> monFilePreFix;
    std::vector<std::string> monFileSuffix;

    //需要监视的确切文件名
    monFiles.push_back( "t.sh" );

    //文件前缀( 可添加多个 )
    monFilePreFix.push_back( "busi" );
    monFilePreFix.push_back( "mytest" );

    //文件后缀( 可添加多个 )
    monFileSuffix.push_back( ".xml" );
    monFileSuffix.push_back( ".csv" );

    //在监视器线程中注册 需要监视的路径1
    FileMonitor::Instance()->startMonitor( 
            monitorDir_1,
            event,
            monFiles,
            monFilePreFix,
            monFileSuffix
            );

    //需要监视的路径2
    char monitorDir_2[]="/home/fusky/tmp/k";

    //在监视器线程中注册 需要监视的路径2
    FileMonitor::Instance()->startMonitor( 
            monitorDir_2,
            event,
            monFiles,
            monFilePreFix,
            monFileSuffix
            );


    while(1)
    {
        PauseThreadSleep( 100, 0 );
    }

    return 0;
}
