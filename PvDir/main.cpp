#include <stdio.h>
#include "PvDir.h"

int main()
{

    int tRet = 0;
    const char* tmp_p=NULL;
    for(int i=PV_DIR_APPSENDP1;i<PV_DIR_COUNT;i++)
    {
        tmp_p=GetPvPubDir( i, &tRet );
        printf("pubfile:[%d],[%s],tRet=[%d],retMsg=[%s]\n",i,tmp_p,tRet,PvStrError(tRet));
    }

    //ERROR
    int i=-1;
    tmp_p=GetPvPubDir( i, &tRet );
    printf("pubfile:[%d],[%s],tRet=[%d],retMsg=[%s]\n",i,tmp_p,tRet,PvStrError(tRet));

    i=21;
    tmp_p=GetPvPubDir( i, &tRet );
    printf("pubfile:[%d],[%s],tRet=[%d],retMsg=[%s]\n",i,tmp_p,tRet,PvStrError(tRet));

    for(int i=PV_PUBCFG_FILE;i<PV_PUBFILE_COUNT;i++)
    {
        tmp_p=GetPvPubFile( i, &tRet );
        printf("pub cfg file :[%d],[%s],tRet=[%d]\n",i,tmp_p,tRet);
    }
    for(int i=RUNPAR_ID1;i<RUNPAR_COUNT;i++)
    {
        tmp_p=GetPvRunPar( i, &tRet );
        printf("app run par :[%d],[%s],tRet=[%d]\n",i,tmp_p,tRet);
    }
    

    return 0;
}
