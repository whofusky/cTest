
#ifndef CP_LOG_H
#define CP_LOG_H

#include "CPLock.h"
#include "CPPub.h"

#include <stdarg.h>
#include <assert.h>

//�����ļ���(·��Ĭ������־���·����ͬһ��Ŀ¼)
#define MC_LOGDEBUG_RRQF "logDebugCfg"

namespace nsCPLog
{

    //ÿ������ļ�����־
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
            //FUNC  ��ʼ��
            //PARAM pLogDir����־Ŀ¼
            //      pPrefix����־ǰ׺
            //      bNeedTimeStamp���Ƿ���Ҫʱ���
            //      FlushFreq��ˢ��Ƶ�ʣ���Ϊ3����ʾÿд���ξ�ǿ��ˢ���ļ�
            //      SavePeriod���������ڣ���Ϊ7����ʾֻ�������7���ڵ���־�ļ�
            //      readFileFrq������־���������ļ���Ƶ��(��λ����)
            //RET   0����ʼ���ɹ�
            //      ��������ʼ��ʧ�ܣ����ش�����
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
                    //��־Ŀ¼������
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

            //FUNC  ��¼��־��printf�汾
            //PARAM bConsoleShow���Ƿ���ʾ�ڿ���̨��
            //      debugLevel  :������־�Ƿ��������־����
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

                //�ж��Ƿ���Ҫ����־������������ļ�
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

                //�жϸ������õ�ǰ��־�Ƿ���Ҫ���
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

            //FUNC  ǿ��ˢ����־�ļ�
            void FlushData()
            {
                if ( m_fpA != NULL )
                {
                    fflush ( m_fpA );
                }
            }

        protected:
            //FUNC  ����ˢ������
            void TryFlushingData()
            {
                time_t tNow = time ( NULL );

                if ( m_tLastOpenTime + 10 * 60 < tNow || m_SaveCount >= m_FlushFreq )
                {
                    //���ļ�����ʮ���� || ��������Ѿ�����ˢ������

                    fflush ( m_fpA );   //ˢ��
                    m_SaveCount = 0;    //�����������

                    //�ж��Ƿ���Ҫ���´�һ���ļ�
#ifdef WINDOWS_PLATFORM
                    struct tm* pNow = localtime ( &tNow );
#else
                    struct tm stm4Now;
                    struct tm* pNow = localtime_r ( &tNow, &stm4Now );
#endif

                    if ( m_tLastOpenTime + 10 * 60 < tNow || pNow->tm_mday != m_tLastOpenDay )
                    {
                        //���ļ�����ʮ���� || �Ѿ�����

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

                        CleanOldLog();  //ÿ��һ�����ļ���������һ�ξ���־
                    }

                    assert ( m_fpA != NULL );
                }
            }
            //FUNC  ����ɵ���־�ļ�
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
            char m_szLogDir[FILENAME_MAX];  //��־Ŀ¼
            char m_szPrefix[FILENAME_MAX];  //��־ǰ׺
            bool m_bNeedTimeStamp;          //�Ƿ���Ҫʱ���
            size_t m_FlushFreq;             //ˢ��Ƶ��
            size_t m_SavePeriod;            //��������
            nsCPLock::CLock m_lock;         //д�ļ�����
            FILE* m_fpA;                    //�ļ�׷��ָ��
            size_t m_SaveCount;             //�������
            time_t m_tLastOpenDay;          //�ϴδ��ļ���ʱ�䣬����Ϊ��λ
            time_t m_tLastOpenTime;         //�ϴδ��ļ���ʱ�䣬����Ϊ��λ
            
            nsCPLock::CLock m_rCfgLock;     //�������ļ���
            FILE* m_fpCfg;                  //�ļ�׷��ָ��
            unsigned char m_preMinute;      //��¼��һ�������־�ķ���
            char m_readFileFrq;             //�������ļ�(����debugMask)��Ƶ��
            unsigned int  m_debugMask;      //���ڿ�����־�Ƿ����������
    };
    //
    // //���Լ�¼��־
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
    //     //FUNC  ��ʼ��
    //     //PARAM pPrefix����־ǰ׺
    //     //      bNeedTimeStamp���Ƿ���Ҫʱ���
    //     //      MaxLogSize����־�ļ�������С������־�ļ������˴�С�������������ǰ�ļ�����ʹ��һ�����ļ�
    //     //RET   0����ʼ���ɹ�
    //     //      ��������ʼ��ʧ��
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
    //         {//Ŀ¼������
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
    //         //����־
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
    //         //��Ļ���
    //         if (bConsoleShow == true)
    //         {
    //             if (m_bNeedTimeStamp)
    //             {
    //                 printf("[%s]", szTimeStamp);
    //             }
    //             vprintf(pFormat, ap);
    //         }
    //
    //         //��־�ļ��㹻�󣬾�������
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
    //     char m_szLogDir[FILENAME_MAX];  //��־Ŀ¼
    //     char m_szPrefix[FILENAME_MAX];  //��־ǰ׺
    //     bool m_bNeedTimeStamp;          //�Ƿ���Ҫʱ���
    //     size_t m_MaxLogSize;            //��־�ļ�����󳤶�
    //     nsCPLock::CLock m_lock;         //��־��
    // };

}   // end of namespace

#endif  // end of #ifndef
