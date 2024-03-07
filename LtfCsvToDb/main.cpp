#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ELFcmd.h"
#include "CustomOutLog.h"
#include "csv.h"


#define  LTF_MAX_FIELD_LEN   25  //长期发电量预测csv中内容字段最大长度
#define  LTF_FIELD_NUM_ONER  4   //长期发电量预测csv中内容一行的字段数量
#define  LTF_REALLOC_ONENUM 100  //realloc分配空间时,每次新增加的记录的个数


//定义csv解析数据结构一个字段的值
typedef struct
{
    unsigned char val[LTF_MAX_FIELD_LEN+1];
}T_Ltf_Field;

//定义csv解析数据结构一行数据的值
typedef struct
{
    T_Ltf_Field field[LTF_FIELD_NUM_ONER];
}T_Ltf_Record;


//定义csv解析数据相关结构
typedef struct
{
    unsigned char  drop_line_flag; //丢弃当前行标识:1 表示当前行数据严重不合格,丢弃当前行
    unsigned int   skip_line_num;  //解析csv时跳过开头的行数
    unsigned int   tmp_row_counter;//临时用于记录解析csv文件时当前所处的行号(值从1开始)
    unsigned int   field_index;    //解析某一行时记录，当前处于哪一个字段(值从0开始)
    unsigned int   record_pos;     //当前record_buf[]的有效内容的最后的index(值从0开始)
    unsigned int   record_size;    //当前record_buf[]中最大容量
    T_Ltf_Record * record_buf;     //存储解析后的csv内容
}T_Ltf_Csv_Data;


//存储标量平均值结构
typedef struct
{
    double sum;      //和
    unsigned int n;  //累计值个数
    double avg;      //平均值
}T_ScalarAvg;


//存储矢量(风向)平均值结构
typedef struct
{
    double sum_sin;   //sin 值和
    double sum_cos;   //cos 值和
    unsigned int n;   //累计值个数
    double average_x; //用于存储平均值的 x 分量
    double average_y; //用于存储平均值的 y 分量
    double avg;       //平均值
}T_VectorAvg;

//预测批次记录结构(不包含入库时间)
typedef struct
{
    char src[4+1];                    //数据来源
    char dd_stt[LTF_MAX_FIELD_LEN+1]; //数据开始日志
    char dd_stp[LTF_MAX_FIELD_LEN+1]; //数据结束日志
    char dd_fc[4+1];                  //数据频率:1h,6h,1d
}T_Ltf_Batch;

//从csv解析并算出的日预测电量值(一行值)
typedef struct
{
    char dd[LTF_MAX_FIELD_LEN+1];      //数据时间
    double      ge;                    //电量(根据小时预测求和的值)
    T_ScalarAvg ws;                    //风速(根据小时预测算的平均值)
    T_VectorAvg wd;                    //风向(根据小时预测算的平均值)
}T_Ltf_Day_Val;


//从csv解析并算出的日预测电量统计结构(多行值)
typedef struct
{
    unsigned int   record_pos;     //当前record_buf[]的有效内容的最后的index(值从0开始)
    unsigned int   record_size;    //当前record_buf[]中最大容量
    T_Ltf_Day_Val* record_buf;     //存储解析后的csv内容
}T_Ltf_Day_Cal;




// 求标量的平均值
static void alg_scalar_average(T_ScalarAvg &s_avg,char* in_val)
{
    if ( NULL == in_val ) return;

    double tval = strtod( in_val, NULL);
    s_avg.sum += tval;
    s_avg.n++;
    s_avg.avg = s_avg.sum / s_avg.n;

    return;
}


// 矢量(风向)求平均
static void alg_vector_average(T_VectorAvg &v_avg,char* in_val)
{
    if ( NULL == in_val ) return;

    //sin cos atan2 M_PI 在math.h头文件中定义

    double tval = strtod( in_val, NULL);

    v_avg.sum_sin += sin(tval * M_PI / 180); // 累加 sin 值
    v_avg.sum_cos += cos(tval * M_PI / 180); // 累加 cos 值
    v_avg.n++;
    v_avg.average_x = v_avg.sum_sin / v_avg.n; // 计算平均值的 x 分量
    v_avg.average_y = v_avg.sum_cos / v_avg.n; // 计算平均值的 y 分量
    v_avg.avg = atan2(v_avg.average_y, v_avg.average_x) * 180 / M_PI; // 计算平均矢量(风向)
    
    while( v_avg.avg < 0 ) //对负值进行修正
    {
        v_avg.avg += 360;
    }

    return;
}


//求和
static void alg_sum(double &out_val,char* in_val)
{
    if ( NULL == in_val ) return;

    double tval = strtod( in_val, NULL);
    out_val += tval;

    return;
}


//测试算法
int test_alg()
{
    char tscalar_a[][10]={ "2.34","1.21","3.4"};
    char tvector_a[][10]={"60.0","60.2","62.1"};
    char tsum_a[][10]={ "2.34", "4.54", "6.45" };

    //static void alg_scalar_average(T_ScalarAvg &s_avg,char* in_val)
    //static void alg_vector_average(T_VectorAvg &v_avg,char* in_val)
    //static void alg_sum(double &out_val,char* in_val)

    T_ScalarAvg s_avg; memset( &s_avg, 0, sizeof( s_avg ) );
    T_VectorAvg v_avg; memset( &v_avg, 0, sizeof( v_avg ) );
    double vsum = 0;

    c_write_log(_DEBUG,"ini:scalar sum[%f],n[%u],avg[%f]", 
            s_avg.sum, s_avg.n, s_avg.avg );
    c_write_log(_DEBUG,"ini:vector sum_sin[%f],sum_cos[%f],n[%u],"
            "average_x[%f],average_y[%f],avg[%f]",
            v_avg.sum_sin, v_avg.sum_cos, v_avg.n,
           v_avg.average_x, v_avg.average_y, v_avg.avg );
    c_write_log(_DEBUG,"ini: sum[%f]\n", vsum );
    for ( int i = 0; i < 3; i++ )
    {
        alg_scalar_average( s_avg, tscalar_a[i]);
        c_write_log(_DEBUG,"scalar[%d] add [%s],sum[%f],n[%u],avg[%f]",
                i, tscalar_a[i],s_avg.sum, s_avg.n, s_avg.avg );

        alg_vector_average( v_avg, tvector_a[i]);
        c_write_log(_DEBUG,"vector[%d] add [%s],sum_sin[%f],sum_cos[%f],n[%u],"
                "average_x[%f],average_y[%f],avg[%f]",
                i, tvector_a[i], v_avg.sum_sin, v_avg.sum_cos, v_avg.n,
               v_avg.average_x, v_avg.average_y, v_avg.avg );

        alg_sum( vsum, tsum_a[i] );
        c_write_log(_DEBUG,"tosum[%d] add [%s],sum[%f]\n",
                i, tscalar_a[i], vsum );
    }

    return 0;
}




//释放T_Ltf_Csv_Data->record_buf
static void free_LtfCsvData( T_Ltf_Csv_Data * d_p )
{
    if ( NULL == d_p ) return;
    if ( NULL != d_p->record_buf )
    {
        free( d_p->record_buf );
        d_p->record_buf = NULL;
    }
    d_p->record_size = 0;
    return;
}



//打印T_Ltf_Csv_Data变量
static void print_LtfCsvData( T_Ltf_Csv_Data * d_p )
{
    if ( NULL == d_p ) return;

    c_write_log(_INFO,"drop_line_flag=[%d],skip_line_num=[%u],tmp_row_counter=[%u],"
            "field_index=[%u],record_pos=[%u],record_size=[%u]",
            d_p->drop_line_flag,d_p->skip_line_num,d_p->tmp_row_counter,
            d_p->field_index,d_p->record_pos,d_p->record_size);

    //fuskytest add
    //return;

    //解析后的数据内容
    unsigned int i = 0;
    for( i = 0; i < d_p->record_pos; i++ )
    {
        T_Ltf_Record* tp = &(d_p->record_buf[i]);

        c_write_log(_DEBUG,"row[%4u]:[%s,%s,%s,%s]",
                i,
                tp->field[0].val,
                tp->field[1].val,
                tp->field[2].val,
                tp->field[3].val
                );
        //double vGeAvg = strtod( (char*)tp->field[1].val, NULL);
        //double vWs = strtod( (char*)tp->field[2].val, NULL);
        //double vWd = strtod( (char*)tp->field[3].val, NULL);
        //c_write_log(_DEBUG,"row[%4u]:[%s,%f,%f,%f]",
        //        i,
        //        tp->field[0].val,
        //        vGeAvg,
        //        vWs,
        //        vWd
        //        );
    }
    return;
}


//判断T_Ltf_Csv_Data 的 record_buf 空间，不足时realloc分配足够的空间
static int record_increase_buffer( T_Ltf_Csv_Data * d_p )
{
    if ( NULL == d_p ) return -1;

    if( (d_p->record_pos + 1) >= d_p->record_size )
    {
        T_Ltf_Record* trecord_p = (T_Ltf_Record*) realloc(
                d_p->record_buf, 
                (d_p->record_size + LTF_REALLOC_ONENUM) * sizeof(T_Ltf_Record) 
                );
        if ( NULL == trecord_p )
        {
            return -2;
        }
        memset( trecord_p + d_p->record_size,
                0,
                LTF_REALLOC_ONENUM * sizeof(T_Ltf_Record) );

        d_p->record_buf  = trecord_p;
        d_p->record_size += LTF_REALLOC_ONENUM;
    }
    return 0;
} 




//cb1 is callback function that will be called from csv_parse() 
//    after an entire field has been read.
void cb1 (void *s, size_t len, void *data) 
{
    //如果记录结构为空则不解析field
    if ( NULL == data )
    { 
        c_write_log(_ERROR,"input parm data=NULL" );
        return;
    }

    T_Ltf_Csv_Data* d_p = (T_Ltf_Csv_Data*) data;
    //如果reord_buf为空则不解析field
    if ( NULL == d_p->record_buf ) return;


    //如果当前行是要丢弃的则不解析field
    if ( 1 == d_p->drop_line_flag ) return;

    //如果设置了需要跳过的开头行数,且当前所解析的行号小于要跳过的行数则不解析field
    if ( d_p->skip_line_num > 0 && d_p->tmp_row_counter <= d_p->skip_line_num )
    {
        return;
    }

    //要解析的field 数超过了 LTF_FIELD_NUM_ONER 则不解析field
    if ( ( d_p->field_index +1 ) > LTF_FIELD_NUM_ONER )
    {
        return;
    }

    if ( NULL == s )
    { 
        c_write_log(_ERROR,"tmp_row_counter=[%u],field_index=[%u],"
                "record_pos=[%u], func cb1 input para s=NULL",
                d_p->tmp_row_counter, d_p->field_index,
                d_p->record_pos );
        return;
    }

    T_Ltf_Field* tfd_p = &(d_p->record_buf[d_p->record_pos].field[d_p->field_index]);
    memset( tfd_p, 0 , sizeof(tfd_p->val));
    if( 0 == len )
    {
        //c_write_log(_DEBUG,"tmp_row_counter=[%u],field_index=[%u],"
        //        "record_pos=[%u], len==0",
        //        d_p->tmp_row_counter, d_p->field_index,
        //        d_p->record_pos );

        //如果第一个字段(时间)为空则标记当前行应该为丢弃行,此行将不再解析
        if( 0 == d_p->field_index )
        {
            d_p->drop_line_flag = 1;
            c_write_log(_ERROR,"tmp_row_counter=[%u],field_index=[%u],"
                    "record_pos=[%u], field val is blank,will set drop_line_flag=1",
                    d_p->tmp_row_counter, d_p->field_index,
                    d_p->record_pos );
        }
        else //其他字段为空默认赋值为"0"
        {
            tfd_p->val[0] = 0x30; //"0"
        }

    }
    else
    {
        size_t tlen = len;

        //copy field的长度不能超过定义的LTF_MAX_FIELD_LEN
        if ( tlen > LTF_MAX_FIELD_LEN ) tlen = LTF_MAX_FIELD_LEN;
        memcpy( tfd_p->val, s, tlen);
    }

    d_p->field_index++;

    return;
}




//cb2 is callback function that will be called from csv_parse()
//    when the end of a record is encountered,
void cb2 (int c, void *data) 
{
    if ( NULL == data ) return;


    T_Ltf_Csv_Data* d_p = (T_Ltf_Csv_Data*) data;

    //如果设置了需要跳过的开头行数,且当前所解析的行号小于要跳过的行数则 record_pos不变
    if ( d_p->skip_line_num > 0 && d_p->tmp_row_counter <= d_p->skip_line_num )
    {
        d_p->drop_line_flag = 0;
        //d_p->record_pos++;
    }
    else if ( 1 == d_p->drop_line_flag ) //丢弃的行record_pos值不变 
    {
        d_p->drop_line_flag = 0;
        //d_p->record_pos++;
    }
    else //其他情况record_pos需要变
    {
        d_p->drop_line_flag = 0;
        d_p->record_pos++;  //将要存储解析行的pos index
    }
    
    d_p->field_index = 0;   //下一行的field index重置
    d_p->tmp_row_counter++; //将要解析的下一行号

    //判断T_Ltf_Csv_Data 的 record_buf 空间，不足时realloc分配足够的空间
    int ret = record_increase_buffer( d_p );
    if ( ret != 0 )
    {
        c_write_log(_ERROR,"record_increase_buffer return[%d]", ret);
        return;
    }

    //将下行将要赋值的解析行赋空
    memset( &(d_p->record_buf[d_p->record_pos]), 0, sizeof(T_Ltf_Record) );

    return;
}




//在使用过的T_Ltf_Csv_Data结构中进行reset 重置操作
static void reInitLtfCsvData( T_Ltf_Csv_Data* p, unsigned int * skip_line_num=NULL )
{
    if ( NULL == p ) return;

    p->drop_line_flag = 0; //丢弃当前行标识:1 表示当前行数据严重不合格,丢弃当前行

    //解析csv时跳过开头的行数
    if ( skip_line_num != NULL )
    {
        p->skip_line_num = *skip_line_num;
    }
    else
    {
        p->skip_line_num = 0;
    }

    p->tmp_row_counter = 1; //临时用于记录解析csv文件时当前所处的行号(值从1开始)
    p->field_index     = 0;//解析某一行时记录，当前处于哪一个字段(值从0开始)
    p->record_pos      = 0;//当前record_buf[]的有效内容的最后的index(值从0开始)

    // record_size 不能进行修改
    //p->record_size     = 0;   //当前record_buf[]中最大容量

    // record_buf 不能进行修改
    //p->record_buf      = NULL;//存储解析后的csv内容

    return;
}




//解析ELF csv文件数据到T_Ltf_Csv_Data结构中去
//   skip_line_num: 解析csv文件时需要跳过文件头的行数
static int parse_Ltf_csv_file( 
        const char* csv_file, 
        T_Ltf_Csv_Data &ltfcsvdata,  
        unsigned int skip_line_num = 1 
        )
{
    reInitLtfCsvData( &ltfcsvdata, &skip_line_num );
    //print_LtfCsvData( &ltfcsvdata);
    record_increase_buffer( &ltfcsvdata );
    //print_LtfCsvData( &ltfcsvdata);

    FILE  *fp = NULL;
    int   ret = 0;
    size_t bytes_read = 0;
    struct csv_parser psr;

    if ( (ret = csv_init(&psr, 0)) != 0 )
    {
        c_write_log(_ERROR,"csv_init return[%d]", ret );
        return ret;
    }

    fp = fopen( csv_file, "rb" );
    if (!fp)
    {
        c_write_log(_ERROR,"fopen(%s) return[%d]:[%s]", 
                csv_file,errno, strerror(errno) );
        return ret;
    }

    char buf[1024];
    while ((bytes_read=fread(buf, 1, 1024, fp)) > 0)
    {
        if (csv_parse(&psr, buf, bytes_read, cb1, cb2, &ltfcsvdata) != bytes_read) 
        { 
            c_write_log(_ERROR,"Error while parsing file:[%s] return[%d]:[%s]", 
                    csv_file, csv_error(&psr), csv_strerror(csv_error(&psr)) ); 
            return 3;
        }
    }

    csv_fini(&psr, cb1, cb2, &ltfcsvdata);
    fclose(fp);
    print_LtfCsvData( &ltfcsvdata);
    csv_free(&psr);

    return 0;
}




//释放T_Ltf_Day_Cal->record_buf
static void free_LtfDayData( T_Ltf_Day_Cal * d_p )
{
    if ( NULL == d_p ) return;
    if ( NULL != d_p->record_buf )
    {
        free( d_p->record_buf );
        d_p->record_buf = NULL;
    }
    d_p->record_size = 0;
    return;
}

//打印T_Ltf_Day_Cal变量
static void print_LtfDayData( T_Ltf_Day_Cal * d_p )
{
    if ( NULL == d_p ) return;

    c_write_log(_INFO,"record_pos=[%u],record_size=[%u]" ,
            d_p->record_pos, d_p->record_size );

    //fuskytest add
    //return;

    //解析后的数据内容
    unsigned int i = 0;
    for( i = 0; i < d_p->record_pos; i++ )
    {
        T_Ltf_Day_Val* tp = &(d_p->record_buf[i]);

        c_write_log(_DEBUG,"row[%4u]:[%s,%f,%f,%f],n=[%u]",
                i, tp->dd, tp->ge, tp->ws.avg, tp->wd.avg, tp->wd.n );
    }
    return;
}




//判断T_Ltf_Day_Cal 的 record_buf 空间，不足时realloc分配足够的空间
static int daycal_increase_buffer( T_Ltf_Day_Cal * d_p )
{
    if ( NULL == d_p ) return -1;

    if( (d_p->record_pos + 1) >= d_p->record_size )
    {
        T_Ltf_Day_Val* trecord_p = (T_Ltf_Day_Val*) realloc(
                d_p->record_buf, 
                (d_p->record_size + LTF_REALLOC_ONENUM) * sizeof(T_Ltf_Day_Val) 
                );
        if ( NULL == trecord_p )
        {
            return -2;
        }
        memset( trecord_p + d_p->record_size,
                0,
                LTF_REALLOC_ONENUM * sizeof(T_Ltf_Day_Val) );

        d_p->record_buf  = trecord_p;
        d_p->record_size += LTF_REALLOC_ONENUM;
    }
    return 0;
} 




//在使用过的T_Ltf_Day_Cal结构中进行reset 重置操作
static void reInitLtfDayData( T_Ltf_Day_Cal* p )
{
    if ( NULL == p ) return;

    p->record_pos      = 0;//当前record_buf[]的有效内容的最后的index(值从0开始)

    // record_size 不能进行修改
    //p->record_size     = 0;   //当前record_buf[]中最大容量

    // record_buf 不能进行修改
    //p->record_buf      = NULL;//存储解析后的csv内容

    return;
}


static size_t find_blank_pos( unsigned char* s )
{
    size_t len = strlen((char*)s);
    size_t pos = 0;
    for( pos=0; pos < len; pos++ )
    {
        //0x20 空格, 0x09 tab
        if ( 0x20 == s[pos] || 0x09 == s[pos] )
        {
            break;
        }
    }
    return pos;
}




//根据从csv读取的 小时预测数据 生成 日预测数据
static int genLtfDayCsvByHour(T_Ltf_Csv_Data *h_p, T_Ltf_Day_Cal *d_p)
{
    if ( NULL == h_p ) return -1;
    if ( NULL == d_p )  return -2;

    reInitLtfDayData( d_p );
    //print_LtfDayData( d_p );
    daycal_increase_buffer( d_p );
    //print_LtfDayData( d_p );

    unsigned int i = 0;
    unsigned int j = 0;

    char find = 0;
    //遍历解析出现的 小时预测数据
    for( i = 0; i < h_p->record_pos; i++ )
    {
        T_Ltf_Record *  htv = &(h_p->record_buf[i]);

        //查找日预测数据中是否已经有相同时间YYYY/MM/dd(如2022/11/15)的数据
        //
        //  当前此查找算法考虑数据量不大所以采用的是完全遍历
        //  此处理过程的优点是:不要求小时csv文件按时间顺序存放
        //               缺点: 数据量大时影响效率
        //
        //  以后数据量大之后,可以根据预测的小时csv文件日期同一天的数据是连在一起的特点
        //  在下面的for循环中j=d_p->record_pos-1 这一条数据开始比较以达到比较次数减少的目的
        //  此处理过程的优点是:效率高
        //               缺点: 对小时预测csv文件有要求:同一天的数据中单不能插入别的日期的数据
        //
        find = 0;
        for ( j = 0; j< d_p->record_pos; j++ )
        {
            T_Ltf_Day_Val * dtv = &(d_p->record_buf[j]);
            // 小时预测csv中的字段顺序为:
            //dtv->field[0] //时间
            //dtv->field[1] //发电量
            //dtv->field[2] //风速
            //dtv->field[3] //风向

            //strlen(YYYY/MM/dd)=10
            //找到YYYY/MM/dd hh24:mi 格式中的空格位置
            size_t pos = find_blank_pos(htv->field[0].val);
            if ( strncmp( (char*)htv->field[0].val, dtv->dd, pos) == 0 )
            {
                find = 1;
                break;
            }
        }

        T_Ltf_Day_Val * dtv = &(d_p->record_buf[j]);
        if ( 1 == find ) //找到
        {
            alg_sum( dtv->ge, (char*)htv->field[1].val );           //预测电量求和
            alg_scalar_average( dtv->ws, (char*)htv->field[2].val );//预测风速求平均
            alg_vector_average( dtv->wd, (char*)htv->field[3].val );//预测风向求平均
        }
        else
        {
            memset( dtv, 0, sizeof(T_Ltf_Day_Val) ); //内容清空

            //strlen(YYYY/MM/dd)=10
            //找到YYYY/MM/dd hh24:mi 格式中的空格位置
            size_t pos = find_blank_pos(htv->field[0].val);

            strncpy( dtv->dd, (char*)htv->field[0].val, pos );
            alg_sum( dtv->ge, (char*)htv->field[1].val );           //预测电量求和
            alg_scalar_average( dtv->ws, (char*)htv->field[2].val );//预测风速求平均
            alg_vector_average( dtv->wd, (char*)htv->field[3].val );//预测风向求平均

            d_p->record_pos++; //下一个可以存放的位置
            daycal_increase_buffer( d_p ); //查看空间是否还够,不够则申请
        }

    }//end 小时预测数据遍历

    print_LtfDayData( d_p );

    return 0;
}


//将 YYYYMMDD 转换成 YYYY/MM/DD
static void str_conv_YYYYMMDD1( char* src, char* dst, int dst_len)
{
    if ( NULL == src || NULL == dst ) return;

    //c_write_log(_DEBUG,"src[%s],dst[%s],dst_len[%d]",src, dst, dst_len);

    if ( strlen(src) < 8 ) return;

    //8+2(个/)+1(字符串结尾结束符)
    if ( dst_len < 11 ) return;

    char tYYYY[4+1] = {0};
    char tMM[2+1] = {0};
    char tDD[2+1] = {0};
    memcpy(tYYYY, src,     4);
    memcpy(tMM,   src+4,   2);
    memcpy(tDD,   src+4+2, 2);
    snprintf( dst,dst_len, "%s/%s/%s",tYYYY,tMM,tDD);

    return;
}


//根据文件名取得预测批次表的数据
static int getLtfBatchByFile( char* file_name, T_Ltf_Batch *b_p )
{
    if ( NULL == file_name ) return -1;
    if ( NULL == b_p )       return -2;

    memset( b_p, 0, sizeof(T_Ltf_Batch) );

    int flen = strlen( file_name );
    if ( 0 == flen )
    {
        c_write_log(_ERROR, "文件名为空");
        return -3;
    }

    //file_name: /path/YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv
    //          or YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv

    char * dot_p = NULL;               //文件名中.的指针位置
    char * reverse_underlin_p1 = NULL; //文件名中倒数第1个下划线位置
    char * reverse_underlin_p2 = NULL; //文件名中倒数第2个下划线位置
    char * reverse_underlin_p3 = NULL; //文件名中倒数第3个下划线位置
    char * reverse_underlin_p4 = NULL; //文件名中倒数第4个下划线位置
    char * reverse_hori_p1     = NULL; //文件名中倒数第1个短横线位置
    char * f_begine_p          = NULL; //文件名的开始位置(不包括路径)

    int i = 0;
    for( i= flen; i>0; i-- )
    {
        if ( file_name[i] == '.' ) 
        { 
            dot_p = &(file_name[i]);
        }
        else if ( file_name[i] == '_' )
        {
            if ( NULL == reverse_underlin_p1 )
            {
                reverse_underlin_p1 = &(file_name[i]); 
            }
            else if ( NULL == reverse_underlin_p2 )
            {
                reverse_underlin_p2 = &(file_name[i]); 
            }
            else if ( NULL == reverse_underlin_p3 )
            {
                reverse_underlin_p3 = &(file_name[i]); 
            }
            else if ( NULL == reverse_underlin_p4 )
            {
                reverse_underlin_p4 = &(file_name[i]); 
            }

        }
        else if ( file_name[i] == '-' )
        {
            reverse_hori_p1 = &(file_name[i]);
        }
        else if ( file_name[i] == '/' )
        {
            //只处理文件名,遇到路径即可跳出
            f_begine_p = &(file_name[i]);
            f_begine_p++;
            break;
        }
    }

    if ( NULL == f_begine_p ) f_begine_p = file_name;

    if ( NULL == dot_p
            || NULL == reverse_underlin_p1
            || NULL == reverse_underlin_p2
            || NULL == reverse_underlin_p3
            || NULL == reverse_underlin_p4
            || NULL == reverse_hori_p1
            || NULL == f_begine_p
       )
    {
        //文件名格式错误
        c_write_log(_ERROR,
                "文件名[%s]格式错误,不是"
                "YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv格式",
                file_name );
        return -4;
    }

    //c_write_log(_DEBUG,"fname[%s]",f_begine_p);

    // 取   YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv
    //  中的第1个YYYYMMDD 到 dd_stt
    int tlen = reverse_hori_p1 - f_begine_p;
    if( tlen < 8 )
    {
        c_write_log(_ERROR,
                "文件名[%s]格式错误,开头的日期长度应为8,"
                "而实际长度=[%d],正确的文件全名格式为:"
                "YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv格式",
                f_begine_p, tlen );
        return -5;
    }

    str_conv_YYYYMMDD1( f_begine_p, b_p->dd_stt, sizeof(b_p->dd_stt) ); 
    c_write_log(_DEBUG,"b_p->dd_stt[%s]",b_p->dd_stt);

    // 取   YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv
    //  中的第2个YYYYMMDD 到 dd_stp
    tlen = reverse_underlin_p4 - ( reverse_hori_p1 + 1);
    if( tlen < 8 )
    {
        c_write_log(_ERROR,
                "文件名[%s]格式错误,第二个日期长度应为8,"
                "而实际长度=[%d],正确的文件全名格式为:"
                "YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv格式",
                f_begine_p, tlen );
        return -6;
    }
    char tmpstr[LTF_MAX_FIELD_LEN+1];
    memset( tmpstr, 0, sizeof(tmpstr) );
    memcpy( tmpstr, reverse_hori_p1 + 1, tlen);

    str_conv_YYYYMMDD1( tmpstr, b_p->dd_stp, sizeof(b_p->dd_stp) ); 
    c_write_log(_DEBUG,"b_p->dd_stp[%s]",b_p->dd_stp);

    // 取   YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv
    //  中的xx 到 b_p->dd_fc
    tlen = reverse_underlin_p3 - (reverse_underlin_p4 + 1);
    if( tlen < 2 || tlen > 4 )
    {
        c_write_log(_ERROR,
                "文件名[%s]格式错误,文件名的数据频度长度为2,"
                "而实际长度=[%d],正确的文件全名格式为:"
                "YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv格式",
                f_begine_p, tlen );
        return -7;
    }
    memcpy( b_p->dd_fc, reverse_underlin_p4 + 1, tlen);
    c_write_log(_DEBUG,"b_p->dd_fc[%s]",b_p->dd_fc);

    // 取   YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv
    //  中的N 到 b_p->src
    tlen = dot_p - (reverse_underlin_p1 + 1);
    if( tlen < 1 || tlen > 4 )
    {
        c_write_log(_ERROR,
                "文件名[%s]格式错误,文件名的数据源长度为1,"
                "而实际长度=[%d],正确的文件全名格式为:"
                "YYYYMMDD-YYYYMMDD_xx_farmname_LTF_N.csv格式",
                f_begine_p, tlen );
        return -7;
    }
    memcpy( b_p->src, reverse_underlin_p1 + 1, tlen);
    c_write_log(_DEBUG,"b_p->src[%s]",b_p->src);

    return 0;
}


int main()
{

    //test_alg();
    //return 0;

    int ret = 0;

    //SetLogLevel(_INFO);
    //SetLogLevel(_DEBUG);

    //c_write_log(_DEBUG,"rundir=[%s]",ELFcmd::get_self_exe_path());
    //c_write_log(_DEBUG,"run_full_name=[%s]",ELFcmd::get_self_exe_fname());

    char tCsvFile[1024] = {0};

    snprintf ( tCsvFile, 1024, "%s/%s",ELFcmd::get_self_exe_path(),"20221115-20221231_1h_新疆华电苦水风电场_LTF_2.csv" );
    c_write_log(_DEBUG,"csv file [%s]",tCsvFile );


    T_Ltf_Batch tbdata;
    ret = getLtfBatchByFile( tCsvFile, &tbdata );
    if ( ret != 0 )
    {
        c_write_log(_ERROR,"getLtfBatchByFile return[%d]",ret);
        return ret;
    }

    return 0;


    T_Ltf_Csv_Data ltfcsvdata;
    memset( &ltfcsvdata, 0, sizeof(T_Ltf_Csv_Data) );
    //unsigned int skip_line_num = 1;
    ret = parse_Ltf_csv_file( tCsvFile, ltfcsvdata );
    if ( ret != 0 )
    {
        c_write_log(_ERROR,"parse_Ltf_csv_file return[%d]",ret);
        return ret;
    }
    T_Ltf_Day_Cal ltfdaydata;
    memset( &ltfdaydata, 0, sizeof( ltfdaydata) );
    print_LtfDayData( &ltfdaydata );

    ret = genLtfDayCsvByHour( &ltfcsvdata, &ltfdaydata);

    free_LtfCsvData( &ltfcsvdata );
    free_LtfDayData( &ltfdaydata );

    return 0;
}
