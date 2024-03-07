#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//查询所有子进程id并不包括进程自己的id
void query_all_child_pid( char* tPid, char* tPidFile )
{
    char shStr[1024];
    memset( shStr, 0, sizeof(shStr) );
    int ret = 0;

    snprintf ( shStr, sizeof ( shStr ),
               "/bin/bash -c '"
                "function F_printSubPid()"
                "{"
                "    local tout=$(pgrep -P \"$1\");"
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
               "F_printSubPid \"%s\" >\"%s\"'",
               tPid,tPidFile
             );

    ret = system ( shStr );
    printf ("\n system(%s) return[%d]\n", shStr, ret);

    return;
}


//查询所有子进程id并包括进程自己的id
void query_all_child_pid2( char* tPid, char* tPidFile )
{
    char shStr[1024];
    memset( shStr, 0, sizeof(shStr) );
    int ret = 0;

    snprintf ( shStr, sizeof ( shStr ),
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
               "F_printSubPid \"%s\" >\"%s\" && echo \"%s\" >>\"%s\"'",
               tPid,tPidFile, tPid,tPidFile
             );

    ret = system ( shStr );
    printf ("\n system(%s) return[%d]\n", shStr, ret);

    return;
}




int main(int argc, char* argv[]) {

    //int ret;

    //for (int count = 0; count < argc; count++)
    //    printf("%d: %s \n", count, argv[count]);

    char tPidFile[30] = {0};
    char tPid[30] = {0};

    strcpy( tPidFile, "./testout.txt" );
    strcpy( tPid, "11473" );
    //strcpy( tPid, "123" );

    query_all_child_pid2( tPid, tPidFile );

    return 0;
}


