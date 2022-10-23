/***********************************************************
 *
 * @file    TestFileMonitor.h
 *
 * @brief   TestFileMonitor头文件
 *
 * @author  fu.sky
 *
 * @date    2022-10-09_15:35:46
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _TESTFILEMONITOR_H_
#define _TESTFILEMONITOR_H_

#include "IEventManager.h"
#include "json/value.h"

class TestFileMonitor: public CThread, public IEventManager::IEventHandler
{

private:
    TestFileMonitor();
    ~TestFileMonitor();

public:
    static TestFileMonitor* Instance();
private:
    static TestFileMonitor* mInstance;

public:
    void onAppEvent( 
            const char* event,
            IEventManager::EVENT_ACTION action,
            Json::Value& eventAttr 
            );

    void start();
    void stop();

private:
    class TestFileHandler: public Handler
    {
        public:
            TestFileHandler( TestFileMonitor* testFileMo );
            ~TestFileHandler();
            typedef enum file_test_handler_msg
            {
                MSG_STOCK_FILE,
                MSG_CREAT_FILE,
                MSG_MODIF_FILE,
                MSG_DELET_FILE,
                MSG_MOVTO_FILE,
                MSG_MOVFM_FILE,
                MSG_NULL
            }FILE_TST_HANDLER_MSG;

            void handleMessage( Message& msg );
        private:
            TestFileMonitor* mFileTestMonitor;
    };

    Handler*    mHandler;
    Looper*     mLooper;

private:
    void threadHandler();

};



#endif//_TESTFILEMONITOR_H_
