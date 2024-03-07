#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>

#include "ELFcmd.h"
#include "CustomOutLog.h"
#include "csv.h"


#define  TF_MAX_FIELD_LEN   43  //短期预测csv中内容字段最大长度
#define  TF_FIELD_NUM_ONER  31   //短期预测csv中内容一行的字段数量
#define  TF_REALLOC_ONENUM 100  //realloc分配空间时,每次新增加的记录的个数


//定义csv解析数据结构一个字段的值
typedef struct
{
    unsigned char val[TF_MAX_FIELD_LEN+1];
}T_Tf_Field;


//定义csv解析数据相关结构
typedef struct
{
    unsigned char  drop_line_flag; //丢弃当前行标识:1 表示当前行数据严重不合格,丢弃当前行
    unsigned char  set_fix_flag;   //只否根据第一行数据列自动生成field_fix_num;0:自动;1:不自动
    unsigned int   field_fix_num;  //设置csv文件有多少列
    unsigned int   skip_line_num;  //解析csv时跳过开头的行数
    unsigned int   tmp_row_counter;//临时用于记录解析csv文件时当前所处的行号(值从1开始)
    unsigned int   field_index;    //解析某一行时记录，当前处于哪一个字段(值从0开始)
    unsigned int   line_pos;     //当前field_buf[]的有效内容行号的最后的index(值从0开始)
    unsigned int   line_size;    //当前field_buf[]中行号的最大容量
    T_Tf_Field * field_buf;     //存储解析后的csv内容
}T_parse_csv_data;



//释放T_parse_csv_data->field_buf
static void free_ParCsvData( T_parse_csv_data * d_p )
{
    if ( NULL == d_p ) return;
    if ( NULL != d_p->field_buf )
    {
        free( d_p->field_buf );
        d_p->field_buf = NULL;
    }
    memset( d_p, 0, sizeof(T_parse_csv_data) );
    return;
}



//打印T_parse_csv_data变量
static void print_TfCsvData( T_parse_csv_data * d_p )
{
    if ( NULL == d_p ) return;

    c_write_log(_INFO,"drop_line_flag=[%d],field_fix_num=[%u],skip_line_num=[%u],tmp_row_counter=[%u],"
            "field_index=[%u],line_pos=[%u],line_size=[%u]",
            d_p->drop_line_flag, d_p->field_fix_num, d_p->skip_line_num,d_p->tmp_row_counter,
            d_p->field_index,d_p->line_pos,d_p->line_size);

    char tmp_prt_buf[1024];

    //解析后的数据内容
    unsigned int tColNum = d_p->field_fix_num;
    unsigned int tLineBase = 0;

    int fixlen = 1024;

    for( unsigned int i = 0; i < d_p->line_pos; i++ )
    {
        memset(tmp_prt_buf,0,fixlen);

        int ret = 0;
        tLineBase = i * tColNum;
        int blen = 0;

        for(unsigned int j=0; j < tColNum; j++)
        {
            if ( 0 == j ){
                ret = snprintf(tmp_prt_buf + blen,fixlen - blen,"%s",d_p->field_buf[tLineBase+j].val);
            }
            else {
                ret = snprintf(tmp_prt_buf + blen,fixlen - blen,",%s",d_p->field_buf[tLineBase+j].val);
            }
            if ( ret < 0 )
            {
                c_write_log(_ERROR,"snprintf return[%d]",ret);
                break;
            }
            blen += ret;
            if ( blen >= fixlen )
            {
                c_write_log(_ERROR,"max len [%d]",fixlen);
                break;
            }
        }
        c_write_log(_DEBUG,"row[%4u]:[%s]", i,tmp_prt_buf );
    }
    return;
}


//判断T_parse_csv_data 的 field_buf 空间，不足时realloc分配足够的空间
static int auto_increase_csv_buffer( T_parse_csv_data * d_p )
{
    if ( NULL == d_p ) return -1;


    //列不够时增加列的数量,每次只增加一列,且此操作需满足以下条件
    // 1. 手动设置列增加标识设为0
    // 2. 用第一行要存的列数作为后面所行要存的列数的最大值
    if( d_p->set_fix_flag == 0 
        && d_p->line_pos == 0 )
    {
        if ( d_p->field_index  >= d_p->field_fix_num )
        {
            int tDiff = d_p->field_index + 1 - d_p->field_fix_num;

            T_Tf_Field* tfield_p = (T_Tf_Field*) realloc(
                    d_p->field_buf, 
                    (d_p->field_fix_num + tDiff) * sizeof(T_Tf_Field) 
                    );
            if ( NULL == tfield_p )
            {
                return -2;
            }
            memset( tfield_p + d_p->field_fix_num ,
                    0,
                    tDiff  * sizeof(T_Tf_Field) );
            d_p->field_buf  = tfield_p;
            d_p->field_fix_num += tDiff;

            //当前只有一行值的容量
            d_p->line_size = 1;
        }
        return 0;
    }


    //行不够时每次多分配TF_REALLOC_ONENUM行的空间
    if( (d_p->line_pos + 1) >= d_p->line_size )
    {
        //至少有一个列值
        if ( d_p->field_fix_num < 1 ) d_p->field_fix_num = 1;

        T_Tf_Field* tfield_p = (T_Tf_Field*) realloc(
                d_p->field_buf, 
                (d_p->line_size + TF_REALLOC_ONENUM) * d_p->field_fix_num * sizeof(T_Tf_Field) 
                );
        if ( NULL == tfield_p )
        {
            return -2;
        }
        memset( tfield_p + d_p->line_size * d_p->field_fix_num,
                0,
                TF_REALLOC_ONENUM * d_p->field_fix_num * sizeof(T_Tf_Field) );

        d_p->field_buf  = tfield_p;
        d_p->line_size += TF_REALLOC_ONENUM;

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

    T_parse_csv_data* d_p = (T_parse_csv_data*) data;
    //如果reord_buf为空则不解析field
    if ( NULL == d_p->field_buf ) return;


    //如果当前行是要丢弃的则不解析field
    if ( 1 == d_p->drop_line_flag ) return;

    //如果设置了需要跳过的开头行数,且当前所解析的行号小于要跳过的行数则不解析field
    if ( d_p->skip_line_num > 0 && d_p->tmp_row_counter <= d_p->skip_line_num )
    {
        return;
    }


    if ( NULL == s )
    { 
        c_write_log(_ERROR,"tmp_row_counter=[%u],field_index=[%u],"
                "line_pos=[%u], func cb1 input para s=NULL",
                d_p->tmp_row_counter, d_p->field_index,
                d_p->line_pos );
        return;
    }

    //根据第一行要存在的值判断要取的列数并重新分配不够的空间
    if ( 0 == d_p->line_pos )
    {
        //判断T_parse_csv_data 的 field_buf 空间，不足时realloc分配足够的空间
        int ret = auto_increase_csv_buffer( d_p );
        if ( ret != 0 )
        {
            c_write_log(_ERROR,"auto_increase_csv_buffer return[%d]", ret);
            return;
        }
    }

    //要解析的field 数超过了 d_p->field_fix_num 则不解析field
    if ( ( d_p->field_index +1 ) > d_p->field_fix_num )
    {
        return;
    }

    //T_Tf_Field* tfd_p = &(d_p->field_buf[d_p->line_pos].field[d_p->field_index]);
    T_Tf_Field* tfd_p = &(d_p->field_buf[d_p->line_pos * d_p->field_fix_num + d_p->field_index]);
    memset( tfd_p, 0 , sizeof(tfd_p->val));
    if( 0 == len )
    {
        //c_write_log(_DEBUG,"tmp_row_counter=[%u],field_index=[%u],"
        //        "line_pos=[%u], len==0",
        //        d_p->tmp_row_counter, d_p->field_index,
        //        d_p->line_pos );

        //如果第一个字段(时间)为空则标记当前行应该为丢弃行,此行将不再解析
        if( 0 == d_p->field_index )
        {
            d_p->drop_line_flag = 1;
            c_write_log(_ERROR,"tmp_row_counter=[%u],field_index=[%u],"
                    "line_pos=[%u], field val is blank,will set drop_line_flag=1",
                    d_p->tmp_row_counter, d_p->field_index,
                    d_p->line_pos );
        }
        else //其他字段为空默认赋值为"0"
        {
            tfd_p->val[0] = 0x30; //"0"
        }

    }
    else
    {
        size_t tlen = len;

        //copy field的长度不能超过定义的TF_MAX_FIELD_LEN
        if ( tlen > TF_MAX_FIELD_LEN ) tlen = TF_MAX_FIELD_LEN;
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


    T_parse_csv_data* d_p = (T_parse_csv_data*) data;

    //如果设置了需要跳过的开头行数,且当前所解析的行号小于要跳过的行数则 line_pos不变
    if ( d_p->skip_line_num > 0 && d_p->tmp_row_counter <= d_p->skip_line_num )
    {
        d_p->drop_line_flag = 0;
        //d_p->line_pos++;
    }
    else if ( 1 == d_p->drop_line_flag ) //丢弃的行line_pos值不变 
    {
        d_p->drop_line_flag = 0;
        //d_p->line_pos++;
    }
    else //其他情况line_pos需要变
    {
        d_p->drop_line_flag = 0;
        d_p->line_pos++;  //将要存储解析行的pos index
    }
    
    d_p->field_index = 0;   //下一行的field index重置
    d_p->tmp_row_counter++; //将要解析的下一行号

    //判断T_parse_csv_data 的 field_buf 空间，不足时realloc分配足够的空间
    int ret = auto_increase_csv_buffer( d_p );
    if ( ret != 0 )
    {
        c_write_log(_ERROR,"auto_increase_csv_buffer return[%d]", ret);
        return;
    }

    //将下行将要赋值的解析行赋空
    memset( &(d_p->field_buf[d_p->line_pos* d_p->field_fix_num]), 
            0, 
            d_p->field_fix_num * sizeof(T_Tf_Field) );

    return;
}




//在使用过的T_parse_csv_data结构中进行reset 重置操作
static void reInitTfCsvData( T_parse_csv_data* p, unsigned int * skip_line_num=NULL )
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
    p->line_pos        = 0;//当前field_buf[]的有效内容的最后的index(值从0开始)
    p->set_fix_flag    = 0;//当前field_buf[]的有效内容的最后的index(值从0开始)

    // field_fix_num 不能进行修改
    //p->field_fix_num = 0;   //固定列数

    // line_size 不能进行修改
    //p->line_size     = 0;   //当前field_buf[]中最大容量

    // field_buf 不能进行修改
    //p->field_buf      = NULL;//存储解析后的csv内容

    return;
}




//解析ELF csv文件数据到T_parse_csv_data结构中去
//   skip_line_num: 解析csv文件时需要跳过文件头的行数
static int parse_Tf_csv_file( 
        const char* csv_file, 
        T_parse_csv_data &ltfcsvdata,  
        unsigned int skip_line_num = 1 
        )
{
    reInitTfCsvData( &ltfcsvdata, &skip_line_num );
    //print_TfCsvData( &ltfcsvdata);
    auto_increase_csv_buffer( &ltfcsvdata );
    //print_TfCsvData( &ltfcsvdata);

    FILE  *fp = NULL;
    int   ret = 0;
    size_t bytes_read = 0;
    struct csv_parser psr;

    if ( (ret = csv_init(&psr, 0)) != 0 )
    {
        c_write_log(_ERROR,"csv_init return[%d]", ret );
        return ret;
    }
    csv_set_delim(&psr,CSV_SPACE);

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
    print_TfCsvData( &ltfcsvdata);
    csv_free(&psr);

    return 0;
}






//static size_t find_blank_pos( unsigned char* s )
//{
//    size_t len = strlen((char*)s);
//    size_t pos = 0;
//    for( pos=0; pos < len; pos++ )
//    {
//        //0x20 空格, 0x09 tab
//        if ( 0x20 == s[pos] || 0x09 == s[pos] )
//        {
//            break;
//        }
//    }
//    return pos;
//}





////将 YYYYMMDD 转换成 YYYY/MM/DD
//static void str_conv_YYYYMMDD1( char* src, char* dst, int dst_len)
//{
//    if ( NULL == src || NULL == dst ) return;
//
//    //c_write_log(_DEBUG,"src[%s],dst[%s],dst_len[%d]",src, dst, dst_len);
//
//    if ( strlen(src) < 8 ) return;
//
//    //8+2(个/)+1(字符串结尾结束符)
//    if ( dst_len < 11 ) return;
//
//    char tYYYY[4+1] = {0};
//    char tMM[2+1] = {0};
//    char tDD[2+1] = {0};
//    memcpy(tYYYY, src,     4);
//    memcpy(tMM,   src+4,   2);
//    memcpy(tDD,   src+4+2, 2);
//    snprintf( dst,dst_len, "%s/%s/%s",tYYYY,tMM,tDD);
//
//    return;
//}


void mytrim( std::string & s)
{
    if ( s.empty() ) return;
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ")+1);
}


int main()
{
    int ret = 0;
    int v1;

    //std::string k1=" 2023-08-28 15:41:21 ";
    std::string k1;
    c_write_log(_DEBUG,"k1=[%s]",k1.c_str());
    mytrim(k1);
    c_write_log(_DEBUG,"k1=[%s]",k1.c_str());
    return 0;


    int *vp1 = NULL;

    if ( ret == 0 ) {
        v1 = 100;
        vp1 = &v1;
    }
    c_write_log(_DEBUG,"v1=[%d],vp1=[%d]",v1,*vp1);
    std::string tss="2022-02-09_12:15:00";
    //std::string tss="2022-02-09 12:15:00";

    c_write_log(_DEBUG,"tss=[%s]",tss.c_str());
    std::size_t tpos = tss.find("_");
    if ( tpos != std::string::npos ) tss.replace(tpos,1," "); 
    c_write_log(_DEBUG,"tss=[%s]",tss.c_str());

    std::string tdstr="12.345";
    double dtdv = std::stod(tdstr);
    std::stringstream strstr;
    strstr<<dtdv;
    std::string t1=strstr.str();
    c_write_log(_DEBUG,"tdstr=[%s],dtdv=[%lf],t1=[%s]",tdstr.c_str(), dtdv,t1.c_str());

    return 0;


    ////std::string  tstr="aa,bb,cc,dd";
    //std::string  tstr="aabbccdd";
    //std::string  tmps;
    //std::size_t found=0;
    //std::size_t bfound=0;
    //std::size_t tlen=0;
    //while( (found=tstr.find(",",bfound)) != std::string::npos ) {
    //    tlen = found - bfound;
    //    tmps = tstr.substr(bfound,tlen);
    //    c_write_log(_DEBUG,"tstr=[%s],tmps=[%s]",tstr.c_str(), tmps.c_str());
    //    bfound = found +1;
    //}

    //tlen = tstr.size() - bfound;
    //tmps = tstr.substr(bfound,tlen);
    //c_write_log(_DEBUG,"tstr=[%s],tmps=[%s]",tstr.c_str(), tmps.c_str());

    //return 0;


    //SetLogLevel(_DEBUG);

    //c_write_log(_DEBUG,"rundir=[%s]",ELFcmd::get_self_exe_path());
    //c_write_log(_DEBUG,"run_full_name=[%s]",ELFcmd::get_self_exe_fname());

    char tCsvFile[1024] = {0};

    //snprintf ( tCsvFile, 1024, "%s/%s",ELFcmd::get_self_exe_path(),"qxsj01093.txt" );
    snprintf ( tCsvFile, 1024, "%s/%s",ELFcmd::get_self_exe_path(),"ycsj01093.txt" );
    c_write_log(_DEBUG,"csv file [%s]",tCsvFile );


    //return 0;


    T_parse_csv_data ltfcsvdata;
    memset( &ltfcsvdata, 0, sizeof(T_parse_csv_data) );
    //unsigned int skip_line_num = 1;
    ret = parse_Tf_csv_file( tCsvFile, ltfcsvdata, 1 );
    if ( ret != 0 )
    {
        c_write_log(_ERROR,"parse_Tf_csv_file return[%d]",ret);
        return ret;
    }


    free_ParCsvData( &ltfcsvdata );

    return 0;
}
