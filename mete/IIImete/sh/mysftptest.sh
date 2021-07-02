#!/bin/bash
#
#use lftp call sftp test

#. ./meteShFunc.sh

shName="$0"
oShName=${shName##*/}
oPreShName=${oShName%.*}

echo "oShName=[${oShName}],oPreShName=[${oPreShName}]"

runDir=$(dirname $0)
if [ "${runDir}" == "." ];then
	prunDir=".."
else
	prunDir=$(dirname ${runDir})
fi

echo "runDir=[${runDir}]"
echo "prunDir=[${prunDir}]"

relaPath=filedo/down/back
backDir=${prunDir}/${relaPath}
if [ ! -d ${backDir} ];then
	echo "mkdir -p ${backDir}"
	mkdir -p ${backDir}
fi
funcFlag=0
diyFuncFile=${runDir}/meteShFunc.sh
if [ -f ${diyFuncFile} ];then
    . ${diyFuncFile}
    funcFlag=1
fi
echo "funcFlag=[${funcFlag}]"


ftpUser=root
ftpPwd=root
ftpIP=192.168.0.198
ftpCtrPNum=22
sftpRdir=zfmd1
sftpLdir=tmp

#lftp  -e "debug 3;set net:timeout 10;set net:max-retries 2;set net:reconnect-interval-base 10;" -u ${ftpUser},${ftpPwd} sftp://${ftpIP}:${ftpCtrPNum}
#lftp  -e "debug 3;set net:timeout 10;set net:max-retries 2;set net:reconnect-interval-base 10;set cmd:fail-exit ture;set cache:cache-empty-listings false" -u ${ftpUser},${ftpPwd} sftp://${ftpIP}:${ftpCtrPNum}
function F_getsftpstatus()
{
    echo $(date +%s)
    sftpRet=$(echo "debug 3
        set net:timeout 3;set net:max-retries 1;set net:reconnect-interval-base 1;
        cd ./ || exit 1
        cd ${sftpRdir} || exit 2
        lcd ${sftpLdir} || exit 10
        cls -1 *
        bye"|lftp -u ${ftpUser},${ftpPwd} sftp://${ftpIP}:${ftpCtrPNum} 2>&1
    )
    echo "stp ret=[$?]"
    echo $(date +%s)
        echo "sftpRet=[${sftpRet}]" 

    return 0
}

function F_test1()
{
    local i=1
    for((;i<10;))
    do
        echo "i=[$i]"
        let i++
    done
    return 
}


F_getsftpstatus
#F_test1

echo "$0 exe compaletely!"

exit 0
