
#ifndef CP_LOCK_H
#define CP_LOCK_H

namespace nsCPLock
{

#ifdef WIN32
    #define WINDOWS_PLATFORM
#endif

#ifdef WIN64
    #define WINDOWS_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM //是WINDOWS平台
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    class CLock
    {
        public:
            CLock()
            {
                InitializeCriticalSection ( &m_cri );
                m_initFlag = 1;
            }

            ~CLock()
            {
                DeleteCriticalSection ( &m_cri );
            }

            void Init()
            {
                if ( m_initFlag != 1 )
                {
                    InitializeCriticalSection ( &m_cri );
                    m_initFlag = 1;
                }
            }

            void Lock()
            {
                EnterCriticalSection ( &m_cri );
            }

            void UnLock()
            {
                LeaveCriticalSection ( &m_cri );
            }
        protected:
        private:
            CRITICAL_SECTION m_cri;
            char             m_initFlag;
    };
#else
#include <pthread.h>
    class CLock
    {
        public:
            CLock()
            {
                pthread_mutex_init ( &m_cri, NULL );
                m_initFlag = 1;
            }

            ~CLock()
            {
                pthread_mutex_destroy ( &m_cri );
            }

            void Init()
            {
                if ( m_initFlag != 1 )
                {
                    pthread_mutex_init ( &m_cri, NULL );
                    m_initFlag = 1;
                }
            }

            void Lock()
            {
                pthread_mutex_lock ( &m_cri );
            }

            void UnLock()
            {
                pthread_mutex_unlock ( &m_cri );
            }
        protected:
        private:
            pthread_mutex_t m_cri;
            char            m_initFlag;
    };
#endif

    //自动锁：利用构造函数/析构函数来申请/释放锁
    class CAutolock
    {
        public:
            CAutolock ( CLock* pLock )
            {
                m_pLock = pLock;

                if ( m_pLock != NULL )
                {
                    m_pLock->Lock();
                }
            }
            ~CAutolock()
            {
                if ( m_pLock != NULL )
                {
                    m_pLock->UnLock();
                }
            }
        protected:
        private:
            CLock* m_pLock;
    };

}   // end of namespace

#endif  // end of #ifndef
