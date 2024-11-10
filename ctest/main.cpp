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
#include <time.h>


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

int my_rund(int min,int max)
{

    //unsigned int seed = (unsigned int)time(NULL);
    //unsigned int seed = 1;
    //

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned int seed = (unsigned int)(ts.tv_sec ^ ts.tv_nsec);

    int ret = min + rand_r(&seed) % (max - min + 1);

    return ret;
}




//生成[1,x)的随机数
// 其中x>=2
double generate_random_1_to_x(double x)
{

    //unsigned int seed = (unsigned int)time(NULL);
    //unsigned int seed = 1;
    //
    struct drand48_data buffer;
    double result;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

	// 初始化随机数生成器的状态
    srand48_r((long)(ts.tv_sec ^ ts.tv_nsec), &buffer);

    // 生成 [0.0, 1.0) 之间的随机数
    drand48_r(&buffer, &result);

    double random_number = 0;
    if ( x < 2 ) x = 2;

    // 将 [0.0, 1.0) 之间的随机数映射到 [1, x) 之间
    random_number = 1 + (x - 1) * result;

    // 四舍五入保留两位小数
    random_number = round(random_number * 100.0) / 100.0;

    return random_number;

}




// 判断 a 与 b 的差值绝对值
// (1) |b - a | >fix_v 时
//    (1.1) 当 b > a 时
//         返回: a + [1,fix_v)的随机值
//    (1.2) 当 b < a 时
//         返回: a - [1,fix_v)的随机值
// (2) |b - a | <= fix_v 时
//     返回 b
//
// 最后实现效果为: | return - a| 不能大于 fix_v
//
double my_ctrl_val( double a, double b, double fix_v )
{
    double d_diff_abs = fabs(b - a);
    double tdval = 0;

    //c_write_log(_DEBUG,"d_diff_abs=[%f]",d_diff_abs);

    if ( d_diff_abs > fix_v ) {
        if ( b > a ) {
            tdval = a + generate_random_1_to_x( fix_v );

        }
        else {
            tdval = a - generate_random_1_to_x( fix_v );

        }

        //c_write_log(_DEBUG,"tdval=[%f]",tdval);

        return tdval;
    }
    else {
       return  b;
    }

   return 0; 
}


double my_str_to_d(const char *str)
{
    char *endptr = NULL;
    double val = strtod( str, &endptr );
    if (endptr == str) {
        c_write_log(_ERROR,"No digits were found in the string[%s].\n", str);
        return 0;
    }

    if (*endptr != '\0') {
        c_write_log(_ERROR,"Trailing characters after the number: '%s', the string is [%s].\n", endptr,str);
        return 0;
    }
    return val;
}


int main(int argc, char *argv[])
{
    //for(int i=0;i<20;i++) {
    //    int ret = my_rund(10,100);
    //    c_write_log(_INFO,"my_rund return val=[%d]",ret );
    //}
    
    ////generate_random_1_to_x
    //for(int i=0;i<20;i++) {
    //    double ret = generate_random_1_to_x(2);
    //    c_write_log(_INFO,"generate_random_1_to_x return val=[%f]",ret );
    //}
    //return 0;

    if ( argc != 4 ){
        c_write_log(_ERROR,"input number not eq 4\n");
        return 1;
    }
    c_write_log(_INFO,"1:[%s]\n",argv[1]);
    c_write_log(_INFO,"2:[%s]\n",argv[2]);
    c_write_log(_INFO,"3:[%s]\n",argv[3]);
    double fst_d = my_str_to_d(argv[1]);
    double scd_d = my_str_to_d(argv[2]);
    int fix_v = atoi(argv[3]);

    double retd = my_ctrl_val(fst_d, scd_d, fix_v );

    c_write_log(_INFO,"fst_d=[%f],scd_d=[%f],fix_v=[%d],retd=[%f],abs=[%f]",fst_d,scd_d,fix_v,retd,fabs(retd-fst_d));
    return 0;

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
