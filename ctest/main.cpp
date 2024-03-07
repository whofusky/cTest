#include <stdio.h>
#include <string.h>
#include "WriteLog.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <unistd.h>
#include <string>
#include <map>
#include <stdlib.h>
#include <sys/syscall.h>
#include "json/json.h"
#include <math.h> // 包含 sin 和 cos 函数


#include "ELFcmd.h"

#include "InfraBase.h"
#include "CustomOutLog.h"

typedef struct{
    double sum_sin;     // 累加 sin 值
    double sum_cos;     // 累加 cos 值
    double average_x;   // 计算平均值的 x 分量
    double average_y;   // 计算平均值的 y 分量
    double average_dir; // 计算平均风向
    unsigned int n;     //值的个数
}T_vetAvg;

void cal_vect_avg_1(const float& inVect, T_vetAvg& calData)
{
    calData.sum_sin += sin(inVect * M_PI / 180); // 累加 sin 值
    calData.sum_cos += cos(inVect * M_PI / 180); // 累加 cos 值
    calData.n++;

    calData.average_x = calData.sum_sin / calData.n; // 计算平均值的 x 分量
    calData.average_y = calData.sum_cos / calData.n; // 计算平均值的 y 分量
    calData.average_dir = atan2(calData.average_y, calData.average_x) * 180 / M_PI; // 计算平均风向
    if ( calData.average_dir < 0 ) calData.average_dir += 360;

    return;
}

void cal_vect_avg_2(const float& inVect, T_vetAvg& calData)
{
    calData.sum_sin += sin(inVect * M_PI / 180); // 累加 sin 值
    calData.sum_cos += cos(inVect * M_PI / 180); // 累加 cos 值
    calData.n++;

    calData.average_x = calData.sum_cos / calData.n; // 计算平均值的 x 分量
    calData.average_y = calData.sum_sin / calData.n; // 计算平均值的 y 分量
    calData.average_dir = atan2(calData.average_y, calData.average_x) * 180 / M_PI; // 计算平均风向
    if ( calData.average_dir < 0 ) calData.average_dir += 360;

    return;
}

std::string::size_type  getStrSpltVal(std::string &inS, 
        std::string &retS,
        const char* delimiter,
        std::string::size_type& head)
{
    if ( head == inS.npos ) return inS.npos;

    std::string::size_type tail = inS.find( delimiter, head );
    if ( tail != inS.npos ) {
        retS = inS.substr( head, tail - head );
        head = tail + 1;
        return 1;
    }
    retS = inS.substr( head );
    head = inS.npos;
    return 1;
}

int main(int argc, char *argv[])
{
    unsigned long long u64=12345;
    c_write_log(_INFO,"argc=[%d],u64=[%llu]", argc,u64);

    pid_t tid;
    tid = syscall(SYS_gettid);
    c_write_log(_INFO,"thread id[%d]", tid);

    float tf;
    T_vetAvg cal_1 = {0};
    T_vetAvg cal_2 = {0};

    //std::string ins="111,222,,333,,11";
    std::string ins="111,";
    std::string rets;
    std::string::size_type head = 0;
    while( getStrSpltVal(ins, rets, ",", head) != ins.npos ) {
        c_write_log(_DEBUG,"ins[%s],rets[%s]",ins.c_str(), rets.c_str() );
    } 

    //std::string ts1="busi.xml";
    std::string ts1="busi";
    std::string ts2= ts1.substr(0,ts1.rfind(".")) + ".tmp";
    c_write_log(_DEBUG,"ts1=[%s],ts2=[%s]",ts1.c_str(),ts2.c_str());

    return 0;

    std::string tmpVal = "30";
    if ( argc > 1 )
    {
        for( int i = 1; i < argc; i++ )
        {
            tmpVal = argv[i];
            tf = std::stof( tmpVal );
            cal_vect_avg_1( tf, cal_1 );
            cal_vect_avg_2( tf, cal_2 );
            c_write_log(_DEBUG,"i=[%d],wd[%d]=[%lf],avg1=[%lf]", i,i,tf, cal_1.average_dir );
            c_write_log(_DEBUG,"i=[%d],wd[%d]=[%lf],avg2=[%lf]", i,i,tf, cal_2.average_dir );
            c_write_log(_DEBUG,"------------------------------------------------------------------------------");
        }
    }
    else
    {
        tf = std::stof( tmpVal );
        cal_vect_avg_1( tf, cal_1 );
        cal_vect_avg_2( tf, cal_2 );
        c_write_log(_DEBUG,"wd=[%lf],avg1=[%lf]", tf, cal_1.average_dir );
        c_write_log(_DEBUG,"wd=[%lf],avg2=[%lf]", tf, cal_2.average_dir );
    }


    return 0;
}
