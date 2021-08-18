#include <stdio.h>
#include <string.h>
#include "log4cplusDiy.h"


//using namespace std;
//using namespace log4cplus;
//using namespace log4cplus::helpers;


//log4cplus::Logger log_0 =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("test"));
//log4cplus::Logger log_2 =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("test.log_2"));
//log4cplus::Logger log_3 =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("test.log_3"));


void printMsgs(log4cplus::Logger& logger)
{
    LOG4CPLUS_TRACE_METHOD(logger, LOG4CPLUS_TEXT("printMsgs()"));

    //level: 
    //     TRACE_LOG_LEVEL 
    //     DEBUG_LOG_LEVEL 
    //     INFO_LOG_LEVEL 
    //     WARN_LOG_LEVEL 
    //     ERROR_LOG_LEVEL 
    //     FATAL_LOG_LEVEL
    //
    //WRITE_LOG(logger,TRACE_LOG_LEVEL,"printMsgs()");
    WRITE_LOG(logger,_DEBUG,"printMsgs()");
    WRITE_LOG(logger,_INFO,"printMsgs()");
    WRITE_LOG(logger,_WARN,"printMsgs()");
    WRITE_LOG(logger,_ERROR,"printMsgs()");
    WRITE_LOG(logger,_FATAL,"printMsgs()");

    //LOG4CPLUS_DEBUG(logger, "printMsgs()");
    //LOG4CPLUS_INFO(logger, "printMsgs()");
    //LOG4CPLUS_WARN(logger, "printMsgs()");
    //LOG4CPLUS_ERROR(logger, "printMsgs()");
    ////LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("version")), "vvv printMsgs()");

    return;
}
    
log4cplus::tstring getPropertiesFileArgument (int argc, char * argv[])
{
    if (argc >= 2)
    {
        char const * arg = argv[1];
        log4cplus::tstring file = LOG4CPLUS_C_STR_TO_TSTRING (arg);
        log4cplus::helpers::FileInfo fi;
        if (log4cplus::helpers::getFileInfo (&fi, file) == 0)
        {
            return file;
        }
        else
        {
            log4cplus::tcout<<"open ["<<arg<<"] error:"<<errno<<" errMsg["<<strerror(errno)<<"]"<<std::endl; 
            log4cplus::tcout<<"now use file:"<<"cfg/log4cplus.properties"<<std::endl;
            return LOG4CPLUS_TEXT ("cfg/log4cplus.properties");

        }
    }

    log4cplus::tcout<<"use file:"<<"cfg/log4cplus.properties"<<std::endl;
    return LOG4CPLUS_TEXT ("cfg/log4cplus.properties");
}


int main(int argc, char * argv[])
{
    log4cplus::tcout << LOG4CPLUS_TEXT("Entering main()...") << std::endl;
    log4cplus::initialize ();
    log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);

    log4cplus::Logger log_version =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("version"));
    log4cplus::Logger log_scada =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("scada"));
    log4cplus::Logger log_apdu =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("apdu"));
    log4cplus::Logger log_user =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("user"));
    log4cplus::Logger log_dtv =  log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("dtv"));

    int tttmpv = 1;

    log4cplus::Logger &lgr = (tttmpv == 1 ? log_version : log_dtv);
    //log4cplus::Logger root = log4cplus::Logger::getRoot();
    try
    {
        log4cplus::ConfigureAndWatchThread configureThread(
            getPropertiesFileArgument (argc, argv), 1 * 1000);

        //LOG4CPLUS_WARN(root, "Testing....");

        WRITE_LOG(lgr,_INFO,"22222");
        //WRITE_LOG_BYNAME("version",INFO_LOG_LEVEL,"22112222[%s]","haha");

        for(int i=0; i<4; ++i) {
            //printMsgs(log_0);
            //log4cplus::Logger v = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("version"));
            printMsgs(log_version);
            printMsgs(log_scada);
            printMsgs(log_apdu);
            printMsgs(log_user);
            printMsgs(log_dtv);
            log4cplus::helpers::sleep(1);
        }
    }
    catch(...) {
        log4cplus::tcout << LOG4CPLUS_TEXT("Exception...") << std::endl;
        //LOG4CPLUS_FATAL(root, "Exception occured...");
    }

    log4cplus::tcout << LOG4CPLUS_TEXT("Exiting main()...") << std::endl;
    return 0;
}

