#include <stdio.h>

#include "IEventManager.h"
#include "FileMonitor.h"
#include "TestFileMonitor.h"

int main()
{
    //启动事件管理线程
    IEventManager::Initialize()->start();

    //响应文件监视器的处理线程
    TestFileMonitor::Instance()->start( IEventManager::isRun );

    //启动文件监视器线程
    FileMonitor::Instance()->start( TestFileMonitor::isRun );

    //定义对监视目录中出现的文件,需要监视的动作
    //  即: 如果监视目录中的文件有如下动作时则文件监视器就给响应进程发消息
    unsigned int  event = 0;
        //event |= EVENT_CREATE;
        event |= EVENT_DELETE;
        event |= EVENT_MOVE_TO;
        event |= EVENT_MOVE_FROM;
        //event |= EVENT_MODIFY;
        //event |= EVENT_STOCK;
        event |= EVENT_CLOSEWRITE;

    //添加对某个目录的监视
    //  其中 文件名、文件前缀、文件后缀 是否选项
    FileMonitor::Instance()->addWatch( 
            "/home/fusky/tmp/t",  /*监控的路径*/
            event,                /*关注事件的类型*/
            "t.sh",               /*关注的文件名(多个用|分隔)*/
            "busi|mytest",        /*关注的文件名前缀(多个用|分隔) */
            ".xml|.csv"           /*关注的文件名后缀(多个用|分隔)*/
            );

    //对/home/fusky/tmp/k目录的k.ini和t.sh文件进行监视
    FileMonitor::Instance()->addWatch( 
            "/home/fusky/tmp/k",
            (EVENT_MOVE_TO|EVENT_CLOSEWRITE),
            "k.ini|t.sh"
            );


    while(1)
    {
        PauseThreadSleep( 100, 0 );
    }

    return 0;
}
