
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
#pragma message("��ǰ��Windowsϵͳ:x32")
#endif

#ifdef WIN64
#define WINDOWS_PLATFORM
#pragma message("��ǰ��Windowsϵͳ:x64")
#endif

#ifdef WINDOWS_PLATFORM //��WINDOWSƽ̨

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

#else   //�Ƿ�WINDOWSƽ̨

    #pragma message("��ǰ����Unixϵͳ")
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

//FUNC  �õ�������
static int LastError()
{
#ifdef WINDOWS_PLATFORM
    return GetLastError();
#else
    return errno;
#endif
}

//FUNC  Sleep���뼶
//PARAM second��Sleep������
static void SleepSecond ( size_t second )
{
#ifdef WINDOWS_PLATFORM
    Sleep ( second * 1000 );
#else
    nanoSleep ( second, 0 );
#endif
}
// 
// FUNC  Sleep�����뼶
// PARAM millisecond��Sleep�ĺ�����
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

//FUNC  Sleep��΢�뼶
//PARAM microsecond��Sleep��΢����
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

    //����ϣ��������Ŀ¼
    char szFilename[FILENAME_MAX+4] = "";
    memcpy(szFilename, pDir, DirLen);
    szFilename[DirLen] = '\0';
    szFilename[DirLen+1] = '\0';
	
    //�����е�'\'תΪ'/'
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

    //�𼶴���Ŀ¼
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

//FUNC  �õ��ļ���С
//PARAM pFilename���ļ���
//      u64FileSize���ļ���С
//RET   0���ļ���С��ȡ�ɹ�����ֵ�洢��u64FileSize��
//      �������ļ���С��ȡʧ��
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


//FUNC  �õ����ڴ�
//PARAM pStr���洢���ڴ���Buf
//      tTime��ʱ��ֵ�������ݴ�ֵ�������ڡ���Ϊ0�����ʾ���ݵ�ǰʱ��������ڡ�
//NOTE  ��ʽ��yyyy/mm/dd
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


//FUNC  �õ�ʱ�䴮
//PARAM pStr���洢ʱ�䴮��Buf
//      tTime��ʱ��ֵ�������ݴ�ֵ����ʱ�䡣��Ϊ0�����ʾ���ݵ�ǰʱ�����ʱ�䡣
//NOTE  ��ʽ��hh:mm:ss
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

//FUNC  �õ�ʱ�䴮���ļ���
//PARAM pStr���洢ʱ�䴮��Buf
//      tTime��ʱ��ֵ�������ݴ�ֵ����ʱ�䡣��Ϊ0�����ʾ���ݵ�ǰʱ�����ʱ�䡣
//NOTE  ��ʽ��hh:mm:ss
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

//FUNC  �õ�����ʱ�䴮
//PARAM pStr���洢����ʱ�䴮��Buf
//      tTime��ʱ��ֵ�������ݴ�ֵ��������ʱ�䡣��Ϊ0�����ʾ���ݵ�ǰʱ���������ʱ�䡣
//NOTE  ��ʽ��yyyy/mm/dd_hh:mm:ss
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


//FUNC  �ж��ļ���Ŀ¼�Ƿ����
//PARAM pFile���ļ���Ŀ¼��
//RET   true���ļ���Ŀ¼����
//      false���ļ���Ŀ¼������
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

//FUNC  ���Դ��ļ�
//PARAM fp���ļ�ָ��
//      pFile���ļ���
//      pMode�����ļ���ģʽ
//      iMaxTryCount������Դ���������ֵΪ����ʱ�����Ч��
//RET   0���򿪳ɹ�����ʱfpΪ��Ӧ���ļ�ָ��
//      ��������ʧ��
static int TryOpenFile(FILE *&fp, const char *pFile, const char *pMode, int iMaxTryCount = -1)
{
    if (pFile == NULL || pMode == NULL)
    {
        return -1;
    }
    
    if (strchr(pMode, 'r') != NULL
        && !FileIsExisting(pFile))
    {//����ʽ��һ�������ڵ��ļ�
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
        {//�ѵ�����Դ���
            return -100;
        }
    }
}

//FUNC  ���ļ�
//PARAM fp���ļ�ָ��
//      pFile���ļ���
//      pMode�����ļ���ģʽ
//RET   0���򿪳ɹ�����ʱfpΪ��Ӧ���ļ�ָ��
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

//FUNC  ����ɾ���ļ�
//PARAM pFile��Դ�ļ���
//      iMaxTryCount������Դ���������ֵΪ����ʱ�����Ч��
//RET   0��ɾ���ɹ�
//      ������ɾ��ʧ��
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

//FUNC  ɾ���ļ�
//PARAM pFile��Դ�ļ���
//RET   0��ɾ���ɹ�
//      ������ɾ��ʧ��
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

//FUNC  �����������ļ�
//PARAM pSrcFile��Դ�ļ���
//      pDestFile��Ŀ���ļ���
//      iMaxTryCount������Դ���������ֵΪ����ʱ�����Ч��
//RET   0���������ɹ�
//      ������������ʧ��
static int TryRenameFile(const char *pSrcFile, const char *pDestFile, int iMaxTryCount = -1)
{
    if (pSrcFile == NULL || pDestFile == NULL)
    {
        return -1;
    }

    if (!FileIsExisting(pSrcFile))
    {//Դ������
        return -10;
    }

    if (FileIsExisting(pDestFile))
    {//Ŀ���Ѵ���
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
        {//MFS�У�rename���ܷ���ʧ�ܣ����Ѿ��ɹ�����
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

//FUNC  �������ļ�
//PARAM pSrcFile��Դ�ļ���
//      pDestFile��Ŀ���ļ���
//RET   0���������ɹ�
//      ������������ʧ��
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
        {//Ŀ���Ѵ���
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
