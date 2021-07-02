#!/bin/bash

#. ./meteShFunc.sh

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

#deterSameFile 15 ${backDir} $@
#stat=$?

#echo "stat=[${stat}]"

tmpStr=fczx/busi22
fName=$(getFnameOnPath ${tmpStr})
stat=$?
echo "fName=[${fName}]"
echo "stat=[${stat}]"
fPath=$(getPathOnFname ${tmpStr})
stat=$?
echo "fPath=[${fPath}]"
echo "stat=[${stat}]"


#20181114-0929
logFile=/fczx/wpfs10/mete/sh/test.log
rmExpiredFile "/home/fczx/tmp/tt" 5 "busi*" >>${logFile} 2>&1
stat=$?
echo " rmExpiredFile stat=[${stat}]"

