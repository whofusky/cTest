/***********************************************************
 *
 * @file    FileReceiver.h
 *
 * @brief   FileReceiver头文件
 *
 * @author  fu.sky
 *
 * @date    2022-10-31_17:45:34
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _FILERECEIVER_H_
#define _FILERECEIVER_H_

#include "IEventManager.h"
#include "json/value.h"

class FileReceiver: public CThread, public IEventManager::IEventHandler
{

private:
    FileReceiver();
    ~FileReceiver();

public:
    static int isRun;
    static FileReceiver* Instance();
private:
    static FileReceiver* mInstance;

public:
    void onAppEvent( 
            const char* event,
            IEventManager::EVENT_ACTION action,
            Json::Value& eventAttr 
            );

    void start( const int &pre_val = -1 );
    void stop();

private:
    class FileRcvHandler: public Handler
    {
        public:
            FileRcvHandler( FileReceiver* testFileMo );
            ~FileRcvHandler();
            typedef enum file_test_handler_msg
            {
                MSG_STOCK_FILE,
                MSG_CREAT_FILE,
                MSG_MODIF_FILE,
                MSG_DELET_FILE,
                MSG_MOVTO_FILE,
                MSG_MOVFM_FILE,
                MSG_CLOSER_FILE,
                MSG_NULL
            }FILE_TST_HANDLER_MSG;

            void handleMessage( Message& msg );
        private:
            FileReceiver* mRcver;
    };

    Handler*    mHandler;
    Looper*     mLooper;

private:
    void threadHandler();

private:
    class Delector
    {
        public:
            ~Delector()
            {
                if ( FileReceiver::mInstance != NULL )
                {
                    delete FileReceiver::mInstance;
                }
            }
    };

    static Delector delector;

};



#endif//_FILERECEIVER_H_
