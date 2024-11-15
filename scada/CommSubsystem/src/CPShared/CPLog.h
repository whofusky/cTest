
#ifndef CP_LOG_H
#define CP_LOG_H

#include "CPLock.h"
#include "CPPub.h"

#include <stdarg.h>
#include <assert.h>

//配置文件名(路径默认与日志输出路径是同一个目录)
#define MC_LOGDEBUG_RRQF "logDebugCfg"

namespace nsCPLog
{

    //每天更换文件的日志
    class CDailyLog
    {
        public:
            CDailyLog()
            {
                memset ( m_szLogDir, 0, sizeof ( m_szLogDir ) );
                memset ( m_szPrefix, 0, sizeof ( m_szPrefix ) );
                m_bNeedTimeStamp = true;
                m_FlushFreq      = 1;
                m_SavePeriod     = 30;
                m_fpA            = NULL;
                m_fpCfg          = NULL;
                m_SaveCount      = 0;
                m_tLastOpenDay   = 0;
                m_tLastOpenTime  = 0;
                m_preMinute      = 0;
                m_readFileFrq    = 0;
                m_debugMask      = 0xffffffff;
            }
            ~CDailyLog()
            {
                if ( m_fpA != NULL )
                {
                    fclose ( m_fpA );
                    m_fpA = NULL;
                }
                if ( m_fpCfg != NULL )
                {
                    fclose( m_fpCfg );
                    m_fpCfg = NULL;
                }
            }
            //FUNC  初始化
            //PARAM pLogDir：日志目录
            //      pPrefix：日志前缀
            //      bNeedTimeStamp：是否需要时间戳
            //      FlushFreq：刷新频率，若为3，表示每写三次就强制刷新文件
            //      SavePeriod：保存周期，若为7，表示只保存最近7日内的日志文件
            //      readFileFrq：读日志掩码配置文件的频度(单位分钟)
            //RET   0：初始化成功
            //      其他：初始化失败，返回错误码
            int Init ( const char* pLogDir,
                       const char* pPrefix,
                       bool bNeedTimeStamp = true,
                       size_t FlushFreq = 1,
                       size_t SavePeriod = 30,
                       unsigned char readFileFrq = 1
                     )
            {
                if ( !nsCPPub::FileIsExisting ( pLogDir ) )
                {
                    //日志目录不存在
            #ifdef WINDOWS_PLATFORM
                    if ( mkdir ( pLogDir ) == -1 )
            #else
                    if ( mkdir ( pLogDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) == -1 )
            #endif

                    {
                        return -10;
                    }
                    else
                    {
                        nsCPPub::FileIsExisting ( pLogDir );
                    }
                }

                strcpy ( m_szLogDir, pLogDir );
                strcpy ( m_szPrefix, pPrefix );
                m_bNeedTimeStamp = bNeedTimeStamp;
                m_FlushFreq      = FlushFreq;
                m_SavePeriod     = SavePeriod;
                m_readFileFrq    = readFileFrq;

                if ( m_readFileFrq > 0 )
                {
                    char mskCfgFname[FILENAME_MAX] = "";
                    snprintf ( mskCfgFname, sizeof ( mskCfgFname ),
                               "%s/%s",
                               m_szLogDir, MC_LOGDEBUG_RRQF );

                    if ( NULL == m_fpCfg
                         && nsCPPub::FileIsExisting ( mskCfgFname )
                       )
                    {
                        m_fpCfg = fopen ( mskCfgFname, "r" );
                    }

                }

                return 0;
            }

            //FUNC  记录日志的printf版本
            //PARAM bConsoleShow：是否显示在控制台上
            //      debugLevel  :控制日志是否输出的日志级别
            void LPrintf ( bool bConsoleShow, const unsigned int& debugLevel, const char* pFormat, ... )
            {
                char szFilename[FILENAME_MAX] = "";
                char szTimeStamp[64] = "";

                if ( m_bNeedTimeStamp )
                {
                    nsCPPub::GetDateTimeStr ( szTimeStamp );
                }

                va_list ap;



                time_t tNow = time ( NULL );
            #ifdef WINDOWS_PLATFORM
                struct tm* pNow = localtime ( &tNow );
            #else
                struct tm stm4Now;
                struct tm* pNow = localtime_r ( &tNow, &stm4Now );
            #endif

                //判断是否需要读日志输出掩码配置文件
                int curMinute = pNow->tm_min;

                if ( m_readFileFrq > 0
                     && m_fpCfg != NULL
                     && ( curMinute - m_preMinute ) >= m_readFileFrq
                   )
                {

                    m_rCfgLock.Lock();
                    if ( ( curMinute - m_preMinute ) >= m_readFileFrq )
                    {
                        rewind ( m_fpCfg );
                        fscanf ( m_fpCfg, "debugMask=[%u]", &m_debugMask );
                        m_preMinute = curMinute;
                    }
                    m_rCfgLock.UnLock();
                }

                //判断根据配置当前日志是否需要输出
                if ( ( m_debugMask & debugLevel ) != debugLevel )
                {
                    return;
                }

                nsCPLock::CAutolock autolock ( &m_lock );

                if ( NULL == m_fpA )
                {
                    snprintf ( szFilename, sizeof ( szFilename ),
                               "%s/%s_%04d%02d%02d.txt",
                               m_szLogDir, m_szPrefix,
                               pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday );

                    do
                    {
                        m_fpA = fopen ( szFilename, "a" );
                    } while ( NULL == m_fpA );

                    m_tLastOpenTime = time ( NULL );
                    m_tLastOpenDay  = pNow->tm_mday;
                }

                assert ( m_fpA != NULL );


                if ( m_bNeedTimeStamp )
                {
                    fprintf ( m_fpA, "[%s]", szTimeStamp );
                }

                fprintf ( m_fpA, "[level_%u]", debugLevel );

                va_start ( ap, pFormat );
                vfprintf ( m_fpA, pFormat, ap );
                va_end ( ap );

                ++m_SaveCount;
                TryFlushingData();

                if ( bConsoleShow == true )
                {
                    if ( m_bNeedTimeStamp )
                    {
                        printf ( "[%s]", szTimeStamp );
                    }

                    va_start ( ap, pFormat );
                    vprintf ( pFormat, ap );
                    va_end ( ap );
                }
            }

            //FUNC  强制刷新日志文件
            void FlushData()
            {
                if ( m_fpA != NULL )
                {
                    fflush ( m_fpA );
                }
            }

        protected:
            //FUNC  尝试刷新数据
            void TryFlushingData()
            {
                time_t tNow = time ( NULL );

                if ( m_tLastOpenTime + 10 * 60 < tNow || m_SaveCount >= m_FlushFreq )
                {
                    //打开文件超过十分钟 || 保存次数已经超过刷新周期

                    fflush ( m_fpA );   //刷新
                    m_SaveCount = 0;    //保存次数清零

                    //判断是否需要重新打开一个文件
#ifdef WINDOWS_PLATFORM
                    struct tm* pNow = localtime ( &tNow );
#else
                    struct tm stm4Now;
                    struct tm* pNow = localtime_r ( &tNow, &stm4Now );
#endif

                    if ( m_tLastOpenTime + 10 * 60 < tNow || pNow->tm_mday != m_tLastOpenDay )
                    {
                        //打开文件超过十分钟 || 已经跨天

                        fclose ( m_fpA );
                        m_fpA = NULL;

                        char szFilename[FILENAME_MAX] = "";
                        sprintf ( szFilename, "%s/%s_%04d%02d%02d.txt",
                                  m_szLogDir, m_szPrefix,
                                  pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday );

                        do
                        {
                            m_fpA = fopen ( szFilename, "a" );
                        } while ( m_fpA == NULL );

                        m_tLastOpenTime = tNow;
                        m_tLastOpenDay  = pNow->tm_mday;

                        CleanOldLog();  //每打开一个新文件，就清理一次旧日志
                    }

                    assert ( m_fpA != NULL );
                }
            }
            //FUNC  清理旧的日志文件
            void CleanOldLog()
            {
                char szFilename[FILENAME_MAX] = "";
                time_t tNow                   = time ( NULL );
                tNow -= m_SavePeriod * 86400;

                for ( size_t i = 0; i < m_SavePeriod; ++i, tNow -= 86400 )
                {
#ifdef WINDOWS_PLATFORM
                    struct tm* ptm = localtime ( &tNow );
#else
                    struct tm stm4Now;
                    struct tm* ptm = localtime_r ( &tNow, &stm4Now );
#endif
                    sprintf ( szFilename, "%s/%s_%04d%02d%02d.txt",
                              m_szLogDir, m_szPrefix,
                              ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday );
                    remove ( szFilename );
                }
            }
        private:
            char m_szLogDir[FILENAME_MAX];  //日志目录
            char m_szPrefix[FILENAME_MAX];  //日志前缀
            bool m_bNeedTimeStamp;          //是否需要时间戳
            size_t m_FlushFreq;             //刷新频率
            size_t m_SavePeriod;            //保存周期
            nsCPLock::CLock m_lock;         //写文件的锁
            FILE* m_fpA;                    //文件追加指针
            size_t m_SaveCount;             //保存次数
            time_t m_tLastOpenDay;          //上次打开文件的时间，以天为单位
            time_t m_tLastOpenTime;         //上次打开文件的时间，以秒为单位
            
            nsCPLock::CLock m_rCfgLock;     //读配置文件锁
            FILE* m_fpCfg;                  //文件追加指针
            unsigned char m_preMinute;      //记录上一次输出日志的分钟
            char m_readFileFrq;             //读配置文件(配置debugMask)的频度
            unsigned int  m_debugMask;      //用于控制日志是否输出的掩码
    };
    //
    // //尝试记录日志
    // class CTrySaveLog
    // {
    // public:
    //     CTrySaveLog()
    //     {
    //         memset(m_szLogDir, 0, sizeof(m_szLogDir));
    //         memset(m_szPrefix, 0, sizeof(m_szPrefix));
    //         m_bNeedTimeStamp = true;
    //         m_MaxLogSize = 100<<20;
    //     }
    //     //FUNC  初始化
    //     //PARAM pPrefix：日志前缀
    //     //      bNeedTimeStamp：是否需要时间戳
    //     //      MaxLogSize：日志文件的最大大小，若日志文件超过此大小，则会重命名当前文件，并使用一个新文件
    //     //RET   0：初始化成功
    //     //      其他：初始化失败
    //     int Init(const char *pPrefix, bool bNeedTimeStamp = true, size_t MaxLogSize = 100<<20)
    //     {
    //         if (pPrefix == NULL)
    //         {
    //             return -1;
    //         }
    //
    //         strcpy(m_szPrefix, pPrefix);
    //         m_bNeedTimeStamp = bNeedTimeStamp;
    //         m_MaxLogSize = m_MaxLogSize;
    //
    //         return 0;
    //     }
    //     int Init(const char *pLogDir, const char *pPrefix, bool bNeedTimeStamp = true, size_t MaxLogSize = 100<<20)
    //     {
    //         if (pLogDir == NULL || pPrefix == NULL)
    //         {
    //             return -1;
    //         }
    //
    //         strcpy(m_szLogDir, pLogDir);
    //         return Init(pPrefix, bNeedTimeStamp, MaxLogSize);
    //     }
    //     void TryLPrintfKernel(const char *pDir, bool bConsoleShow, const char *pFormat, va_list ap)
    //     {
    //         if (pDir == NULL || !nsCPPub::FileIsExisting(pDir))
    //         {//目录不存在
    //             return;
    //         }
    //
    //         char szTimeStamp[64] = "";
    //         if (m_bNeedTimeStamp)
    //         {
    //             nsCPPub::GetDateTimeStr(szTimeStamp);
    //         }
    //
    //         char szFilename[FILENAME_MAX] = "";
    //         sprintf(szFilename, "%s/%s.txt", pDir, m_szPrefix);
    //
    //         nsCPLock::CAutolock autolock(&m_lock);
    //
    //         //记日志
    //         FILE *fp = fopen(szFilename, "a");
    //         if (NULL != fp)
    //         {
    //             if (m_bNeedTimeStamp)
    //             {
    //                 fprintf(fp, "[%s]", szTimeStamp);
    //             }
    //             vfprintf(fp, pFormat, ap);
    //
    //             fclose(fp);
    //         }
    //
    //         //屏幕输出
    //         if (bConsoleShow == true)
    //         {
    //             if (m_bNeedTimeStamp)
    //             {
    //                 printf("[%s]", szTimeStamp);
    //             }
    //             vprintf(pFormat, ap);
    //         }
    //
    //         //日志文件足够大，就重命名
    //         uint64_t u64FileSize = 0;
    //         int iRet = nsCPPub::GetFileSize(szFilename, u64FileSize);
    //         if (iRet == 0 && u64FileSize >= m_MaxLogSize)
    //         {
    //             char szDestFilename[FILENAME_MAX] = "";
    //             sprintf(szDestFilename, "%s.%d", szFilename, time(NULL));
    //             nsCPPub::TryRenameFile(szFilename, szDestFilename);
    //         }
    //     }
    //     void TryLPrintf(const char *pDir, bool bConsoleShow, const char *pFormat, ...)
    //     {
    //         va_list ap;
    //         va_start(ap, pFormat);
    //         TryLPrintfKernel(pDir, bConsoleShow, pFormat, ap);
    //         va_end(ap);
    //     }
    //     void TryShowLPrintf(const char *pFormat, ...)
    //     {
    //         va_list ap;
    //         va_start(ap, pFormat);
    //         TryLPrintfKernel(m_szLogDir, true, pFormat, ap);
    //         va_end(ap);
    //     }
    //     void TrySaveLPrintf(const char *pFormat, ...)
    //     {
    //         va_list ap;
    //         va_start(ap, pFormat);
    //         TryLPrintfKernel(m_szLogDir, false, pFormat, ap);
    //         va_end(ap);
    //     }
    // protected:
    // private:
    //     char m_szLogDir[FILENAME_MAX];  //日志目录
    //     char m_szPrefix[FILENAME_MAX];  //日志前缀
    //     bool m_bNeedTimeStamp;          //是否需要时间戳
    //     size_t m_MaxLogSize;            //日志文件的最大长度
    //     nsCPLock::CLock m_lock;         //日志锁
    // };

}   // end of namespace

#endif  // end of #ifndef
