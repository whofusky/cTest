#pragma once
#include <stdlib.h>



union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};


class IPCShmMemComm
{
private:
    int shmKeyWrite;			//写端shm key（用于建立shm）进程锁key可以与shmkey共用同一个key
    int shmIdWrite;				//写端shm ID
    int lockIdWrite;			//写端进程锁的标识 ID
    void * shmAddrWrite;		//写端shm 地址

    int shmKeyRead;				//读端shm key（用于建立shm）进程锁key可以与shmkey共用同一个key
    int shmIdRead;				//读端shm ID
    int lockIdRead;				//读端进程锁的标识 ID
    void * shmAddrRead;			//读端shm 地址

    int timeOut;				//读写超时时间 单位秒 若传入0则默认2s

    //建立写端共享内存
    int IPCInitWriteMem(void);
    //建立写端进程锁
    int IPCInitWriteLock(void);
    //写端加锁
    int IPCWriteLock(void);
    //写端解锁
    int IPCWriteUnLock(void);

    //建立读端共享内存
    int IPCInitReadMem(void);
    //建立读端进程锁
    int IPCInitReadLock(void);
    //读端加锁
    int IPCReadLock(void);
    //读端解锁
    int IPCReadUnLock(void);

    //PV操作 加锁/解锁
    int semPV(int semId, int op);

public:
    /*	构造
    _shmKeyWrite	写端共享内存标识
    _shmSizeWrite	写端共享内存大小固定 不用应用层指定
    _shmKeyRead		读端共享内存标识
    _shmSizeRead	读端共享内存大小固定 不用应用层指定
    */
    IPCShmMemComm(const int _shmKeyWrite, const int _shmKeyRead, const int _timeOut)
    {
        shmIdWrite = 0;
        shmAddrWrite = NULL;
        shmIdRead = 0;
        shmAddrRead = NULL;

        shmKeyWrite = _shmKeyWrite;
        shmKeyRead = _shmKeyRead;

        if(_timeOut == 0)
        {
            timeOut = 2;
        }
        else
        {
            timeOut = _timeOut;
        }
    }
    ~IPCShmMemComm();

    //建立读写2端共享内存
    //建立读写2个锁
    int IPCInitShmMem(void);

    /*	IPC通信主动模式，从写数据开始
    rowData		传入的原始数据
    返回值	0正常   负数是错误码
    */
    int IPCCommunicationActive(void *structSet, int structLen, unsigned char controlCode, int srcAppCode, int dstAppCode, int commType,
        DATEFORMAT_T startTime, DATEFORMAT_T endTime, unsigned int step, DATEFORMAT_T forcastTime);

    /*	IPC通信被动模式，从读数据开始
    rowData			传出的原始数据
    返回值		0正常   负数是错误码
    */
    int IPCCommunicationPassive(void *SCADASet, int *structLen, unsigned char*controlCode, int srcAppCode, int *dstAppCode, int *commType,
        DATEFORMAT_T* startTime, DATEFORMAT_T* endTime, unsigned int* step, DATEFORMAT_T* forcastTime);
};
