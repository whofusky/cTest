/***********************************************************
 *
 * @file    ELFcmd.cpp
 *
 * @brief   ELFcmd源文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-17_09:19:45
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>



#include "ELFcmd.h"
#include "CustomOutLog.h"
#include "InfraBase.h"

static pthread_mutex_t pgsLock = PTHREAD_MUTEX_INITIALIZER;

#define LC_DIR_MAX_LEN    1024        //路径的最大长度
#define LC_FILENAME_MAX   100         //文件名的最大长度
#define LC_FULLNAME_MAX   1124        //路径+文件名的最大长度

static char  pgsGetPthFlag = 0;                      //判断是否已经取值
static char  pgsBaseDir_a[LC_DIR_MAX_LEN]   = {0};   //程序运行路径
static char  pgsPathFile_a[LC_FULLNAME_MAX] = {0};   //带路径的程序名

#define MC_CBUFFER_SIZE 1024

///////////////////////////////////md5 src begine
typedef struct
{
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
}MD5_CTX;

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16]);


#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
	{ \
		a += F(b,c,d) + x + ac; \
		a = ROTATE_LEFT(a,s); \
		a += b; \
	}
#define GG(a,b,c,d,x,s,ac) \
	{ \
		a += G(b,c,d) + x + ac; \
		a = ROTATE_LEFT(a,s); \
		a += b; \
	}
#define HH(a,b,c,d,x,s,ac) \
	{ \
		a += H(b,c,d) + x + ac; \
		a = ROTATE_LEFT(a,s); \
		a += b; \
	}
#define II(a,b,c,d,x,s,ac) \
	{ \
		a += I(b,c,d) + x + ac; \
		a = ROTATE_LEFT(a,s); \
		a += b; \
	}

void MD5Transform(unsigned int state[4],unsigned char block[64]);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);

unsigned char PADDING[] = {
	0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void MD5Init(MD5_CTX *context) {
	context->count[0] = 0;
	context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
}
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)
{
	unsigned int i = 0,index = 0,partlen = 0;
	index = (context->count[0] >> 3) & 0x3F;
	partlen = 64 - index;
	context->count[0] += inputlen << 3;
	if(context->count[0] < (inputlen << 3)) {
		context->count[1]++;
	}
	context->count[1] += inputlen >> 29;

	if(inputlen >= partlen) {
		memcpy(&context->buffer[index],input,partlen);
		MD5Transform(context->state,context->buffer);
		for(i = partlen;i+64 <= inputlen;i+=64) {
			MD5Transform(context->state,&input[i]);
		}
		index = 0;
	} else {
		i = 0;
	}
	
	memcpy(&context->buffer[index],&input[i],inputlen-i);
}

void MD5Final(MD5_CTX *context,unsigned char digest[16]) {
	unsigned int index = 0,padlen = 0;
	unsigned char bits[8];
	index = (context->count[0] >> 3) & 0x3F;
	padlen = (index < 56)?(56-index):(120-index);
	MD5Encode(bits,context->count,8);
	MD5Update(context,PADDING,padlen);
	MD5Update(context,bits,8);
	MD5Encode(digest,context->state,16);
}
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len) {
	unsigned int i = 0,j = 0;
	while(j < len) {
		output[j] = input[i] & 0xFF;
		output[j+1] = (input[i] >> 8) & 0xFF;
		output[j+2] = (input[i] >> 16) & 0xFF;
		output[j+3] = (input[i] >> 24) & 0xFF;
		i++;
		j+=4;
	}
}
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len) {
	unsigned int i = 0,j = 0;
	while(j < len) {
		output[i] = (input[j]) | (input[j+1] << 8) | (input[j+2] << 16) | (input[j+3] << 24);
		i++;
		j+=4;
	}
}
void MD5Transform(unsigned int state[4],unsigned char block[64]) {
	unsigned int a = state[0];
	unsigned int b = state[1];
	unsigned int c = state[2];
	unsigned int d = state[3];
	unsigned int x[64];

	MD5Decode(x,block,64);

	FF(a, b, c, d, x[ 0], 7, 0xd76aa478);
	FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);
	FF(c, d, a, b, x[ 2], 17, 0x242070db);
	FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);
	FF(a, b, c, d, x[ 4], 7, 0xf57c0faf);
	FF(d, a, b, c, x[ 5], 12, 0x4787c62a);
	FF(c, d, a, b, x[ 6], 17, 0xa8304613);
	FF(b, c, d, a, x[ 7], 22, 0xfd469501);
	FF(a, b, c, d, x[ 8], 7, 0x698098d8);
	FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);
	FF(c, d, a, b, x[10], 17, 0xffff5bb1);
	FF(b, c, d, a, x[11], 22, 0x895cd7be);
	FF(a, b, c, d, x[12], 7, 0x6b901122);
	FF(d, a, b, c, x[13], 12, 0xfd987193);
	FF(c, d, a, b, x[14], 17, 0xa679438e);
	FF(b, c, d, a, x[15], 22, 0x49b40821);

	GG(a, b, c, d, x[ 1], 5, 0xf61e2562);
	GG(d, a, b, c, x[ 6], 9, 0xc040b340);
	GG(c, d, a, b, x[11], 14, 0x265e5a51);
	GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
	GG(a, b, c, d, x[ 5], 5, 0xd62f105d);
	GG(d, a, b, c, x[10], 9,  0x2441453);
	GG(c, d, a, b, x[15], 14, 0xd8a1e681);
	GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
	GG(a, b, c, d, x[ 9], 5, 0x21e1cde6);
	GG(d, a, b, c, x[14], 9, 0xc33707d6);
	GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);
	GG(b, c, d, a, x[ 8], 20, 0x455a14ed);
	GG(a, b, c, d, x[13], 5, 0xa9e3e905);
	GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8);
	GG(c, d, a, b, x[ 7], 14, 0x676f02d9);
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);


	HH(a, b, c, d, x[ 5], 4, 0xfffa3942);
	HH(d, a, b, c, x[ 8], 11, 0x8771f681);
	HH(c, d, a, b, x[11], 16, 0x6d9d6122);
	HH(b, c, d, a, x[14], 23, 0xfde5380c);
	HH(a, b, c, d, x[ 1], 4, 0xa4beea44);
	HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
	HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
	HH(b, c, d, a, x[10], 23, 0xbebfbc70);
	HH(a, b, c, d, x[13], 4, 0x289b7ec6);
	HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);
	HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);
	HH(b, c, d, a, x[ 6], 23,  0x4881d05);
	HH(a, b, c, d, x[ 9], 4, 0xd9d4d039);
	HH(d, a, b, c, x[12], 11, 0xe6db99e5);
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
	HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);


	II(a, b, c, d, x[ 0], 6, 0xf4292244);
	II(d, a, b, c, x[ 7], 10, 0x432aff97);
	II(c, d, a, b, x[14], 15, 0xab9423a7);
	II(b, c, d, a, x[ 5], 21, 0xfc93a039);
	II(a, b, c, d, x[12], 6, 0x655b59c3);
	II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
	II(c, d, a, b, x[10], 15, 0xffeff47d);
	II(b, c, d, a, x[ 1], 21, 0x85845dd1);
	II(a, b, c, d, x[ 8], 6, 0x6fa87e4f);
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
	II(c, d, a, b, x[ 6], 15, 0xa3014314);
	II(b, c, d, a, x[13], 21, 0x4e0811a1);
	II(a, b, c, d, x[ 4], 6, 0xf7537e82);
	II(d, a, b, c, x[11], 10, 0xbd3af235);
	II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
	II(b, c, d, a, x[ 9], 21, 0xeb86d391);
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

///////////////////////////////////md5 src end








// Dsc: 获取当前运行程序的路径值
static int getAppPath()
{

    int tLen = LC_FULLNAME_MAX -1;

    int r = readlink( "/proc/self/exe", pgsPathFile_a, tLen );
    if ( r < 0 || r >= tLen )
    {
        printf( "readlink fail,errno[%d],errMsg[%s]", errno, strerror(errno) );

        return 1;
    }

    //带路径值的可执行程序
    pgsPathFile_a[r] = '\0';
    int i            = 0;
    int tFindFlag    = 0;
    for ( i = r; i>=0; i-- )
    {
        if ( pgsPathFile_a[i] == '/' )
        {
            //pgsPathFile_a[i] = '\0';
            strncpy(pgsBaseDir_a,pgsPathFile_a,i);
            pgsBaseDir_a[i] = '\0';
            //tLen            = r - i;
            //strncpy( pgsExe, pgsPathFile_a+i+1, tLen );
            tFindFlag = 1;
            break;
        }
    }

    if ( tFindFlag == 0 )
    {
        return 2;
    }

    return 0;
}





//获取当前运行程序的路径
const char* ELFcmd::get_self_exe_path()
{
    if ( 0 == pgsGetPthFlag )
    {
        pthread_mutex_lock( &pgsLock );
        if ( 0 == pgsGetPthFlag )
        {
            getAppPath();
            pgsGetPthFlag = 1;
        }
        pthread_mutex_unlock( &pgsLock );
    }
    return pgsBaseDir_a;
}






//获取当前运行程序的的名称(带绝对路径)
const char* ELFcmd::get_self_exe_fname()
{
    if ( 0 == pgsGetPthFlag )
    {
        pthread_mutex_lock( &pgsLock );
        if ( 0 == pgsGetPthFlag )
        {
            getAppPath();
            pgsGetPthFlag = 1;
        }
        pthread_mutex_unlock( &pgsLock );
    }
    return pgsPathFile_a;
}


// filename : 文件路径
// omd5     : 文件的md5值
// return   :  0  成功  !0 失败
int ELFcmd::md5sum( const char* filename, char omd5[32+1] )
{
    if ( NULL == filename )
    {
        return -1;
    }

	MD5_CTX md5;
	unsigned char decrypt[16];
    MD5Init(&md5);

    FILE *fp = fopen(filename, "rb");
    if( NULL == fp) 
    {
        return -2;//打开文件错误
    }
    
    unsigned char encrypt[1024];
    unsigned int ilen = 0;
    while( !feof(fp) && !ferror(fp) ) 
    {
        ilen = fread( encrypt, 1, 1024, fp );
        if ( !ferror(fp) )
        {
            MD5Update(&md5, encrypt, ilen);
        }
    }
    fclose(fp);
    MD5Final(&md5, decrypt);

    snprintf( omd5, 33,
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            decrypt[0],decrypt[1],decrypt[2],decrypt[3],
            decrypt[4],decrypt[5],decrypt[6],decrypt[7],
            decrypt[8],decrypt[9],decrypt[10],decrypt[11],
            decrypt[12],decrypt[13],decrypt[14],decrypt[15]
            );
    //omd5[33]='\0';

    return 0;
}


// instr    : 将要加密的字符串
// omd5     : 字符串的的md5值
// return   :  0  成功  !0 失败
int ELFcmd::md5str( char* instr, char omd5[32+1] )
{
    if ( NULL == instr )
    {
        return -1;
    }

	MD5_CTX md5;
	unsigned char decrypt[16];
    MD5Init(&md5);

    MD5Update(&md5, (unsigned char*)instr, strlen(instr));
    MD5Final(&md5, decrypt);

    snprintf( omd5, 33,
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            decrypt[0],decrypt[1],decrypt[2],decrypt[3],
            decrypt[4],decrypt[5],decrypt[6],decrypt[7],
            decrypt[8],decrypt[9],decrypt[10],decrypt[11],
            decrypt[12],decrypt[13],decrypt[14],decrypt[15]
            );
    //omd5[33]='\0';

    return 0;
}


// 取/proc/pid/cmdline文件的内容
int ELFcmd::getCmdlinePar( const char* path, std::string str[], int &inoutnum )
{
    char tmpStr[1024] = {0};
    int tfixnum = inoutnum;
    FILE *fp;

    if ( ( fp = fopen(path,"rb") ) == NULL )
    {
        inoutnum = 0;
        return errno;
    }
    if ( fgets(tmpStr,1023,fp) == NULL )
    {
        inoutnum = 0;
        return 1;
    }
    fclose(fp);

    char *tp = tmpStr;
    int i = 0;
    while ( *tp != '\0' && i < tfixnum )
    {
        //b_write_log(_DEBUG,"[%s]",tp);
        str[i++] = tp;
        tp = tp + strlen(tp) +1;
    }

    inoutnum = i;

    return 0;
}




// 取/proc/pid/status文件的内容
//  str[0] Name   进程名
//  str[1] State  进程的状态
//  str[2] PPid   进程的父进程
//  str[3] VmSize 进程现在正在占用的内存
//  str[4] VmPeak 当前进程运行过程中占用内存的峰值
//  str[5] VmRSS  是程序现在使用的物理内存
//  str[6] VmHWM  是程序得到分配到物理内存的峰值
int ELFcmd::getPidStatus( const char* path, std::string str[], int &inoutnum )
{
    char tmpStr[1024] = {0};
    char name[512]    = {0};
    char key[512]     = {0};
    int tfixnum = inoutnum;

    FILE *fp = NULL;
    if ( ( fp = fopen(path,"rb") ) == NULL )
    {
        inoutnum = 0;
        return errno;
    }

    int i = 0;
    while( fgets(tmpStr,1023,fp) != NULL && i < tfixnum )
    {
        sscanf( tmpStr,"%[^:] : %511[^\n]",name,key);

        if ( strcmp(name,"Name") == 0 )
        {
            str[0] = key;
            i++;
        }
        else if ( strcmp(name,"State") == 0 )
        {
            str[1] = key;
            i++;
        }
        else if ( strcmp(name,"PPid") == 0 )
        {
            str[2] = key;
            i++;
        }
        else if ( strcmp(name,"VmSize") == 0 )
        {
            str[3] = key;
            i++;
        }
        else if ( strcmp(name,"VmPeak") == 0 )
        {
            str[4] = key;
            i++;
        }
        else if ( strcmp(name,"VmRSS") == 0 )
        {
            str[5] = key;
            i++;
        }
        else if ( strcmp(name,"VmHWM") == 0 )
        {
            str[6] = key;
            i++;
        }

        //b_write_log( _DEBUG,"name[%s],key[%s]",name,key);
    }
    fclose(fp);

    inoutnum = i;

    return 0;

}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByName( const std::string &exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[LC_DIR_MAX_LEN] = {0};
    char tcwdpath[LC_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];
    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name.c_str(), tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , LC_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, LC_DIR_MAX_LEN );
        if ( r < 0 || r >= LC_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name.c_str(), s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , LC_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, LC_DIR_MAX_LEN );
                if ( r < 0 || r >= LC_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path


        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name.c_str(),tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByName( const char* exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[LC_DIR_MAX_LEN] = {0};
    char tcwdpath[LC_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];
    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name, tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , LC_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, LC_DIR_MAX_LEN );
        if ( r < 0 || r >= LC_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name, s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , LC_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, LC_DIR_MAX_LEN );
                if ( r < 0 || r >= LC_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path


        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name,tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByFullName( const char* full_exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[LC_DIR_MAX_LEN] = {0};
    char tcwdpath[LC_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];

    std::string fullexename = full_exe_name;
    std::string exe_name ;
    std::string exe_path;
    get_path_fname( fullexename, exe_path, exe_name);

    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name.c_str(), tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , LC_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, LC_DIR_MAX_LEN );
        if ( r < 0 || r >= LC_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name.c_str(), s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , LC_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, LC_DIR_MAX_LEN );
                if ( r < 0 || r >= LC_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path

        if ( strPathRun != exe_path )
        {
            continue;
        }

        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name.c_str(),tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByFullName( const std::string &full_exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[LC_DIR_MAX_LEN] = {0};
    char tcwdpath[LC_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];

    std::string fullexename = full_exe_name;
    std::string exe_name ;
    std::string exe_path;
    get_path_fname( fullexename, exe_path, exe_name);

    //b_write_log(_DEBUG,"fusktest:fullexename[%s],exe_path[%s],exe_name[%s]",
    //        fullexename.c_str(), exe_path.c_str(), exe_name.c_str() );

    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name.c_str(), tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , LC_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, LC_DIR_MAX_LEN );
        if ( r < 0 || r >= LC_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name.c_str(), s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , LC_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, LC_DIR_MAX_LEN );
                if ( r < 0 || r >= LC_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path

        if ( strPathRun != exe_path )
        {
            continue;
        }

        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name.c_str(),tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




/*************************************
  *功能 ：执行shell程序
  * 输入：shell名、shell需要的参数
  * 输出：无
  *返回：
  *     　0         :成功，
  *       >0        :shell程序抛出的异常值
  *       <0        :失败
*************************************/
int ELFcmd::executeShell ( const char* shName_p, const char* szFormat_p, ... )
{
    if ( shName_p == NULL )
    {
        b_write_log( _ERROR, "input parm shName_p is NULL");
        return -1;
    }

    char  szParam[256]     = {0};
    char  szCommand[256 * 2] = {0};
    int   actLen           = 0;
    pid_t status           = 0;
    int   vRet             = 0;

    if ( szFormat_p != NULL )
    {
        va_list pvList;
        va_start ( pvList, szFormat_p );

        //把所有参数都存入szParam字符串
        actLen = vsprintf ( szParam, szFormat_p, pvList ); 


        if ( ( actLen < 0 ) || (  actLen >= 256 ) )
        {
            b_write_log( _ERROR,"actLen=[%d],par fix max len=[256]", actLen);

            return -2;
        }

        va_end ( pvList );

        sprintf ( szCommand, "%s %s", shName_p, szParam );
    }
    else
    {
        sprintf ( szCommand, "%s", shName_p );
    }

    sighandler_t old_handler;
    old_handler = signal ( SIGCHLD, SIG_DFL );

    status = system ( szCommand );

    signal ( SIGCHLD, old_handler );

    if ( status == -1 )
    {
        //system 在创建子进程等准备工作 失败
        b_write_log( _ERROR,"system Failed to create child processes and other preparations");
        return -3;
    }
    else
    {
        if ( ! ( WIFEXITED ( status ) ) )
        {
            //system 执行shell异常结束(如被强制kill掉)
            b_write_log( _ERROR,"system The execution shell ends abnormally (such as being forcibly killed)");
            return -4;
        }
        else
        {
            //vRet 一般为0 表示shell执行正常；其他情况则为shell中抛出的异常值
            vRet = WEXITSTATUS ( status );
            return vRet;
        }

    }

    return 0;
}




//kill -9 进程pid及其子进程
//  kill的顺序是按进程的生成顺序逆序进行的(即后生成的进程先kill,先生成的
//  后kill)
//killAllFlag: 1 子进程也kill; 其他值 只kill pid一个进程
//pid:       : 进程号
//status     : 返回状态: -2 正常结束进程, 其他值 异常情况
void ELFcmd::kill9Pbypid( const int &killAllFlag, const pid_t &pid, int &status )
{
    if ( killAllFlag == 1 )
    {
        char tPidFile[30] = {0};
        char tStr[1024] = {0};
        strcpy( tPidFile, ".plusTmp_XXXXXX" );

        int fd = mkstemp( tPidFile );           
        if ( fd < 0 )
        {
            b_write_log(_ERROR,"mkstemp(%s),errno[%d],errMsg=[%s]",
                    tPidFile, errno,strerror ( errno ) );

            status = -1;
            return;
            //break;

        }

        //snprintf ( tStr, sizeof ( tStr ),
        //           "pstree -np %d|awk -F'(' '{i = 2; while "
        //           "(i <= NF) {print $i;i++}}'|awk -F')' '"
        //           "{print $1}'|sort -r >%s",
        //           pid, tPidFile
        //         );

        //查询所有子进程号(包括进程号自己),按生成进程的逆序排列(即后生成的排在
        //前面,先生成的排在后面)
        snprintf ( tStr, sizeof ( tStr ),
                   "/bin/bash -c '"
                    "function F_printSubPid()"
                    "{"
                    "    local tout=$(pgrep -P \"$1\" 2>/dev/null);"
                    "    if [ -z \"${tout}\" ];then"
                    "        return 0;"
                    "    fi;"
                    "    local tnaa;"
                    "    echo \"${tout}\"|tac|while read tnaa;"
                    "    do"
                    "        F_printSubPid \"${tnaa}\";"
                    "        echo \"${tnaa}\";"
                    "    done;"
                    "    return 0;"
                    "};"
                   "F_printSubPid \"%d\" >\"%s\" && echo \"%d\" >>\"%s\"'",
                   pid,tPidFile, pid,tPidFile
                 );

        //b_write_log(_DEBUG,"tPidFile=[%s",tPidFile);
        system ( tStr );

        FILE* fp  = fdopen ( fd, "rb" );

        if ( fp == NULL )
        {
            b_write_log(_ERROR,"fdopen(%d,rb) error,errno[%d],errMsg=[%s]",
                    fd, errno,strerror ( errno )  );

            status = -1;
            return;
            //break;
        }

        int tPid = 0;
        int tNum = 0;
        int tFNum = 0;
        char tStrErr[100];
        memset ( tStrErr, 0, sizeof ( tStrErr ) );

        while ( fscanf ( fp, "%d", &tPid ) > 0 )
        {
            tNum++;

            if ( kill ( tPid, SIGKILL ) == 0 )
            {
                //b_write_log(_DEBUG,"fusktest kill[%d]",tPid);
                waitpid ( tPid, NULL, 0 );
            }
            else
            {
                tFNum++;
                snprintf ( tStrErr, sizeof ( tStrErr ),
                           "errno[%d],errMsg=[%s]", errno,
                           strerror ( errno ) );
            }
        }

        if ( fp != NULL )
        {
            fclose ( fp );
            fp = NULL;
        }

        unlink( tPidFile );

        if ( tNum > 0 && tNum == tFNum )
        {
            status = -3;

            b_write_log(_ERROR,"After the shell script execution time-out,the kill process is abnormal,%s",tStrErr);
        }
        else
        {
            status = -2;
        }

    }
    else
    {
        if ( kill ( pid, SIGKILL ) == 0 )
        {
            waitpid ( pid, NULL, 0 );
            status = -2;
            //b_write_log(_DEBUG,"fusktest kill[%d]",pid);
        }
        else
        {
            b_write_log(_ERROR,"After the shell script execution time-out,the kill process is abnormal,errno[%d],errMsg[%s]",errno, strerror ( errno ) );

            status = -3;
        }
    }

    return;
}





/*******************************************************************
  *功能 ：执行shell程序
  * 输入：
        *cmdStr_p,    需要执行的shell命令或shell脚本
        &timeout,     大于0表示超时时间(单位秒)等于0为不超时
        &times,       超时后需要重新执行的的次数
        &killAllFlag, 1:超时后kill脚本派生的所有进程;0只kill脚本进程
  *返回：
  *     　0           成功，
  *       >0 且!=127  shell程序抛出的异常值
  *       -1          失败
  *       -2          shell执行超时
  *       -3          shell执行超时后kill进程错误
  *注意:
  *     要想利用脚本回返值来判断脚本的执行逻辑,需要在脚本中显示的用
        exit value (value的值是需要小于127的正数)
********************************************************************/
int ELFcmd::system_plus ( const char* cmdStr_p, 
        const int& timeout, 
        const int& times, 
        const int& killAllFlag
        )
{
    if ( NULL == cmdStr_p )
    {
        b_write_log( _ERROR, "input parm cmdStr_p is NULL");
        return -1;
    }

    int tTimes = ( times > 0 ) ? times : 0;

    errno = 0;

    pid_t pid       = 0;
    int   status    = 0;
    bool  needRetry = true;

    struct sigaction ignore;
    struct sigaction saveintr;
    struct sigaction savequit;
    sigset_t chldmask;
    sigset_t savemask;

    memset ( &ignore,   0, sizeof ( ignore ) );
    memset ( &saveintr, 0, sizeof ( saveintr ) );
    memset ( &savequit, 0, sizeof ( savequit ) );
    memset ( &chldmask, 0, sizeof ( chldmask ) );
    memset ( &chldmask, 0, sizeof ( chldmask ) );


    ignore.sa_handler = SIG_IGN;
    sigemptyset ( &ignore.sa_mask );
    ignore.sa_flags = 0;

    if ( sigaction ( SIGINT, &ignore, &saveintr ) < 0 )
    {
        b_write_log( _ERROR,
                "sigaction SIGINT,errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    if ( sigaction ( SIGQUIT, &ignore, &savequit ) < 0 )
    {
        b_write_log( _ERROR,
                "sigaction SIGQUIT,errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    sigemptyset ( &chldmask );
    sigaddset ( &chldmask, SIGCHLD );

    if ( sigprocmask ( SIG_BLOCK, &chldmask, &savemask ) < 0 )
    {
        b_write_log( _ERROR,
                "sigprocmask SIG_BLOCK,errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    for ( int i = -1; i < tTimes && true == needRetry; i++ )
    {
        needRetry = false;

        if ( ( pid = fork() ) < 0 )
        {
            b_write_log( _ERROR,
                    "fork child process,errno[%d],errmsg[%s]", 
                    errno, strerror ( errno ) );

            status = -1;
        }
        else if ( 0 == pid )
        {
            sigaction ( SIGINT, &saveintr, NULL );
            sigaction ( SIGQUIT, &savequit, NULL );
            sigprocmask ( SIG_SETMASK, &savemask, NULL );

            execlp ( "sh", "sh", "-c", cmdStr_p, ( char* ) 0 );

            _exit ( 127 );

        }
        else
        {
            int leftTime = timeout;
            int rv;
            int options = ( timeout > 0 ) ? WNOHANG : 0 ;

            while ( ( rv = waitpid ( pid, &status, options ) ) <= 0 )
            {
                if ( -1 == rv && EINTR != errno )
                {
                    b_write_log(_ERROR,
                            "waitpid [%d] returns error,errno[%d],errmsg[%s]",
                            pid, errno, strerror ( errno ) );

                    status = -1;
                    break;
                }
                if ( timeout <= 0 ) continue;

                //sleep ( 1 );
                PauseThreadSleep( 1, 0 );

                leftTime--;

                if ( leftTime > 0 ) continue;

                kill9Pbypid( killAllFlag, pid, status );

                needRetry = true;
                break;
            }
        }//end 父进程
    }//end 重试

    if ( sigaction ( SIGINT, &saveintr, NULL ) < 0 )
    {
        b_write_log(_ERROR,
                "sigaction SIGINT ignore:errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    if ( sigaction ( SIGQUIT, &savequit, NULL ) < 0 )
    {
        b_write_log(_ERROR,
                "sigaction SIGQUIT ignore:errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    if ( sigprocmask ( SIG_SETMASK, &savemask, NULL ) < 0 )
    {
        b_write_log(_ERROR,
                "sigprocmask SIG_SETMASK savemask:errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    int tRetValue = ( WIFEXITED ( status ) ) ? ( WEXITSTATUS ( status ) ) : status;

    if ( tRetValue == 127 )
    {
        b_write_log(_ERROR,
                "child process execlp returns returns error");
    }

    return tRetValue;

}


//  mod null  判断文件或目录是否存在
//  mod "rwx" 判断文件或目录是否有相应权限
bool ELFcmd::fd_exist( const char* f_p ,const char* mode )
{
    if ( NULL == f_p ) return false;

    int tmd = F_OK;

    //b_write_log(_DEBUG,"[%s][%s][%d]",f_p,mode,tmd);

    if ( mode != NULL )
    {
        const char* l = NULL; const char* u = NULL;

        l = strchr(mode,'r'); u = strchr(mode, 'R');
        if ( l != NULL || u != NULL ) tmd |= R_OK;

        l = strchr(mode,'w'); u = strchr(mode, 'W');
        if ( l != NULL || u != NULL ) tmd |= W_OK;

        l = strchr(mode,'x'); u = strchr(mode, 'X');
        if ( l != NULL || u != NULL ) tmd |= X_OK;
    }

    //b_write_log(_DEBUG,"[%s][%s][%d]",f_p,mode,tmd);
    
    if ( 0 == ( access(f_p,tmd) ) ) return true;

    return false;
}

//chmod u+x
bool ELFcmd::chmod_ux( const char* f_p )
{
    if ( NULL == f_p ) return false;
    struct stat sb;

    if ( stat(f_p, &sb) == -1) return false;

    if ( (sb.st_mode & S_IXUSR) != S_IXUSR )
    {
        mode_t tmode = ( sb.st_mode | S_IXUSR );
        if ( chmod( f_p, tmode ) != 0 ) return false;
        return true;
    }

    return false;
}


bool ELFcmd::isFile( const char* f_p )
{
    if ( NULL == f_p ) return false;

    struct stat sb;

    if ( stat(f_p, &sb) == -1) return false;

    if ( (sb.st_mode & S_IFMT) != S_IFREG ) return false;

    return true;
}


bool ELFcmd::isDir( const char* f_p )
{
    if ( NULL == f_p ) return false;

    struct stat sb;

    if ( stat(f_p, &sb) == -1) return false;

    if ( (sb.st_mode & S_IFMT) != S_IFDIR ) return false;

    return true;
}


//mkdir -p dir
int ELFcmd::mkdirs( const std::string& dir )
{


    if ( isDir(dir.c_str()) ) return 0;

    std::size_t iIndex = 0;
    std::size_t iPos   = 0;
    std::string sub;

    mode_t mkmode = 0755;

    while ( ( iPos = dir.find('/', iIndex) ) != std::string::npos )
    {
        sub = dir.substr( 0, iPos+1 );
        if ( (!sub.empty()) && (!isDir(sub.c_str())) ) 
        {
            if ( 0 != (mkdir( sub.c_str(), mkmode )) )
            {
                b_write_log(_ERROR,"mkdir dir[%s]fail,errno[%d],errMsg[%s]",
                        sub.c_str(), errno, strerror(errno) );

                return -1;
            }
        }
        iIndex = iPos + 1;
    }

    //does not end with '/'
    if ( sub != dir )
    {
        if ( 0 != (mkdir( dir.c_str(), mkmode )) )
        {
            b_write_log(_ERROR,"mkdir dir[%s]fail,errno[%d],errMsg[%s]",
                    dir.c_str(), errno, strerror(errno) );

            return -1;
        }
    }

    return 0;
}

//rm -rf fds
int ELFcmd::rm( const char* fds )
{
    if ( NULL == fds ) return 0;

    if ( ! fd_exist( fds ) ) return 0;

    if ( ! isDir( fds ) ) 
    {
        //b_write_log(_DEBUG," remove [%s]", fds);
        return remove( fds );
    }

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    dirStream_p = opendir( fds );
    if ( dirStream_p == NULL ) return 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    ///current dir OR parrent dir
        {
            continue;
        }

        std::string tPathFile;
        if ( '/' == fds[strlen(fds)-1] )
        {
            tPathFile = std::string(fds) + std::string( dir_fp->d_name );
        }
        else
        {
            tPathFile = std::string(fds) + "/" + std::string( dir_fp->d_name );
        }

        //b_write_log(_DEBUG," into [%s]", tPathFile.c_str());

        rm( tPathFile.c_str() );
    }

    closedir( dirStream_p );

    //b_write_log(_DEBUG," remove [%s]", fds);
    return remove( fds );
}


//获得全路径文件名 中的 文件名 和 路径值
//  eg: /home/fusky/1.txt  ->   path=/home/fusky/   fname=1.txt
//获得路径值       中的 最后一级目录 和最后一级目录所在目录
//  eg: /home/fusky/   ->  path=/home   fname=fusky
//
int ELFcmd::get_path_fname(
        const std::string &src, 
        std::string &path, 
        std::string &fname
        )
{
    if ( src.empty() )
    {
        path  = ""; fname = "";
        return 0;
    }

    std::size_t found = src.rfind("/");
    if ( found != std::string::npos )
    {
        path  = src.substr(0,found);

        //b_write_log(_DEBUG,"found=[%ld],src.length=[%ld],path=[%s]",
        //        found,src.length(), path.c_str() );

        if ( found != (src.length() -1) ) 
        {
            fname = src.substr(found +1);
        }
        else
        {
            std::string tsrc = path;
            //fname = "";
            get_path_fname(tsrc, path, fname);
        }

        return 0;
    }

    path  = "./"; fname = src; 
    return 0;
}


int ELFcmd::mv( const char* src, const char* dst)
{
    if ( NULL == src || NULL == dst ) return -1;

    if ( ! fd_exist(src) ) return -2;

    bool dstIsDir = isDir(dst);
    //if ( fd_exist(dst) && (!dstIsDir) ) return -3;
    int ret = 0;

    if ( dstIsDir )
    {
        std::string tpath; std::string tfname;
        get_path_fname( src, tpath, tfname );

        std::string tdst;
        if ( dst[strlen(dst)-1] == '/' )
        {
            tdst = std::string(dst) + tfname;
        }
        else
        {
            tdst = std::string(dst) + "/" + tfname;
        }

        ret =  rename ( src, tdst.c_str() );
        if ( ret != 0 )
        {
            b_write_log(_ERROR,"rename(%s,%s) return[%d],errno[%d],errMsg[%s]",
                    src, tdst.c_str(), ret, errno, strerror(errno) );
            return -4;
        }

        return 0;

    }

    ret =  rename ( src, dst );
    if ( ret != 0 )
    {
        b_write_log(_ERROR,"rename(%s,%s) return[%d],errno[%d],errMsg[%s]",
                src, dst, ret, errno, strerror(errno) );
    }
    
    return 0;
}


//cp -r src  dst
int ELFcmd::cp( const char* src, const char* dst)
{
    if ( NULL == src || NULL == dst ) return -1;

    if ( ! fd_exist(src) ) return -2;

    if ( ! fd_exist(src,"r") ) return -3;

    bool dstExist = fd_exist(dst); 
    bool dstIsDir = isDir(dst);
    bool srcIsDir = isDir(src);

    //源为目录,目标必须为存在的目录
    if ( srcIsDir && ( ! dstExist ) ) return -4;
    if ( srcIsDir && ( ! dstIsDir ) ) return -5;

    std::string tsrc = src;
    std::string tdst = dst;

    //目标文件的上级目录必须存在
    if( ! dstExist )
    {
        std::string dstPath;  std::string dstFname;
        get_path_fname( tdst, dstPath, dstFname );
        if ( ! isDir(dstPath.c_str()) ) return -6;
    }


    //源是文件
    if ( ! srcIsDir )
    {
        char tmpbuf[MC_CBUFFER_SIZE] = {0};
        int lenR = 0;
        int lenW = 0;

        if ( dstIsDir )
        {
            std::string srcPath;  std::string srcFname;
            get_path_fname( tsrc, srcPath, srcFname );
            if ( dst[strlen(dst)-1] == '/' )
            {
                tdst += srcFname;
            }
            else
            {
                tdst += "/"; tdst += srcFname;
            }
        }

        //b_write_log(_DEBUG,": cp [%s] [%s]",src, tdst.c_str());
        
        FILE* fpR_p = NULL;
        FILE* fpW_p = NULL;
        if ( ( fpR_p = fopen ( src, "r" ) ) == NULL )
        {
            b_write_log(_ERROR," fopen(%s),errno[%d],errMsg[%s]",
                    src, errno, strerror ( errno ) );
            return -7;
        }

        if ( ( fpW_p = fopen ( tdst.c_str(), "w" ) ) == NULL )
        {
            fclose ( fpR_p );
            b_write_log(_ERROR," fopen(%s),errno[%d],errMsg[%s]",
                    tdst.c_str(), errno, strerror ( errno ) );
            return -8;
        }

        memset ( tmpbuf, 0, MC_CBUFFER_SIZE );

        while ( ( lenR = fread ( tmpbuf, 1, MC_CBUFFER_SIZE, fpR_p ) ) > 0 )
        {
            if ( ( lenW = fwrite ( tmpbuf, 1, lenR, fpW_p ) ) != lenR )
            {
                fclose ( fpR_p );
                fclose ( fpW_p );
                b_write_log(_ERROR,"fwrite[%s] error lenW=[%d],lenR=[%d]",
                        tdst.c_str(), lenW, lenR);

                return -9;
            }

            memset ( tmpbuf, 0, MC_CBUFFER_SIZE );
        }

        fclose ( fpR_p );
        fclose ( fpW_p );

        return 0;
    }


    //srcIsDir  dstIsDir
    tdst = dst;
    tsrc = src;
    std::string srcPath;  std::string srcFname;
    get_path_fname( tsrc, srcPath, srcFname );
    if ( dst[strlen(dst)-1] == '/' )
    {
        tdst += srcFname;
    }
    else
    {
        tdst += "/"; tdst += srcFname;
    }

    //建立目标目录
    int ret = mkdirs( tdst );
    if ( ret != 0 )
    {
        b_write_log(_ERROR,"mkdirs [%s] return[%d]", tdst.c_str(), ret);
        return -10;
    }

    //b_write_log(_DEBUG,": opendir [%s]",src);

    //遍历源目录进行拷贝
    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    dirStream_p = opendir( src );
    if ( dirStream_p == NULL )
    {
        b_write_log(_ERROR,"opendir [%s] errno[%d],errMsg[%s]", 
                src, errno, strerror ( errno ) );
        return -11;
    }

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    ///current dir OR parrent dir
        {
            continue;
        }

        tsrc = src;
        if ( src[strlen(src)-1] == '/' )
        {
            tsrc += std::string( dir_fp->d_name );
        }
        else
        {
            tsrc += "/"; 
            tsrc += std::string( dir_fp->d_name );
        }

        //b_write_log(_DEBUG,": will into cp [%s] [%s]",
        //        tsrc.c_str(), tdst.c_str());

        cp( tsrc.c_str(), tdst.c_str() );

    }

    closedir( dirStream_p );
    
    return 0;
}








ELFcmd::ELFcmd()
{

}




ELFcmd::~ELFcmd()
{

}




