#ifndef DATALOG_H
#define DATALOG_H



enum
{
    FILELOGINFO,
    FILELOGERROR,
    FILELOGFAIL,
    FILELOGWARN,
    FILELOGEXCEPTION,
    FILELOGNOTE
};





//void _DataWriteLog(const char * FILE, int LINE, int logLevel, const char* strMsg);
void _DataWriteLog(const char * FILE, int LINE, int logLevel, const char* strMsg);
#define DataWriteLog(logLevel, strMsg)	\
        _DataWriteLog(__FILE__, __LINE__, logLevel, strMsg);



#endif // LOG_H
