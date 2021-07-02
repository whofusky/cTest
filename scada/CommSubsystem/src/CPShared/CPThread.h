
#ifndef CP_THREAD_H
#define CP_THREAD_H

#include <stdio.h>

#ifdef WIN32
#define WINDOWS_PLATFORM
#endif

#ifdef WIN64
#define WINDOWS_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM //��WINDOWSƽ̨
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
#endif

//#include "./CPMem.h"

namespace nsCPThread
{

//�̹߳���
class CThread
{
public:
    CThread(size_t MaxThreadCount = 1024)
    {
        m_MaxThreadCount = MaxThreadCount;
        m_CurrThreadCount = 0;
        m_pThreadID = NULL;
    }
    //FUNC  �����߳�
    //PARAM ThreadFunc���̺߳���
    //      pParam���̺߳����Ĳ���
    //      ThreadCount���߳�����
	//      usSetProi : �߳����ȼ�
	//      strategy  : �̵߳��Ȳ���
    //RET   0�������ɹ�
    //      ����������ʧ�ܣ����ش�����
    int StartThread(void (*ThreadFunc)(void*), void *pParam = NULL, size_t ThreadCount = 1, int usSetProi = 0 , int strategy = 0)
    {
        if (ThreadFunc == NULL)
        {
            return -1;
        }

        //�����߳�ID���飬���ڱ������������̵߳���Ϣ
        if (m_pThreadID == NULL)
        {
            m_pThreadID = new void *[m_MaxThreadCount];//nsCPMem::New<void*>(m_MaxThreadCount);
            if (m_pThreadID == NULL)
            {
                return -10;
            }
        }

        //�����߳�
        size_t i = 0;
        for (i = 0; i < ThreadCount && m_CurrThreadCount < m_MaxThreadCount; ++i, ++m_CurrThreadCount)
        {
#ifdef WINDOWS_PLATFORM
            unsigned long ulRet = _beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFunc, pParam, 0, NULL);
            if (ulRet == -1L)
            {
                return -100;
            }
            m_pThreadID[m_CurrThreadCount] = (void*)ulRet;
			if ( usSetProi != THREAD_PRIORITY_NORMAL )
			{
				SetThreadPriority((HANDLE)ulRet, usSetProi);
				//int threadnum = GetThreadPriority((HANDLE)ulRet);
			}			
#else

			pthread_attr_t attr;
			struct sched_param param;
            pthread_t pid;
			pthread_attr_init(&attr);
			param.sched_priority = usSetProi;
			//Ҫʹ���ȼ������ñ���Ҫ����仰
			pthread_attr_setinheritsched(&attr , PTHREAD_EXPLICIT_SCHED);
			//�����̵߳��Ȳ���
			//1.SCHED_OTHER ��ʱ���Ȳ���(ϵͳĬ��)
			////SCHED_FIFO ʵʱ���Ȳ��ԣ��ȵ��ȷ���һ��ռ��cpu��һֱ���С�һֱ����ֱ���и������ȼ����񵽴���Լ�������
			//SCHED_RR ʵʱ���Ȳ��ԣ�ʱ��Ƭ��ת�������̵�ʱ��Ƭ���꣬ϵͳ�����·���ʱ��Ƭ�������ھ�������β��
			//���ڶ���β��֤�����о�����ͬ���ȼ���RR����ĵ��ȹ�ƽ��
			pthread_attr_setschedpolicy(&attr , strategy);
			//�����߳����ȼ�
			pthread_attr_setschedparam(&attr , &param);
            int iRet = pthread_create(&pid, &attr, (void *(*)(void *))ThreadFunc, pParam);
            printf("pthread_create::iRet=%d, errno= %d,errorMsg =%s\r\n",
            		iRet, errno,strerror(errno) );
            if (iRet != 0)
            {
                return -100;
            }

            m_pThreadID[m_CurrThreadCount] = (void*)pid;
#endif
            //printf("thread %d create, id is %u\n", i, (int)m_pThreadID[i]);
        }

        return 0;
    }
    //FUNC  �ȴ��߳��˳�
    //RET   0�������߳��Ѿ��˳�
    int WaitForQuit()
    {
#ifdef WINDOWS_PLATFORM
        for (size_t i = 0; i < m_CurrThreadCount; ++i)
        {
            WaitForSingleObject((HANDLE)m_pThreadID[i], INFINITE);
            CloseHandle((HANDLE)m_pThreadID[i]);
            //printf("thread %u quit, id is %u\n", i, (int)m_pThreadID[i]);
        }
        m_CurrThreadCount = 0;
#else
        for (size_t i = 0; i < m_CurrThreadCount; ++i)
        {
            //printf("Wait Thread %u : %u Quit\n", i, (pthread_t)m_pThreadID[i]);
            pthread_join((pthread_t)m_pThreadID[i],NULL);
            //printf("thread %u quit, id is %u\n", i, (int)m_pThreadID[i]);
        }
        m_CurrThreadCount = 0;
#endif
        return 0;
    }
    size_t GetThreadCount()
    {
        return m_CurrThreadCount;
    }
protected:
private:
    size_t m_MaxThreadCount;    //����߳�����
    size_t m_CurrThreadCount;   //�߳�����
    void **m_pThreadID;         //�߳�ID�����׵�ַ�����ڱ������������̵߳���Ϣ
};

//FUNC  �õ��߳�ID
static int GetThreadID()
{
#ifdef WINDOWS_PLATFORM
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

}   // end of namespace

#endif  // end of #ifndef
