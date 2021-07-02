//˫�������(C)������(B)���ܵ����ȼ�(P)����ʵ���ļ�
//
//˵����1.����������ȼ��Ķ�����ɣ����ڸ����еĵ�Ԫ�ṹ��ͬ��ÿ��ӵ���Լ����¼�/�źš�
//      2.ÿ�������Ӻͳ��Ӳ����������һ����У���Ӻ��͸���ӵ�е��¼�/�źš�
//      3.�����ڽ��յ��¼�/�ź�������У��¼�/�ź���ʹ���ֹ���λ���ƣ����ڳ��Ӻ��������
//        ���о��ʱ�ŻḴλ��
//      4.���ڶ�������ݣ�������������������¼�/�ź�������λ��Ϊ��ֹƵ�ʿ�ѭִ�г��Ӳ�
//        ������ǰ��sleep 280ms(�궨��)���ڴ��ڼ䲻��Ӱ����Ӳ�����
//      5.���Ӳ��������ַ�ʽ��ѡ����ȴ���ӣ��¼�����ӡ����������е�1���¼�������Ӧ��
//        �޵�1���¼�ʱ��Ӧ��2���¼������¼������ʱ��1�����ȼ���ߡ������¼�ͬʱ����ʱ��
//        δ��Ӧ���¼�����ʧ��
//      6.��ʹ�������ȼ����ԶԵ�Ϊ����Ӧ�ø��ֻҪ��֤���Ψһ�ԣ����Դ���Ϊ���ӵ��Ⱥ�
//        ˳�򼴿ɡ�
//
//��λ���׷����Ϸ��ܹ���(�Ϻ�)���޹�˾
//��д�ˣ�Liu ambition
//��дʱ�䣺2016��12��
//

#include "duGrp_BCPQ.h"
#include <stdio.h>

/************************************************************/
/*                 ƽ̨��ز��֣���̬����                    */
/************************************************************/
//
#ifdef  WINDOWS_PLATFORM

//#define handle HANDLE
//
//
////////////////windows 64λ������ȴ��¼�����
////
//���ܣ���ģʽִ���¼��ȴ�ػ�õ��¼���
//���룺
//      wMode���ȴ�ʽ��=1���ȴ��1�������¼���=2�ȴ��2�������¼���
//                       =3���ȴ��1�͵�2������һ������¼������Ȼ�ȡ��1��
//      hEvt_p��Ҫ��ȡ���¼����ָ�롣
//�����
//      ��
//���أ�ִ��״̬��=1,��ȡ����1�������¼���=2��,��ȡ����2�������¼���
//                =-1������Ƿ���=-2���ȴ�ʱ��=-3���ȴ�ʧ��
//
static int waitEvt_WIN64 ( u_char wMode,  C_DuGrp_BCPQ* obj_p )
{
    DWORD dwRet = 0;

    switch ( wMode )
    {
        case 1://wait only 1 evt
        case 2:
        {
            dwRet = WaitForMultipleObjects ( 1, obj_p->getEvtMem(), false, INFINITE );

            if ( WAIT_TIMEOUT == dwRet )
            {
                return -2;
            }

            if ( WAIT_FAILED == dwRet )
            {
                return -3;
            }

            switch ( dwRet )
            {
                case WAIT_OBJECT_0: //get fstG evt, dont upset scdG evt!!!
                    return 1;       //maybe scdG evt still wait

                case WAIT_OBJECT_0+1:   //only exsit scdG evt
                    return 2;       //no fstG evt

                default:
                    return -5;

            }

            return wMode;
        }

        case 3://wait 2 evt
        {
            dwRet = WaitForMultipleObjects ( 2, obj_p->getEvtMem(), false, INFINITE );

            switch ( dwRet )
            {
                case WAIT_TIMEOUT:
                    return -2;

                case WAIT_FAILED:
                    return -3;

                case WAIT_OBJECT_0: //get fstG evt, dont upset scdG evt!!!
                    return 1;       //maybe scdG evt still wait

                case WAIT_OBJECT_0+1:   //only exsit scdG evt
                    return 2;       //no fstG evt

                default:            //never happen
                    return -5;
            }
        }

        default:
            return -1;                  //never happen
    }
}

//
//
////////////////window 64λ�����������¼�����
////
//���ܣ�����������������¼���event�������Լ��İ�ȫ���ԣ��Զ���λ����ʼ���¼���
//���룺
//      evt_p���¼����ָ��
//�����
//      ��
//���أ�ִ��״̬��=0���ɹ���=-1��ʧ��
//
static int  crtEvt_WIN64 ( T_EVT* evt_p )
{
    HANDLE* tEvt_p;
    tEvt_p = evt_p;
    *tEvt_p = CreateEvent ( NULL, false, false, NULL );

    if ( ! ( *tEvt_p ) )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/*code walkthrough*/static int  sndEvt_WIN64(grpQ* grpQ_p)
/*code walkthrough*/{
/*code walkthrough*/	int tRet;
/*code walkthrough*/	tRet = SetEvent(grpQ_p->Ev_Nfy);
/*code walkthrough*/	if (0 == tRet)
/*code walkthrough*/	{
/*code walkthrough*/		return -1;
/*code walkthrough*/	}
/*code walkthrough*/	grpQ_p->evtSnd++;
/*code walkthrough*/	if (grpQ_p->evtSnd > grpQ_p->evtRev)
/*code walkthrough*/	{
/*code walkthrough*/		grpQ_p->holdEvtFlag = 1;
/*code walkthrough*/		tRet = 0;
/*code walkthrough*/	}
/*code walkthrough*/	else
/*code walkthrough*/	{
/*code walkthrough*/		grpQ_p->holdEvtFlag = 0;
/*code walkthrough*/		tRet = -1;
/*code walkthrough*/	}
/*code walkthrough*/	return tRet;
/*code walkthrough*/}

/*code walkthrough*/#define crtEvt(hEvt_p)	(crtEvt_WIN64(hEvt_p))//#define crtEvt(hEvt_p, evtBody_p)       (crtEvt_WIN64(hEvt_p))//discard para evtBody_p
/*code walkthrough*/#define sndEvt(grpQ_p)	(sndEvt_WIN64(grpQ_p))//#define sndEvt(hEvt, grpQ_p)        (SetEvent(hEvt))//discard para grpQ
#define waitEvt(wMode, obj_p)   (waitEvt_WIN64(wMode,obj_p))
/*code walkthrough*/#define freeEvt(hEvt)	(CloseHandle(hEvt))//#define freeEvt(hEvt, evtBody_p)            (CloseHandle(hEvt))//discard para evtBody_p

/*code walkthrough*/#define clrEvt(hEvt)	(ResetEvent(hEvt))//#define clrEvt(hEvt, grpQ_p)            (ResetEvent(hEvt))//discard para grpQ_p
#define crtQLock(qKey)      (InitializeCriticalSection(&qKey))
#define freeQLock(qKey)     (DeleteCriticalSection(&qKey))
#define lockQ(qKey)         (EnterCriticalSection(&qKey))
#define unlockQ(qKey)       (LeaveCriticalSection(&qKey))
#define REST                280 //ms
//#define sleep(CNT)          (Sleep(CNT))
#else
#include <sys/time.h>
#include <string.h>

#define lockQ_print(qKey) \
        {\
            pthread_mutex_lock(&qKey);\
            printf("===fusktest===C_DuGrp_BCPQ--->lockQ ==>1111 \n");\
        }

#define unlockQ_print(qKey) \
        {\
            pthread_mutex_unlock(&qKey);\
            printf("===fusktest===C_DuGrp_BCPQ--->unlockQ ==>2222 \n");\
        }

#define crtQLock(qKey)      (pthread_mutex_init(&qKey,NULL))
#define freeQLock(qKey)     (pthread_mutex_destroy(&qKey))
//#define lockQ(qKey)         (pthread_mutex_lock(&qKey))
//#define unlockQ(qKey)       (pthread_mutex_unlock(&qKey))
#define lockQ(qKey)         (lockQ_print(qKey))
#define unlockQ(qKey)       (unlockQ_print(qKey))
#define REST                280  //time = 70ticks * 4ms/ticks = 280ms

//
//
////////////////linux 64λ�����������¼�����
////
//���ܣ�����˽�е��ź�����̣���ʼΪ��
//      
//���룺
//      evt_p���ź���ʵ��ָ��
//�����
//      ��
//���أ�ִ��״̬��=0���ɹ���=-1��ʧ��
//
/*code walkthrough*/static int  crtEvt_LNX64(T_EVT evt_p)//static int  crtEvt_LNX64 ( T_EVT* evt_pp, T_EVT* evtBody_p )
{
	/*code walkthrough*///sem_t* tEvt_p;
	/*code walkthrough*///tEvt_p = *evt_pp;
	/*code walkthrough*/return sem_init(evt_p, 0, 0);
	/*code walkthrough*//*if ( 0xFFFFFFFF == * ( int* ) ( evtBody_p ) ) //un-create
    {
        return sem_init ( tEvt_p, 0, 0 );
    }
    else
    {
        *evt_pp = *evtBody_p;    //use body ptr before created! evt_p = 0xFFFFFFFF
    }
    return 0;//succ!*/
}

//
//
////////////////linux 64λ�����鷢���¼�����
////
//���ܣ���ָ���������Ϸ����¼���posix�ź�����
//���룺
//      grpQ_p�����������ָ��
//�����
//      ��
//���أ�ִ��״̬��=0,�ɹ���=-1��ʧ��
//
/*code walkthrough*/static int  sndEvt_LNX64(grpQ* grpQ_p)//static int  sndEvt_LNX64(T_EVT evt_p, grpQ* grpQ_p)
{
	/*code walkthrough*/int tRet = -1;
                        lockQ(grpQ_p->grpQKey);
	/*code walkthrough*/if(grpQ_p->evtSnd >= grpQ_p->evtRev)//����0
	/*code walkthrough*/{
	/*code walkthrough*/	grpQ_p->evtSnd = grpQ_p->evtSnd - grpQ_p->evtRev;
	/*code walkthrough*/	grpQ_p->evtRev = 0;
	/*code walkthrough*/	if (grpQ_p->evtSnd >= (u_short)0x1000)//���۵�δ�����¼���࣬�����⣬���ٷ���ֱ�ӷ���
	/*code walkthrough*/	{
                                unlockQ(grpQ_p->grpQKey);
	/*code walkthrough*/		return tRet;
	/*code walkthrough*/	}
	/*code walkthrough*/}
	/*code walkthrough*/else
	/*code walkthrough*/{
                            unlockQ(grpQ_p->grpQKey);
	/*code walkthrough*/	return tRet;
	/*code walkthrough*/}
	/*code walkthrough*/// grpQ_p->holdEvtFlag ++;//snd evt
	/*code walkthrough*/tRet = sem_post(grpQ_p->Ev_Nfy);// return sem_post ( evt_p );
	/*code walkthrough*/if (-1 == tRet)
	/*code walkthrough*/{
                            unlockQ(grpQ_p->grpQKey);
	/*code walkthrough*/	return tRet;
	/*code walkthrough*/}
	/*code walkthrough*/grpQ_p->evtSnd++;
	/*code walkthrough*/grpQ_p->holdEvtFlag = 1;
                        unlockQ(grpQ_p->grpQKey);
	/*code walkthrough*/return tRet;
}

//
//
////////////////linux 64λ������ȴ��¼�����
////
//���ܣ���ģʽִ���¼��ȴ�ػ�õ��¼��š��ڵȴ�����һ������¼��ķ�ʽ�У�
//      �����¼��ͻ���ԭ��һһ��ԣ���1���2���¼�ͬʱ����ʱ�����۱�����������
//      ��������ѣ����Ե�1������ԭ���䰴��ȡ����1���¼����ﴦ��
//���룺
//      wMode���ȴ�ʽ��=1���ȴ��1�������¼���=2�ȴ��1�������¼���
//                       =3���ȴ��1�͵�2������һ������¼������Ȼ�ȡ��1��
//      obj_p��˫���������ָ��
//�����
//      ��
//���أ�ִ��״̬��=1,��ȡ����1�������¼���=2��,��ȡ����2�������¼���
//                =-1������Ƿ���=-2���ȴ�ʱ��=-3���ȴ�ʧ�ܣ�=-5��δ֪ԭ��ʧ��
//
static int  waitEvt_LNX64(int wMode, C_DuGrp_BCPQ* obj_p)
{
	int evt = -1;

    int retryNum = 6000;//permit to wait 5 minute
    int hasEvtGrpFlag = 0;

    struct timeval tTmLcal;
    int tRet = gettimeofday ( &tTmLcal, NULL );
    if( tRet != 0 )
    {
        return -5;
    }
	struct timespec tTmOut;
	tTmOut.tv_sec = tTmLcal.tv_sec;
    tTmOut.tv_nsec = tTmLcal.tv_usec * 1000;
    long tAddNsec = 50000000;//wait 50 ms if no evt

	grpQ* tGrp1Q, *tGrp2Q;
	tGrp1Q = obj_p->getFstGrpQ();
	tGrp2Q = obj_p->getScdGrpQ();

	do{
		if (0 == retryNum )
			return -3;

        tTmOut.tv_nsec = tTmOut.tv_nsec + tAddNsec;
        if( tTmOut.tv_nsec >= 1000000000 )
        {
            tTmOut.tv_sec = tTmOut.tv_sec +1;
            tTmOut.tv_nsec = 0;
        }

		if (wMode == 1)
		{
			evt = sem_timedwait(tGrp1Q->Ev_Nfy, &tTmOut);
		}
		else if (wMode == 2)
		{
			evt = sem_timedwait(tGrp2Q->Ev_Nfy, &tTmOut);
		}
		else// wMode == 3
		{
			if (0 == tGrp1Q->holdEvtFlag)//G1 no evt, wait G2 evt
			{
				evt = sem_timedwait(tGrp2Q->Ev_Nfy, &tTmOut);
				if(evt == 0) //G2 has evt
				{
					hasEvtGrpFlag = 2;
				}
				else//G2 no evt, return to wait G1 evt
				{
					evt = sem_timedwait(tGrp1Q->Ev_Nfy, &tTmOut);
					if (evt == 0) //G1 has evt.
					{
						hasEvtGrpFlag = 1;
					}
				}
			}
			else//G1 has evt!
			{
				evt = sem_timedwait(tGrp1Q->Ev_Nfy, &tTmOut);
				if (evt == 0) //G1 has evt.
				{
					hasEvtGrpFlag = 1;
				}
				else//has evt, but get nothing!
				{
					return -5;
				}
			}
		}
		retryNum--;
	}while (evt == -1);

	//get evt:
    switch(wMode)
	{
		case 1://get G1 evt
		{
            lockQ(tGrp1Q->grpQKey);
			tGrp1Q->evtRev++;
			if (tGrp1Q->evtRev > tGrp1Q->evtSnd)
			{
                unlockQ(tGrp1Q->grpQKey);
				return -5;
			}
			else if(tGrp1Q->evtRev == tGrp1Q->evtSnd)
			{
				/*code walkthrough*/tGrp1Q->evtSnd = 0;//����0
				/*code walkthrough*/tGrp1Q->evtRev = 0;//����0
				tGrp1Q->holdEvtFlag = 0;
			}
			else
			{
				/*code walkthrough*/tGrp1Q->evtSnd = tGrp1Q->evtSnd - tGrp1Q->evtRev;//����0
				/*code walkthrough*/tGrp1Q->evtRev = 0;//����0
				tGrp1Q->holdEvtFlag = 1;
			}
            unlockQ(tGrp1Q->grpQKey);
			return 1;//fst Grp evt
		}
		case 2: //get G2 evt
		{
            lockQ(tGrp2Q->grpQKey);
            tGrp2Q->evtRev++;
			if (tGrp2Q->evtRev > tGrp2Q->evtSnd)
			{
                unlockQ(tGrp2Q->grpQKey);
				return -5;
			}
			else if (tGrp2Q->evtRev == tGrp2Q->evtSnd)
			{
				/*code walkthrough*/tGrp2Q->evtSnd = 0;//����0
				/*code walkthrough*/tGrp2Q->evtRev = 0;//����0
				tGrp2Q->holdEvtFlag = 0;
			}
			else
			{
				/*code walkthrough*/tGrp2Q->evtSnd = tGrp2Q->evtSnd - tGrp2Q->evtRev;//����0
				/*code walkthrough*/tGrp2Q->evtRev = 0;//����0
				tGrp2Q->holdEvtFlag = 1;
			}
            unlockQ(tGrp2Q->grpQKey);
			return 2;//scd Grp evt
		}
		case 3: //get G1 or G2 evt
		{
			if (hasEvtGrpFlag == 1) //get G1 evt
			{
                lockQ(tGrp1Q->grpQKey);
                tGrp1Q->evtRev++;
				if (tGrp1Q->evtRev > tGrp1Q->evtSnd)
				{
                    unlockQ(tGrp1Q->grpQKey);
					return -5;
				}
				else if (tGrp1Q->evtRev == tGrp1Q->evtSnd)
				{
					/*code walkthrough*/tGrp1Q->evtSnd = 0;//����0
					/*code walkthrough*/tGrp1Q->evtRev = 0;//����0
					tGrp1Q->holdEvtFlag = 0;
				}
				else
				{
					/*code walkthrough*/tGrp1Q->evtSnd = tGrp1Q->evtSnd - tGrp1Q->evtRev;//����0
					/*code walkthrough*/tGrp1Q->evtRev = 0;//����0
					tGrp1Q->holdEvtFlag = 1;
				}
                unlockQ(tGrp1Q->grpQKey);
				return hasEvtGrpFlag;//fst Grp evt
			}
			if (hasEvtGrpFlag == 2) //get G2 evt
			{
                lockQ(tGrp2Q->grpQKey);
                tGrp2Q->evtRev++;
				if (tGrp2Q->evtRev > tGrp2Q->evtSnd)
				{
                    unlockQ(tGrp2Q->grpQKey);
					return -5;
				}
				else if (tGrp2Q->evtRev == tGrp2Q->evtSnd)
				{
					/*code walkthrough*/tGrp2Q->evtSnd = 0;//����0
					/*code walkthrough*/tGrp2Q->evtRev = 0;//����0
					tGrp2Q->holdEvtFlag = 0;
				}
				else
				{
					/*code walkthrough*/tGrp2Q->evtSnd = tGrp2Q->evtSnd - tGrp2Q->evtRev;//����0
					/*code walkthrough*/tGrp2Q->evtRev = 0;//����0
					tGrp2Q->holdEvtFlag = 1;
				}
                unlockQ(tGrp2Q->grpQKey);
				return hasEvtGrpFlag;//scd Grp evt
			}
		}
		default:            //never happen
			return -1;
	}
	return -5;
}

//
//
////////////////linux ����������¼�����
////
//���ܣ����ָ���������ۻ��ȫ���¼���posix�ź�����
//���룺
//		grpQ_p�����������ָ��
//�����
//      ��
//���أ�ִ��״̬��=0���ɹ�
//
/*code walkthrough*/static int clrEvt_LNX64(grpQ* grpQ_p)//static int clrEvt_LNX64 (T_EVT evt_p, grpQ* grpQ_p)
{
    grpQ_p->holdEvtFlag = 0;

	/*code walkthrough*/while (sem_trywait(grpQ_p->Ev_Nfy) != -1);//	while (sem_trywait(evt_p) != -1);
    return 0;
}

//
//
////////////////linux ����������¼�������
////
//���ܣ����ָ����������¼�����posix�ź�����������ں��ź����ṹ��
//      ��˫���������Ϊδ�����¼�״̬
//���룺
//      evtBody_p���ź���ʵ��ָ�룬˫�������&evtMem[0/1]
//�����
//      ��
//���أ�ִ��״̬��=0���ɹ���=-1��ʧ��
//
static int freeEvt_LNX64 ( sem_t* evt_p)
{
    int tRtn;
	while (sem_trywait(evt_p) != -1);
	tRtn = sem_destroy(evt_p);
	/*code walkthrough*//*	if (0 == tRtn)
	{
		*(u_int*)evt_p = 0xFFFFFFFF;
	}*/
	return tRtn;
}

/*code walkthroug*/#define crtEvt(hEvt_p)	(crtEvt_LNX64(hEvt_p))//#define crtEvt(hEvt_p, evtBody_p)           (crtEvt_LNX64(hEvt_p,evtBody_p))
/*code walkthroug*/#define sndEvt(grpQ_p)	(sndEvt_LNX64(grpQ_p))//#define sndEvt(evtId, grpQ_p)               (sndEvt_LNX64(evtId, grpQ_p))



#define waitEvt(wMode, obj_p)           (waitEvt_LNX64(wMode, obj_p))
/*code walkthroug*/#define clrEvt(grpQ_p)	(clrEvt_LNX64(grpQ_p))//#define clrEvt(hEvt, grpQ_p)            (clrEvt_LNX64(hEvt, grpQ_p))
/*code walkthrough*/#define freeEvt(hEvt)	(freeEvt_LNX64(hEvt))//#define freeEvt(hEvt, evtBody_p)        (freeEvt_LNX64(evtBody_p))//discard para hEvt

//#define sleep(CNT)          (sleep(CNT))
#endif

/************************************************************/
/*               ƽ̨�޹ز��֣����ʵ��                  */
/************************************************************/
//
//
////////////////��������ͷ����
////�����������ж�ͷ
//���ܣ�Ϊ�������ж�ͷ��������ռ䣬��֧������ʽ����
//���룺
//      qType�������͡�=1��ltG��=2��lgG
//      qNum�����ڶ�����
//�����
//      qHd_p����1����ͷָ��
//���أ�ִ��״̬��>=0,�����Ķ�ͷ��ɹ���=-1������Ƿ���=-2��ʧ��
//
static int creatQHd ( u_char qType, u_short qNum, qHd** qHd_pp )
{
    qHd* tQHd_p;

    if ( ( 1 == qType ) && ( qNum > LIG_MAX_SIZE ) ) //ltG
    {
        return -1;
    }

    if ( ( 2 == qType ) && ( qNum > LGG_MAX_SIZE ) ) //lgG
    {
        return -1;
    }

    if ( !qNum )
    {
        return 0;
    }

    tQHd_p = ( qHd* ) calloc ( qNum, sizeof ( qHd ) ); //init 0

    if ( !tQHd_p )
    {
        return -2;
    }

    *qHd_pp = tQHd_p;
    return qNum;
}

//
//
////�����������ж�����
//���ܣ�Ϊ���ж�ͷ�������ռ䣬ÿ������ռ�������֧�ֶ�ͬһ���������ʽ����
//      ��ͷ��������
//���룺
//      qHd_p�����ڵ�1����ͷָ�룻
//      qHdNum�������贴������Ķ�ͷ��
//      qVol����������
//      qULng��������ݵ�Ԫ����
//�����
//      ��
//���أ�=0,�ɹ���=-1������Ƿ���=-2��ʧ��
//
static int creatQBody ( qHd* qHd_p, u_short qHdNum, u_char qVol, u_char qULng )
{
    qHd* tQHd_p = NULL;
    int i;

    if ( ( !qHd_p ) || ( !qHdNum ) || ( qVol > QB_MAX_VOL ) || ( !qVol ) || ( qULng > QB_MAX_SIZE ) || ( !qULng ) )
    {
        return -1;
    }

    tQHd_p = qHd_p;

    for ( i = 1, tQHd_p; i <= qHdNum; i++, tQHd_p++ )
    {
        tQHd_p->qBody_p = ( char* ) malloc ( qVol * qULng );

        if ( !tQHd_p->qBody_p )
        {
            return -2;
        }

    }

    return 0;
}

//
//
////LtG���Ͷ�������ҳ��Ӷ�ͷ
//���ܣ����ټ��Lt�������ж�ͷ���ж������Ƿ�����ݣ������ж��Ƿ�������ӵ�������������
//      �������������ͷָ��
//���룺
//      grpQ_p��������ָ��
//�����
//      qHd_pp����ͷָ���ָ�룬��ſ��Գ��ӵĶ�ͷָ��ĵ�ַ�����NULL��ʾ�޿ɳ��ӵĶ��У�
//      grpQMk_p����ų��Ӷ��еĸ�λ����
//���أ�ִ��״̬��=0,�ҵ�����ݵĶ��У�����������������������µĳ��ӣ�
//      =1���ҵ�����ݵĶ����ҿ��Գ��ӣ��������Ļ�����������Ҫ�󣩣�=-4�����пմ���
//
static int findPopLtQHd ( grpQ* grpQ_p, qHd** qHd_pp, u_short* grpQMk_p )
{
	u_short tGrpQStCd;
    grpQ* tGrpQ_p;
    qHd* tGrpQHd_p;
	int tQhdUseCnt = 0;
	u_short tQHdInx = 0;

    tGrpQ_p = grpQ_p;
	tGrpQHd_p = tGrpQ_p->grpQHd_p;
    tGrpQStCd = tGrpQ_p->grpQStCd.ltGrpQStMk;//u_short,16λ

	u_short tTstGrpQStCd;
	u_short tQuickRet;
	u_short tShiftMask = 0xFFFF;
	tQuickRet = tGrpQStCd;
	*grpQMk_p = 0xFFFF;
	*qHd_pp = NULL;
	int findFlag = 0;

	do{
		if (!tQuickRet)
		{
			if (findFlag)//find and context disable
			{
//				sndEvt(tGrpQ_p);
				return 0; 
			}
			return -4;
		}
	
		tShiftMask = tShiftMask << 1;//1111 1111 1111 111[0]b
		tQuickRet = tGrpQStCd & tShiftMask;//xxxx xxxx xxxx xxx[0]b
		tTstGrpQStCd = tGrpQStCd & (~tShiftMask);//0000 0000 0000 000[x]b
		if (!tTstGrpQStCd)//0000 0000 0000 000[0]b, go on!
		{
			tQHdInx++;//nxt bit no.
			continue;
		}

		//0000 0000 0000 000[1]b
		//fix position
		tGrpQHd_p = tGrpQHd_p + tQHdInx;//set tGrpQHd_p to pop q header
		tQhdUseCnt += tGrpQHd_p->usedBlkNum;//compute total using block num.

		//check boundary
		if (tQhdUseCnt > tGrpQ_p->qUsdUTCnt || tGrpQHd_p >= (tGrpQ_p->grpQHd_p + tGrpQ_p->grpQNum))
		{
			return -4;
		}

		if(tGrpQHd_p->usedBlkNum)//target Q exsit data block, match with grpQStCd.ltGrpQStMk
		{
			//check context
			if (tGrpQHd_p->qCnxt_p && !(*(tGrpQHd_p->qCnxt_p)))//context disable, pop info has prepared, and go on!
			{
				if (!findFlag)//first find
				{
					//prepare pop info.
					*qHd_pp = tGrpQHd_p;
					*grpQMk_p = ~((u_short)1 << tQHdInx);//set pop Q. reset mask
					findFlag = 1;//set find flag
				}
				tQHdInx++;//nxt bit no. to find next no context or context enable Q data block.
				tGrpQHd_p = tGrpQ_p->grpQHd_p;//restore tGrpQHd_p
				continue;
			}

			//prepare pop info.
			*qHd_pp = tGrpQHd_p;
			*grpQMk_p = ~((u_short)1 << tQHdInx);//set pop Q. reset mask
			return 1;//data can pop
		}
		else//at fix position, target Q no data block, dismatch with grpQStCd.ltGrpQStMk
		{
			return -4;
		}
	}while(tQHdInx < tGrpQ_p->grpQNum);

	return -4;//tQHdInx > tGrpQ_p->grpQNum
}

//
//
////LgG���Ͷ�������ҳ��Ӷ�ͷ
//���ܣ����ټ��Lg�������ж�ͷ���ж������Ƿ�����ݣ������ж��Ƿ�������ӵ�������������
//      �������������ͷָ��
//���룺
//      grpQ_p��������ָ��
//�����
//      qHd_pp����ſ��Գ��ӵĶ�ͷָ��ĵ�ַ��NULL��ʾ�޿ɳ��ӵĶ��У�
//      qRef_p����ų��Ӷ��е���������ţ�u_short[qRef_p]
//		grpQMk_p����ų��Ӷ��еĸ�λ���룬u_short[qRef_p]=1111[0]111��bitNo = 3
//���أ�ִ��״̬��=0,�ҵ�����ݵĶ��У��������������ܳ��ӣ�
//      =1���ҵ�����ݵĶ����ҿ��Գ��ӣ�=-4�����пմ���
//
static int findPopLgQHd(grpQ* grpQ_p, qHd** qHd_pp, u_char* qRef_p, u_short* grpQMk_p)
{
    grpQ* tGrpQ_p;
    qHd* tGrpQHd_p;
	u_short tGrpQStCd[64];
	int i,j;// , tRtn;
	int tQhdUseCnt = 0;
	u_short tQHdInx = 0;

	tGrpQ_p = grpQ_p;
	tGrpQHd_p = tGrpQ_p->grpQHd_p;
	for(i = 0; i < 64; i++)//64*16 bits
		tGrpQStCd[i] = tGrpQ_p->grpQStCd.lgGrpQStMk[i];


	u_short tTstGrpQStCd;
	u_short tQuickRet;
	u_short tShiftMask;
	*qRef_p = 0xFF;
	*grpQMk_p = 0xFF;
	*qHd_pp = NULL;
	int findFlag = 0;

	for (i = 0; i < 64; i++) //find mask block, a group inc 64 mask block, a mask block inc 16 queues
	{
		tShiftMask = 0xFFFF;
		if (!(tGrpQStCd[i]))
		{
			tQHdInx = tQHdInx + 16;
			continue;
		}

		//tGrpQStCd[i] !=0
		tTstGrpQStCd = tGrpQStCd[i];
		tQuickRet = tGrpQStCd[i];

		for (j = 0; j < 16; j++) //a mask block inc 16 bits used for 16 queues
		{
			if (!tQuickRet)
				break;

			tShiftMask = tShiftMask << 1;//1111 1111 1111 111[0]b
			tQuickRet = tGrpQStCd[i] & tShiftMask;//xxxx xxxx xxxx xxx[0]b
			tTstGrpQStCd = tGrpQStCd[i] & (~tShiftMask);//0000 0000 0000 000[x]b
			if (!tTstGrpQStCd)//0000 0000 0000 000[0]b, go on!
			{
				tQHdInx++;//nxt bit no.
				continue;
			}

			//0000 0000 0000 000[1]b
			//fix position
			tGrpQHd_p = tGrpQHd_p + tQHdInx;//pop q header
			tQhdUseCnt += tGrpQHd_p->usedBlkNum;//compute total using block num.

			//check boundary
			if (tQhdUseCnt > tGrpQ_p->qUsdUTCnt || tGrpQHd_p >= (tGrpQ_p->grpQHd_p + tGrpQ_p->grpQNum))
			{
				return -4;
			}

			if (tGrpQHd_p->usedBlkNum)//target Q exsit data block, match with grpQStCd.lgGrpQStMk[i]
			{
				//check context
				if (tGrpQHd_p->qCnxt_p && !(*(tGrpQHd_p->qCnxt_p)))//context disable, pop info has prepared, and go on!
				{
					if (!findFlag)//first find
					{
						//prepare pop info.
						*qHd_pp = tGrpQHd_p;
						*qRef_p = i;//0~63
						*grpQMk_p = ~((u_short)1 << j);//set pop Q. reset mask
						findFlag = 1;//set find flag
					}
					tQHdInx++;//nxt bit no. to find next no context or context enable Q data block.
					tGrpQHd_p = tGrpQ_p->grpQHd_p;//restore tGrpQHd_p to Group queue head
					continue;
				}

				//prepare pop info.
				*qHd_pp = tGrpQHd_p;
				*qRef_p = i;//0~63
				*grpQMk_p = ~((u_short)1 << j);//set pop Q. reset mask
				return 1;//data can pop
			}
			else//at fix position, target Q no data block, dismatch with grpQStCd.lgGrpQStMk[i]
			{
				return -4;
			}

			//boudary check
			if (tQHdInx >= tGrpQ_p->grpQNum)
				return -4;
		}//erro and able pop have return
	}
		//find nothing and find but context disable (j = 16) reach here(i = 64)!
		if (findFlag)
			return 0;
		return -4;//i = 64
}

//
//
////////////////���캯��
////���쵥�����
//���ܣ�����������С������¼����ٽ�������������ж��пռ䡣�����Ϳ���Ϊ�����С��
//���룺
//      fstGrpPar�����鴴������
//�������
//���أ���
//
C_DuGrp_BCPQ::C_DuGrp_BCPQ ( crtGrpQPar fstGrpPar )
{
    qHd* tQHd_p = NULL;
    int tRtn;

    //����ֵ
    qGrpNum = 0;
    //evtMem = (HANDLE)0xFFFFFFFF;
    fstGrpQ.grpQtype = fstGrpPar.grpQtype;
    fstGrpQ.grpQNum = fstGrpPar.grpQNum;
    fstGrpQ.grpQULng = fstGrpPar.grpQULng;
    fstGrpQ.grpQVol = fstGrpPar.grpQVol;
    fstGrpQ.qUsdUTCnt = 0;
    memset ( ( void* ) ( &fstGrpQ.grpQStCd ), 0, 128 );//128*8=1024λ

    fstGrpQ.Ev_Nfy = ( T_EVT ) 0;
    //  fstGrpQ.grpQKey = NULL;
    fstGrpQ.grpQHd_p = NULL;

    //�����¼��ͻ�����
	/*code walkthroug*/	evtMem[0] = (T_EVT)0xFFFFFFFF;
#ifndef WINDOWS_PLATFORM
	/*code walkthrough*///int initNum = 0xFFFFFFFF;
    /*code walkthrough*/evtMem[0] = &evt_Vlu[0];//evtMem[0] = * ( T_EVT* ) &initNum;
	/*code walkthrough*/fstGrpQ.Ev_Nfy = evtMem[0];//fstGrpQ.Ev_Nfy = &this->evt_Vlu[0];
#endif

	/*code walkthrough*///if ( crtEvt ( & ( fstGrpQ.Ev_Nfy ), &evtMem[0] ) == -1 )
	if (crtEvt(evtMem[0]) == -1)
	{
        return;
    }

	/*code walkthrough*/// this->evtMem[0] = fstGrpQ.Ev_Nfy;//[0]-fst
    crtQLock ( fstGrpQ.grpQKey );
	fstGrpQ.holdEvtFlag = 0;
	fstGrpQ.evtSnd = 0;
	fstGrpQ.evtRev = 0;

    ////////������пռ�
    //����ͷ
    tRtn = creatQHd ( fstGrpQ.grpQtype, fstGrpQ.grpQNum, &tQHd_p );

    if ( tRtn <= 0 )
    {
        fstGrpQ.grpQNum = 0;
        fstGrpQ.grpQULng = 0;
        fstGrpQ.grpQVol = 0;

        if ( tRtn < 0 )
        {
            fstGrpQ.grpQtype = 0;
            return;
        }

    }
	//��������
	if ( tRtn )
    {
        tRtn = creatQBody ( tQHd_p, fstGrpQ.grpQNum, fstGrpQ.grpQVol, fstGrpQ.grpQULng );
    }

    fstGrpQ.grpQHd_p = tQHd_p;
    qGrpNum = 1;
}

//
//
////����˫�����
//���ܣ�����˫����С�����˫�¼���˫�ٽ������˫�����ж��пռ�
//���룺
//      fstGrpPar��1�鴴������
//      scdGrpPar��2�鴴������
//�������
//���أ���
//
C_DuGrp_BCPQ::C_DuGrp_BCPQ ( crtGrpQPar fstGrpPar, crtGrpQPar scdGrpPar )
{
    grpQ* tGrpQ_p;
    crtGrpQPar* tCrtGrpQPar_p;
	qHd* tQHd_p = NULL;
    qHd** tQHd_pp = &tQHd_p;
    int tRtn;

    if(( 1 != scdGrpPar.grpQtype) && (2 != scdGrpPar.grpQtype))
    {
        return;
    }

    qGrpNum = 0;
    fstGrpQ.holdEvtFlag = 0;//fstG no evt!for lnx!
	scdGrpQ.holdEvtFlag = 0;//scdG no evt!for lnx!
    fstGrpQ.qUsdUTCnt = 0;
	scdGrpQ.qUsdUTCnt = 0;
	fstGrpQ.evtSnd = 0;
	scdGrpQ.evtSnd = 0;
	fstGrpQ.evtRev = 0;
	scdGrpQ.evtRev = 0;
	memset((void*)(&fstGrpQ.grpQStCd), 0, 128);//128*8=1024λ
	memset((void*)(&scdGrpQ.grpQStCd), 0, 128);//128*8=1024λ

	fstGrpQ.Ev_Nfy = (T_EVT)0;
	scdGrpQ.Ev_Nfy = (T_EVT)0;
	/*code walkthroug*/	evtMem[0] = (T_EVT)0xFFFFFFFF;
	/*code walkthroug*/	evtMem[1] = (T_EVT)0xFFFFFFFF;
#ifndef WINDOWS_PLATFORM
	/*code walkthroug*/evtMem[0] = &evt_Vlu[0];//	evtMem[0] = (T_EVT)(0xFFFFFFFF);
	/*code walkthroug*/evtMem[1] = &evt_Vlu[1];//evtMem[1] = (T_EVT)(0xFFFFFFFF);
	/*code walkthroug*/fstGrpQ.Ev_Nfy = evtMem[0];//fstGrpQ.Ev_Nfy = &evt_Vlu[0];
	/*code walkthroug*/scdGrpQ.Ev_Nfy = evtMem[1];//scdGrpQ.Ev_Nfy = &evt_Vlu[1];
#endif
	fstGrpQ.grpQHd_p = NULL;
	scdGrpQ.grpQHd_p = NULL;

    //����ֵ
	int i = 2;
	while( i )
    {
        if ( 1 == i ) //fstG
        {
            tGrpQ_p = &fstGrpQ;
            tCrtGrpQPar_p = &fstGrpPar;
        }
        else//scdG
        {
            tGrpQ_p = &scdGrpQ;
            tCrtGrpQPar_p = &scdGrpPar;
        }

		tGrpQ_p->grpQtype = tCrtGrpQPar_p->grpQtype;
		tGrpQ_p->grpQNum = tCrtGrpQPar_p->grpQNum;
		tGrpQ_p->grpQULng = tCrtGrpQPar_p->grpQULng;
		tGrpQ_p->grpQVol = tCrtGrpQPar_p->grpQVol;

		//�����¼��ͻ�����
		/*code walkthrough*///if (crtEvt(&(tGrpQ_p->Ev_Nfy), &evtMem[i - 1]) == -1) //lnx-sem_t, win-handle
		/*code walkthrough*/if(crtEvt(evtMem[i - 1]) == -1)
		{
			return;
		}
		/*code walkthrough*///evtMem[i - 1] = tGrpQ_p->Ev_Nfy; //[0]-fst;[1]--scd
		crtQLock(tGrpQ_p->grpQKey);//[0]-fst;[1]--scd
        --i;

        ////////������пռ�
        //����ͷ
        tRtn = creatQHd ( tGrpQ_p->grpQtype, tGrpQ_p->grpQNum, tQHd_pp );
        if ( tRtn <= 0 )
        {
            tGrpQ_p->grpQNum = 0;
            tGrpQ_p->grpQULng = 0;
            tGrpQ_p->grpQVol = 0;

            if ( tRtn < 0 )
            {
                tGrpQ_p->grpQtype = 0;
                return;
            }
        }

        //������
        if ( tRtn )
        {
            tRtn = creatQBody ( tQHd_p, tGrpQ_p->grpQNum, tGrpQ_p->grpQVol, tGrpQ_p->grpQULng );
        }
        tGrpQ_p->grpQHd_p = tQHd_p;
        qGrpNum++;
    }
}

//
//
////////////////��������
////
//���ܣ���ٵ���˫�¼����ٽ����ͷŶ��пռ�
//���룺��
//�������
//
C_DuGrp_BCPQ::~C_DuGrp_BCPQ()
{
    qHd* tQHd_p;
	if (1 == qGrpNum)
	{
		/*code walkthrough*/freeEvt(fstGrpQ.Ev_Nfy);//freeEvt(fstGrpQ.Ev_Nfy, evtMem[0]);
		/*code walkthrough*/fstGrpQ.Ev_Nfy = (T_EVT)0;
		/*code walkthrough*/evtMem[0] = (T_EVT)0xFFFFFFFF;
		freeQLock(fstGrpQ.grpQKey);
		if (fstGrpQ.grpQNum)
		{

			for (fstGrpQ.grpQNum, tQHd_p = fstGrpQ.grpQHd_p; fstGrpQ.grpQNum > 0; fstGrpQ.grpQNum--, tQHd_p++)
			{
				free(tQHd_p->qBody_p);
			}

			free(fstGrpQ.grpQHd_p);
		}
	}
	if (2 == qGrpNum)
	{
		/*code walkthrough*/freeEvt(fstGrpQ.Ev_Nfy);//freeEvt(fstGrpQ.Ev_Nfy, evtMem[0]);
		/*code walkthrough*/fstGrpQ.Ev_Nfy = (T_EVT)0;
		/*code walkthrough*/evtMem[0] = (T_EVT)0xFFFFFFFF;
		freeQLock(fstGrpQ.grpQKey);

		/*code walkthrough*/freeEvt(scdGrpQ.Ev_Nfy);//freeEvt(scdGrpQ.Ev_Nfy, evtMem[1]);
		/*code walkthrough*/scdGrpQ.Ev_Nfy = (T_EVT)0;
		/*code walkthrough*/evtMem[1] = (T_EVT)0xFFFFFFFF;
		freeQLock(scdGrpQ.grpQKey);

		if (fstGrpQ.grpQNum)
		{

			for (fstGrpQ.grpQNum, tQHd_p = fstGrpQ.grpQHd_p; fstGrpQ.grpQNum > 0; fstGrpQ.grpQNum--, tQHd_p++)
			{
				free(tQHd_p->qBody_p);
			}

			free(fstGrpQ.grpQHd_p);
		}
		if (scdGrpQ.grpQNum)
		{

			for (scdGrpQ.grpQNum, tQHd_p = scdGrpQ.grpQHd_p; scdGrpQ.grpQNum > 0; scdGrpQ.grpQNum--, tQHd_p++)
			{
				free(tQHd_p->qBody_p);
			}
			free(scdGrpQ.grpQHd_p);
		}

	}
}

//
//
////////////////���ö��еĳ���������״̬
////
//���ܣ����ö��еĳ���������״̬��ָʾ��Ӧ�Ķ�����ݿ�����Ƿ����������ĵ�ȡֵҪ��
//���룺grpId����Id��=1����1�飻=2����2��
//      qIdx����������=0xFFFF FFFF������ȫ��
//      cnxt_p������������״ָ̬��
//�������
//���أ�=0���ɹ���=-1���������=-2��ʧ��
//
int C_DuGrp_BCPQ::setCnxtQ(u_char grpId, u_short qIdx, void* cnxt_p)
{
	grpQ* tGrpQ_p;
	qHd* tGrpQHd_p;
	u_short tNum;

	//������
	if ((1 != grpId) && (2 != grpId))
	{
		return -1;
	}

	if (!cnxt_p)
	{
		return -1;
	}

	if (1 == grpId)
	{
		tGrpQ_p = &fstGrpQ;
	}
	else
	{
		tGrpQ_p = &scdGrpQ;
	}

	if ((qIdx > tGrpQ_p->grpQNum) && (qIdx != 0xFFFFFFFF)) //max qId = grpQNum-1
	{
		return -1;
	}

	//���ã����Ტ�У�������
	if (0xFFFFFFFF == qIdx)
	{
		tNum = tGrpQ_p->grpQNum;
		tGrpQHd_p = tGrpQ_p->grpQHd_p;
		do
		{
			tGrpQHd_p->qCnxt_p = (u_char*)cnxt_p;
			tGrpQHd_p = tGrpQHd_p++;
		} while (tNum--);
	}
	else
	{
		tGrpQHd_p = tGrpQ_p->grpQHd_p + qIdx;
		tGrpQHd_p->qCnxt_p = (u_char*)cnxt_p;
	}
    return 0;
}

//
//
////////////////��ݿ����
////
//���ܣ���ݻ�������ӣ������¼����������ӣ��������¼�
//���룺
//      grpId��  �����ݿ�ͷ��¼�������Ե���ţ���1��=1��fstGrpQ������2��=2(scdGrpQ)
//               ����ݿ���ӣ����޷����ʱ�������¼�
//      qPrio��  ��ݿ���ӵ����ȼ������ڶ���������ͨ�����飩�ţ���ȡֵ��0��ʼ
//      dBlk_p�� ��ݿ�ָ�룬=NULLʱ�������ӣ������¼�
//      dBlkLng����ݿ鳤��һ�����Ϊ���е�һ����ݵ�Ԫ
//�������
//����ֵ��>=0���ɹ�����������ʱ����ʾ��Ӻ󣬶���ʣ����õ�Ԫ������¼�ʱ����ʾ�¼�������
//        < 0��ʧ�ܡ�=-1���������=-2,�������޷���ӣ�=3��δ֪����
//
int C_DuGrp_BCPQ::pshQ ( u_char grpId, u_short qPrio, char* dBlk_p, u_char dBlkLng )
{
    grpQ* tGrpQ_p;
    qHd* tQhd_p;
    char* tPshPos_p;
    u_short tGrpQStCd;
    int tRtn;

    //������
    if ( ( 1 != grpId ) && ( 2 != grpId ) )
    {
        //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-1]1 \n");
        return -1;
    }

    if ( 1 == grpId )
    {
        tGrpQ_p = &fstGrpQ;
    }
    else
    {
        tGrpQ_p = &scdGrpQ;
    }

    //�����¼����¼��������޶���
    if ( !dBlk_p )
    {
        //lockQ ( tGrpQ_p->grpQKey );
        /*code walkthrough*///sndEvt ( tGrpQ_p->Ev_Nfy, tGrpQ_p );//linux�У���2��ʹ��
		/*code walkthrough*/if (sndEvt(tGrpQ_p))
		/*code walkthrough*/{
		/*code walkthrough*/	//unlockQ(tGrpQ_p->grpQKey);
                                //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-3]9 \n");
		/*code walkthrough*/	return -3;
		/*code walkthrough*/}
		/*code walkthrough*/else
		/*code walkthrough*/{
        /*code walkthrough*/    //unlockQ(tGrpQ_p->grpQKey);
		/*code walkthrough*/	return grpId;
		/*code walkthrough*/}
    }

    if ( qPrio >= tGrpQ_p->grpQNum ) //prio. from 0 to grpQNum-1
    {
         //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-1]2 \n");
        return -1;
    }

    if ( dBlkLng <= 0 || ( tGrpQ_p->grpQULng > 0 && dBlkLng > tGrpQ_p->grpQULng ) )
    {
         //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-1]3 \n");
        return -1;
    }

    ///////////��ݿ���ӣ��ҷ��¼�
    lockQ ( tGrpQ_p->grpQKey );
    tQhd_p = tGrpQ_p->grpQHd_p + qPrio; //find q-head.

    if ( tQhd_p->usedBlkNum >= tGrpQ_p->grpQVol ) //q full
    {
        unlockQ ( tGrpQ_p->grpQKey );
         //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-2]4 \n");
        return -2;
    }

    //���
    tPshPos_p = tQhd_p->qBody_p;
    if ( tPshPos_p )
    {
        tPshPos_p = tPshPos_p + ( tQhd_p->pshQIndx * tGrpQ_p->grpQULng );

        if ( !memcpy ( tPshPos_p, dBlk_p, dBlkLng ) )
        {
            unlockQ ( tGrpQ_p->grpQKey );
             //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-3]5 \n");
            return -3;
        }

        tQhd_p->pshQIndx = ( ++tQhd_p->pshQIndx ) % tGrpQ_p->grpQVol;
        tQhd_p->usedBlkNum++;
        tGrpQ_p->qUsdUTCnt++;

        //���������״̬��
        if ( 1 == tGrpQ_p->grpQtype ) //ltG
        {
            tGrpQStCd = ( u_short ) 1 << qPrio;
            tGrpQ_p->grpQStCd.ltGrpQStMk = tGrpQ_p->grpQStCd.ltGrpQStMk | tGrpQStCd;
        }
        else//lgG
        {
            tGrpQStCd = ( u_short ) 1 << ( qPrio % 16 );
            tGrpQ_p->grpQStCd.lgGrpQStMk[qPrio / 16] = tGrpQ_p->grpQStCd.lgGrpQStMk[qPrio / 16] | tGrpQStCd;
        }
    }
    else//tPshPos_p = NULL, erro. cause unknown!
    {
        unlockQ ( tGrpQ_p->grpQKey );
         //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-3]6 \n");
        return -3;
    }

    tRtn = tGrpQ_p->grpQVol - tQhd_p->usedBlkNum;

    if ( tRtn < 0 )
    {
        unlockQ ( tGrpQ_p->grpQKey );
         //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-3]7 \n");
        return -3;
    }

    unlockQ(tGrpQ_p->grpQKey);
	/*code walkthrough*///unlockQ ( tGrpQ_p->grpQKey );

    //���¼�
	/*code walkthrough*///tGrpQ_p->evtSnd++;
    /*code walkthrough*///sndEvt ( tGrpQ_p->Ev_Nfy, tGrpQ_p );//liunx ��2��ʹ��
	/*code walkthrough*/if(sndEvt(tGrpQ_p))
	/*code walkthrough*/{
	/*code walkthrough*/	//unlockQ(tGrpQ_p->grpQKey);
                             //printf("===fusktest===C_DuGrp_BCPQ::pshQ------return[-3]8 \n");
	/*code walkthrough*/	return -3;
	/*code walkthrough*/}
	/*code walkthrough*///unlockQ ( tGrpQ_p->grpQKey );
    return tRtn;
}

//
//
////////��ݿ����
////
//���ܣ������¼�����ݻ����ݳ��ӣ�����ݳ��ӣ��൱�ڽ������¼������Ӻ�ӿո�λ�¼�
//���룺
//      grpId��  �ȴ���ݺͽ����¼��Ķ����顣��1��=0x1����2��=0x2�������е�����һ��=0x3
//      dBlk_p�� ���ճ�����ݵ�Ԫ����ݿ�ָ�룬=NULLʱֻ��grpId���¼�
//		dBlkLng_p�����ճ�����ݵ�Ԫ����ݿ鳤��
//�����
//      dBlkLng_p��ʵ�ʳ��ӵ���ݵ�Ԫ���ȣ�����ݳ���ʱ��ֵΪ0
//����ֵ��> 0���ɹ�����Ӧ���¼�����������ţ��Ҳ����������ġ�=0x01��1������¼���=0x02��2������¼�
//					 ��Ӧ���¼�����������ţ������������ġ�  =0x11��1������¼���=0x12��2������¼�
//        < 0������ԭ���롣=-1������������=-2,�ȴ�ʱ��=-3���ȴ�ʧ�ܣ�
//                         =-4���ӿմ�����ӿղ����أ���=-5,δ֪ԭ���ʧ��
//                         =0�����зǿգ�������������������
//
int C_DuGrp_BCPQ::popQ ( u_char grpId, char* dBlk_p, u_char* dBlkLng_p )
{
    int tEvtNo;
    grpQ* tGrpQ_p;
    qHd* tGrpQHd_p;
    u_char* tPopQPos;
	u_short* tGrpQStCd_p = NULL;
	u_short tGrpQMk = 0;
	u_char tQRef;
    int tRtn;

    //check parameter
    if((grpId < 1)||(grpId > 3))
    {
        //printf("===fusktest===C_DuGrp_BCPQ::popQ------return[-1]1 \n");
        return -1;
    }

	//only wait for evt
    if ( !dBlk_p )
    {
		/*code walkthrough*///lockQ(fstGrpQ.grpQKey);
		/*code walkthrough*///lockQ(scdGrpQ.grpQKey);
		/*code walkthrough*/tRtn = waitEvt(grpId, this);
		/*code walkthrough*///unlockQ(fstGrpQ.grpQKey);
		/*code walkthrough*///unlockQ(scdGrpQ.grpQKey);
        return tRtn;
    }

    if ( !dBlkLng_p )
    {
       // printf("===fusktest===C_DuGrp_BCPQ::popQ------return[-1]2 \n");
        return -1;
    }

	//lockQ(fstGrpQ.grpQKey);
	//lockQ(scdGrpQ.grpQKey);
    tEvtNo = waitEvt ( grpId, this ); //wait...
	//unlockQ(fstGrpQ.grpQKey);
	//unlockQ(scdGrpQ.grpQKey);

    if ( tEvtNo < 0 )//waitEvt erro!
    {
        ////printf("===fusktest===C_DuGrp_BCPQ::popQ------return[waitEvt erro]3 \n");
        return tEvtNo;
    }

	//check to match evt with wait mode  
	if (((1 == grpId) && (1 != tEvtNo))||((2 == grpId) && (2 != tEvtNo)))
    {
        //printf("===fusktest===C_DuGrp_BCPQ::popQ------return[-5]4 \n");
		return -5;
    }

    if ( 1 == tEvtNo )  //fstG evt
		tGrpQ_p = &fstGrpQ;
	else// 2: scdG evt
		tGrpQ_p = &scdGrpQ;
   if( !tGrpQ_p->grpQVol)//Q no body
   {
	   *dBlkLng_p = 0;
	   return tEvtNo;//0x01/0x02
	}
 
    //prepare get data
    lockQ ( tGrpQ_p->grpQKey );

	//fix pop Q
	if (1 == tEvtNo) //ltG
	{
		tRtn = findPopLtQHd(tGrpQ_p, &tGrpQHd_p, &tGrpQMk); //rtn=0|1|-4
		tGrpQStCd_p = &((tGrpQ_p->grpQStCd).ltGrpQStMk);
	}
	else
	{
		tRtn = findPopLgQHd(tGrpQ_p, &tGrpQHd_p, &tQRef, &tGrpQMk); //rtn=0|1|-4
		tGrpQStCd_p = (tGrpQ_p->grpQStCd).lgGrpQStMk;
		tGrpQStCd_p = tGrpQStCd_p + tQRef;
	}
	if (tRtn < 0)
	{
		unlockQ(tGrpQ_p->grpQKey);
        //printf("===fusktest===C_DuGrp_BCPQ::popQ------return[%d]5 \n",tRtn);
		return tRtn;
	}

	//tRtn=0|1, must pop!
    tPopQPos = ( u_char* ) tGrpQHd_p->qBody_p + tGrpQHd_p->popQIndx * tGrpQ_p->grpQULng;
	tGrpQHd_p->popQIndx = (++tGrpQHd_p->popQIndx) % tGrpQ_p->grpQVol;
	tGrpQHd_p->usedBlkNum--;
	if (!(tGrpQHd_p->usedBlkNum))//reset q state mask
		*tGrpQStCd_p = (*tGrpQStCd_p) & tGrpQMk;
	tGrpQ_p->qUsdUTCnt--;

	if (*dBlkLng_p < tGrpQ_p->grpQULng)//receive Buffer too little��pop but discard data.
	{
		*dBlkLng_p = 0;
		unlockQ(tGrpQ_p->grpQKey);
        //printf("===fusktest===C_DuGrp_BCPQ::popQ------return[-1]6 \n");
		return -1;
	}
    if ( memcpy ( dBlk_p, tPopQPos, tGrpQ_p->grpQULng ) == NULL )
    {
        *dBlkLng_p = 0;
        unlockQ ( tGrpQ_p->grpQKey );
        //("===fusktest===C_DuGrp_BCPQ::popQ------return[-4]7 \n");
        return -4;
    }
    *dBlkLng_p = tGrpQ_p->grpQULng;

    unlockQ ( tGrpQ_p->grpQKey );
	tEvtNo = tEvtNo | (tRtn << 4);
    return tEvtNo;
}

//
//
////////���ָ�������ڵ�ȫ�����
////
//���ܣ�������������Ͷ������ȼ���������ͨ�����飩�ţ�����Ӧ���е�ȫ�����
//���룺
//      grpId��Ҫ�����ݵĶ���������š���1��=0x1����2��=0x2
//      prio�� Ҫ�����ݵĶ������ȼ�
//�����
//      ��
//����ֵ��= 0���ɹ���=-1��ʧ��
//
int C_DuGrp_BCPQ::clrGQ ( u_char grpId, u_short qPrio )
{
    grpQ* tGrpQ_p;
    qHd* tQHd_p;
    u_short tGrpQStCd;
	u_char tEvtNum;
	/*code walkthrough*///T_EVT* tEvt_p;

    //������
    if ( ( 1 != grpId ) && ( 2 != grpId ) )
    {
        return -1;
    }

    if ( 1 == grpId )
    {
        tGrpQ_p = &fstGrpQ;
		/*code walkthrough*///tEvt_p = &fstGrpQ.Ev_Nfy;
    }
    else
    {
        tGrpQ_p = &scdGrpQ;
		/*code walkthrough*///tEvt_p = &scdGrpQ.Ev_Nfy;
    }

    if ( qPrio >= tGrpQ_p->grpQNum ) //prio. from 0 to grpQNum-1
    {
        return -1;
    }

    //////////������
    lockQ ( tGrpQ_p->grpQKey );
    tQHd_p = tGrpQ_p->grpQHd_p + qPrio; //find q-head.
    tGrpQ_p->qUsdUTCnt = tGrpQ_p->qUsdUTCnt - tQHd_p->usedBlkNum;
	tEvtNum = tQHd_p->usedBlkNum;
    tQHd_p->usedBlkNum = 0;
    tQHd_p->pshQIndx = 0;
    tQHd_p->popQIndx = 0;

    //�����״̬��
    if ( 1 == tGrpQ_p->grpQtype ) //ltG
    {
        tGrpQ_p->grpQStCd.ltGrpQStMk = ( tGrpQ_p->grpQStCd.ltGrpQStMk ) & ( ~ ( ( u_short ) 1 << qPrio ) );
    }
    else//lgG
    {
        tGrpQStCd = ~ ( ( u_short ) 1 << ( qPrio % 16 ) );
        tGrpQ_p->grpQStCd.lgGrpQStMk[qPrio / 16] = tGrpQ_p->grpQStCd.lgGrpQStMk[qPrio / 16] & tGrpQStCd;
    }

    if (!tGrpQ_p->qUsdUTCnt) //reset evt!
    {
		/*code walkthrough*/clrEvt(tGrpQ_p);//clrEvt ( tGrpQ_p->Ev_Nfy, tGrpQ_p );
    }
	else//only clear evt for the Q.
	{	
		while (tEvtNum--)
			sem_trywait(tGrpQ_p->Ev_Nfy);
	}

    unlockQ ( tGrpQ_p->grpQKey );
    return 0;
}

//
//
////////���ָ�������������ж��е�ȫ�����
////
//���ܣ������������飬���ָ�������������ж��е�ȫ����ݣ���������¼�
//���룺
//      grpId��Ҫ�����ݵĶ���������š���1��=0x1����2��=0x2
//�����
//      ��
//����ֵ��= 0���ɹ���=-1��ʧ��
//
int C_DuGrp_BCPQ::clrG ( u_char grpId )
{
    grpQ* tGrpQ_p;
    qHd* tQHd_p;
	/*code walkthrough*///T_EVT* tEvt_p;
    int i;

    //������
    if ( ( 1 != grpId ) && ( 2 != grpId ) )
    {
        return -1;
    }

    if ( 1 == grpId )
    {
        tGrpQ_p = &fstGrpQ;
		/*code walkthrough*///tEvt_p = &fstGrpQ.Ev_Nfy;
    }
    else
    {
        tGrpQ_p = &scdGrpQ;
		/*code walkthrough*///tEvt_p = &scdGrpQ.Ev_Nfy;
    }

    lockQ ( tGrpQ_p->grpQKey );
    tGrpQ_p->qUsdUTCnt = 0;

    for ( i = 0; i < 64; i++ )//ltG and lgG same!
    {
        tGrpQ_p->grpQStCd.lgGrpQStMk[i] = 0;
    }

    for ( i = 0, tQHd_p = tGrpQ_p->grpQHd_p; i < tGrpQ_p->grpQNum; tQHd_p++, i++ )
    {
        tQHd_p->usedBlkNum = 0;
        tQHd_p->pshQIndx   = 0;
        tQHd_p->popQIndx   = 0;
    }
	/*code walkthrough*/clrEvt(tGrpQ_p);
    unlockQ ( tGrpQ_p->grpQKey );
    return 0;
}

T_EVT* C_DuGrp_BCPQ::getEvtMem()
{
    return evtMem;
}

grpQ* C_DuGrp_BCPQ::getFstGrpQ()
{
    return &fstGrpQ;
}

grpQ* C_DuGrp_BCPQ::getScdGrpQ()
{
    return &scdGrpQ;
}
