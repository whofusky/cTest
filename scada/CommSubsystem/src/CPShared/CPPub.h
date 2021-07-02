
#ifndef CP_PUB_H
#define CP_PUB_H

#include "CPInt.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

const int INVALID_ID = 0;
#define USE_MFS 0    //rename will check twice on MFS

#ifdef WIN32
#define WINDOWS_PLATFORM
#pragma message("当前是Windows系统:x32")
#endif

#ifdef WIN64
#define WINDOWS_PLATFORM
#pragma message("当前是Windows系统:x64")
#endif

#ifdef WINDOWS_PLATFORM //是WINDOWS平台

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #define snprintf _snprintf
    #define access _access
    #define I64d "%I64d"
    #define I64u "%I64u"
    #define I64x "%I64x"

#ifdef _USE_32BIT_TIME_T
    #define PTTF "%d"   //PTF = print time_t format
#else
    #define PTTF "%I64d"
#endif

#else   //是非WINDOWS平台

    #pragma message("当前是类Unix系统")
    #include <unistd.h>
    #include <dirent.h>
    #include <errno.h>
    #include <pthread.h>
    #include <sys/time.h>
    #include <time.h>
    #define I64d "%lld"
    #define I64u "%llu"
    #define I64x "%llx"

    #define PTTF "%lld"

    static int nanoSleep ( const time_t& tv_sec,
                           const long&   tv_nsec
                         )
    {
        struct timespec req;
        req.tv_sec = tv_sec;
        req.tv_nsec = tv_nsec;

        return ( nanosleep ( &req, NULL ) );
    }

#endif


namespace nsCPPub
{

//FUNC  得到错误码
static int LastError()
{
#ifdef WINDOWS_PLATFORM
    return GetLastError();
#else
    return errno;
#endif
}

//FUNC  Sleep，秒级
//PARAM second：Sleep的秒数
static void SleepSecond ( size_t second )
{
#ifdef WINDOWS_PLATFORM
    Sleep ( second * 1000 );
#else
    nanoSleep ( second, 0 );
#endif
}
// 
// FUNC  Sleep，毫秒级
// PARAM millisecond：Sleep的毫秒数
static void SleepMilliSecond ( size_t millisecond )
{
#ifdef WINDOWS_PLATFORM
    Sleep ( millisecond );
#else
    time_t tv_sec = 0;
    long   tv_nsec = 0;

    if ( millisecond > 1000 )
    {
        tv_sec = millisecond / 1000;
        tv_nsec = ( millisecond % 1000 ) * 1000000;
    }
    else
    {
        tv_nsec = millisecond * 100000;
    }

    nanoSleep ( tv_sec, tv_nsec );
#endif
}

//FUNC  Sleep，微秒级
//PARAM microsecond：Sleep的微秒数
static void SleepMicroSecond(size_t microsecond)
{
#ifdef WINDOWS_PLATFORM
    LARGE_INTEGER m_liPerfFreq = {0};
    if (!QueryPerformanceFrequency(&m_liPerfFreq))
    {
        return;
    }
    LARGE_INTEGER m_liPerfStart = {0};
    QueryPerformanceCounter(&m_liPerfStart);
    LARGE_INTEGER liPerfNow={0};
    while (true)
    {
        QueryPerformanceCounter(&liPerfNow);
            double time = ( ( liPerfNow.QuadPart - m_liPerfStart.QuadPart
                            ) * 1000000 ) / ( double ) m_liPerfFreq.QuadPart;
        if (time >= microsecond)
        {
            break;
        }
    }
#else
    //usleep(microsecond);
    time_t tv_sec = 0;
    long   tv_nsec = 0;

    if ( microsecond > 1000000 )
    {
        tv_sec = microsecond / 1000000;
        tv_nsec = ( microsecond % 1000000 ) * 1000;
    }
    else
    {
        tv_nsec = microsecond * 1000;
    }

    nanoSleep ( tv_sec, tv_nsec );
#endif
}

static int MakeDir(const char *pDir)
{
    if (pDir == NULL)
    {
        return -10;
    }
    size_t DirLen = strlen(pDir);
    if (DirLen <= 0 || DirLen >= FILENAME_MAX)
    {
        return -20;
    }    

    //复制希望创建的目录
    char szFilename[FILENAME_MAX+4] = "";
    memcpy(szFilename, pDir, DirLen);
    szFilename[DirLen] = '\0';
    szFilename[DirLen+1] = '\0';
	
    //将所有的'\'转为'/'
    char *pCurrDir = szFilename;
    while (*pCurrDir != '\0')
    {
        if (*pCurrDir < 0)
        {
            pCurrDir += 2;
        }
        else
        {
            if (*pCurrDir == '\\')
            {
                *pCurrDir = '/';
            }
            ++pCurrDir;
        }
    }

    //逐级创建目录
    pCurrDir = szFilename;
    while (true)
    {
        pCurrDir = strchr(pCurrDir+1, '/');
        if (pCurrDir == NULL)
        {
#ifdef WINDOWS_PLATFORM
            _mkdir(szFilename);
#else
            mkdir(szFilename, S_IRWXO|S_IRWXU);
#endif
            return 0;
        }
        *pCurrDir = '\0';
#ifdef WINDOWS_PLATFORM
        _mkdir(szFilename);
#else
        mkdir(szFilename, S_IRWXO|S_IRWXU);
#endif
        *pCurrDir = '/';        
    }
    return 0;
}

//FUNC  得到文件大小
//PARAM pFilename：文件名
//      u64FileSize：文件大小
//RET   0：文件大小获取成功，其值存储在u64FileSize中
//      其他：文件大小获取失败
static int GetFileSize(const char *pFilename, uint64_t &u64FileSize)
{
    if (pFilename == NULL)
    {
        return -1;
    } 
    struct stat sFileInfo;
    int iRet = stat(pFilename, &sFileInfo);
    if (iRet != -1)
    {
        u64FileSize = sFileInfo.st_size;
        return 0;
    }
    else
    {
        return -10;
    }
}


//FUNC  得到日期串
//PARAM pStr：存储日期串的Buf
//      tTime：时间值，将根据此值计算日期。若为0，则表示根据当前时间计算日期。
//NOTE  格式：yyyy/mm/dd
static void GetDateStr(char *pStr, time_t tTime = 0)
{
    if (pStr == NULL)
    {
        return;
    }
    if (tTime == 0)
    {
        tTime = time(NULL);
    }

#ifdef WINDOWS_PLATFORM
    struct tm *pTime = localtime(&tTime);
#else
    struct tm stm4Time;
    struct tm *pTime = localtime_r(&tTime, &stm4Time);
#endif    
    sprintf(pStr, "%04d/%02d/%02d",
        pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday);
}


//FUNC  得到时间串
//PARAM pStr：存储时间串的Buf
//      tTime：时间值，将根据此值计算时间。若为0，则表示根据当前时间计算时间。
//NOTE  格式：hh:mm:ss
static void GetTimeStr(char *pStr, time_t tTime = 0)
{
    if (pStr == NULL)
    {
        return;
    }	
    if (tTime == 0)
    {
        tTime = time(NULL);
    }

#ifdef WINDOWS_PLATFORM
    struct tm *pTime = localtime(&tTime);
#else
    struct tm stm4Time;
    struct tm *pTime = localtime_r(&tTime, &stm4Time);
#endif
    sprintf(pStr, "%02d:%02d:%02d",
        pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
}

//FUNC  得到时间串做文件名
//PARAM pStr：存储时间串的Buf
//      tTime：时间值，将根据此值计算时间。若为0，则表示根据当前时间计算时间。
//NOTE  格式：hh:mm:ss
static void GetTimeStrForFilename(char *pStr, time_t tTime = 0)
{
    if (pStr == NULL)
    {
        return;
    }	
    if (tTime == 0)
    {
        tTime = time(NULL);
    }

#ifdef WINDOWS_PLATFORM
    struct tm *pTime = localtime(&tTime);
#else
    struct tm stm4Time;
    struct tm *pTime = localtime_r(&tTime, &stm4Time);
#endif
    sprintf(pStr, "%02d%02d%02d",
        pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
}

//FUNC  得到日期时间串
//PARAM pStr：存储日期时间串的Buf
//      tTime：时间值，将根据此值计算日期时间。若为0，则表示根据当前时间计算日期时间。
//NOTE  格式：yyyy/mm/dd_hh:mm:ss
static void GetDateTimeStr(char *pStr, time_t tTime = 0)
{
    if (pStr == NULL)
    {
        return;
    }	
    if (tTime == 0)
    {
        tTime = time(NULL);
    }

    GetDateStr(pStr, tTime);
    pStr[10] = '_';
    GetTimeStr(pStr+11, tTime);
}


//FUNC  GetTickCount for Linux
static uint32_t GetTickCnt()
{
#ifdef WINDOWS_PLATFORM
    return GetTickCount();
#else
    struct timeval sTimeVal;
    if(gettimeofday(&sTimeVal, NULL) != 0)
    {
        return 0;
    }
    return sTimeVal.tv_sec*1000+sTimeVal.tv_usec/1000;
#endif
}


//FUNC  判断文件或目录是否存在
//PARAM pFile：文件或目录名
//RET   true：文件或目录存在
//      false：文件或目录不存在
static bool FileIsExisting(const char *pFile)
{
    if (pFile == NULL)
    {
        return false;
    }

    if (access(pFile, 0) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FUNC  尝试打开文件
//PARAM fp：文件指针
//      pFile：文件名
//      pMode：打开文件的模式
//      iMaxTryCount：最大尝试次数。仅当值为正的时候才有效。
//RET   0：打开成功，此时fp为相应的文件指针
//      其他：打开失败
static int TryOpenFile(FILE *&fp, const char *pFile, const char *pMode, int iMaxTryCount = -1)
{
    if (pFile == NULL || pMode == NULL)
    {
        return -1;
    }
    
    if (strchr(pMode, 'r') != NULL
        && !FileIsExisting(pFile))
    {//读方式打开一个不存在的文件
        return -10;
    }

    int iAlreadyTry = 0;
    while (true)
    {
        fp = fopen(pFile, pMode);
        if (fp != NULL)
        {
            return 0;
        }
        SleepMilliSecond(1);
        if (iMaxTryCount > 0 && ++iAlreadyTry >= iMaxTryCount)
        {//已到最大尝试次数
            return -100;
        }
    }
}

//FUNC  打开文件
//PARAM fp：文件指针
//      pFile：文件名
//      pMode：打开文件的模式
//RET   0：打开成功，此时fp为相应的文件指针
static int MustOpenFile(FILE *&fp, const char *pFile, const char *pMode)
{
    while (true)
    {
        int iRet = TryOpenFile(fp, pFile, pMode, 10000);
        if (iRet == 0)
        {
            return 0;
        }
        else
        {
            printf("MustOpenFile Failed: %s, %s, code=%d, LastError=%d\n",
                pFile, pMode, iRet, LastError());
        }
    }
}

//FUNC  尝试删除文件
//PARAM pFile：源文件名
//      iMaxTryCount：最大尝试次数。仅当值为正的时候才有效。
//RET   0：删除成功
//      其他：删除失败
static int TryRemoveFile(const char *pFile, int iMaxTryCount = -1)
{
    if (pFile == NULL)
    {
        return -1;
    }

    int iAlreadyTry = 0;
    while (true)
    {
        if (!FileIsExisting(pFile))
        {
            return 0;
        }

        if (remove(pFile) == 0)
        {
            return 0;
        }

        SleepMilliSecond(1);
        if (iMaxTryCount > 0 && ++iAlreadyTry >= iMaxTryCount)
        {
            return -100;
        }
    }
}

//FUNC  删除文件
//PARAM pFile：源文件名
//RET   0：删除成功
//      其他：删除失败
static int MustRemoveFile(const char *pFile)
{
    while (true)
    {
        int iRet = TryRemoveFile(pFile, 10000);
        if (iRet == 0)
        {
            return 0;
        }
        else
        {
            printf("MustRemoveFile Failed: %s, code=%d, LastError=%d\n",
                pFile, iRet, LastError());
        }
    }
}

//FUNC  尝试重命名文件
//PARAM pSrcFile：源文件名
//      pDestFile：目标文件名
//      iMaxTryCount：最大尝试次数。仅当值为正的时候才有效。
//RET   0：重命名成功
//      其他：重命名失败
static int TryRenameFile(const char *pSrcFile, const char *pDestFile, int iMaxTryCount = -1)
{
    if (pSrcFile == NULL || pDestFile == NULL)
    {
        return -1;
    }

    if (!FileIsExisting(pSrcFile))
    {//源不存在
        return -10;
    }

    if (FileIsExisting(pDestFile))
    {//目标已存在
        return -20;
    }

    int iAlreadyTry = 0;
    while (true)
    {
        if (rename(pSrcFile, pDestFile) == 0)
        {
            return 0;
        }
        SleepMilliSecond(1);
#if USE_MFS
        if (!FileIsExisting(pSrcFile) && FileIsExisting(pDestFile))
        {//MFS中，rename可能返回失败，但已经成功复制
            printf("rename() OK while it returns fault.\n");
            return 0;
        }
#endif
        if (iMaxTryCount > 0 && ++iAlreadyTry >= iMaxTryCount)
        {
            return -100;
        }
    }
}

//FUNC  重命名文件
//PARAM pSrcFile：源文件名
//      pDestFile：目标文件名
//RET   0：重命名成功
//      其他：重命名失败
static int MustRenameFile(const char *pSrcFile, const char *pDestFile)
{
    while (true)
    {
        int iRet = TryRenameFile(pSrcFile, pDestFile, 10000);
        if (iRet == 0)
        {
            return 0;
        }
        else if (iRet == -20)
        {//目标已存在
            MustRemoveFile(pDestFile);
        }
        else
        {
            printf("MustRenameFile Failed: %s to %s, code=%d, LastError=%d\n",
                pSrcFile, pDestFile, iRet, LastError());
        }
    }
}


}   // end of namespace

#endif  // end of #ifndef
