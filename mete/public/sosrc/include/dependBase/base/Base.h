#ifndef _BASE_H_
#define _BASE_H_



#ifndef __u_char_defined
    typedef unsigned char   u_char;
    typedef unsigned short  u_short;
    typedef unsigned int    u_int;
    typedef unsigned long   u_long;
#define __u_char_defined
#endif

#define Int32  int
#define UInt32 unsigned int
#define U8     unsigned char
#define I8     char
#define Bool   unsigned char

#ifndef True
#define True   1
#endif

#ifndef False
#define False  0
#endif


void PauseThreadSleep(const int& sec, const int& us);
void getFormatTime( char* buf, int bufLen, const char* timeFormat="%Y-%m-%d_%H:%M:%S", int addSeconds=0 );


#endif//_BASE_H_
