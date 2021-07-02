#!/bin/bash

#############################################################################
#author       :    fushikai
#date         :    20201120
#linux_version:    Red Hat Enterprise Linux Server release 6.7
#dsc          :
#    Upload files to sftp server
#
#invoke eg    :    
#   ./putFileSftp.sh  0 0 192.168.0.51 fczx fczx "/home/fczx/fushikai_test/sftp_dir/up" "/home/fczx/tmp/up" "busilist_20201010_0.xml"
#
#version chg  :
#   20201120@created
#
#############################################################################


. ~/.bash_profile >/dev/null 2>&1

runDir=$(dirname $0)

#load sh func
funcFlag=0
diyFuncFile=${runDir}/meteShFunc.sh
if [ ! -f ${diyFuncFile} ];then
    exit 1
else
    . ${diyFuncFile}
    funcFlag=1
fi

exeShName="$0"
oShName=${exeShName##*/}
oPreShName=${oShName%.*}

NOWSHNAME="${oShName}"
shNAME="${oPreShName}"
#shNAME="putFileSftp"

#print log level:identifiable level 2 N-th power combination
#shDebugFlag=16
shDebugFlag=255

logTime="$(date '+%Y/%m/%d %H:%M:%S.%N')"
logFNDate="$(date '+%Y%m%d')"

#function outShDebugMsg() #out put $4 to $1; use like: outShDebugMsg $logfile $cfgdebug $valdebug $putcontent $clearflag

#if you need a personalized log directory,you need to configure the environment
#   varible "RMETEMAINP",for exampl RMETEMAINP=/home/fczx
#if you do not configure the default directory as the log folder under the upper
#   directory
if [[ -z ${RMETEMAINP} ]]; then
    RMETEMAINP=$(dirname $(dirname $0))
    if [[ ! -d ${RMETEMAINP}/log ]]; then
        mkdir -p ${RMETEMAINP}/log
        if [[ $? -eq 0  ]]; then
            outmsg="${logTime} mkdir -p ${RMETEMAINP}/log"
            outShDebugMsg "${RMETEMAINP}/log/${shNAME}${logFNDate}.log" ${shDebugFlag} 0 "${outmsg}" 0
        fi
    fi
fi
logFile=${RMETEMAINP}/log/${shNAME}${logFNDate}.log


function writeLog()
{
    local valDebug="$1"
    local outmsg="$2"
    local clearFlag=0
    [ $# -eq 3 ] && clearFlag=$3

    #echo -e "-------logFile=[${logFile}]"
    outShDebugMsg "${logFile}" "${shDebugFlag}" "${valDebug}" "${outmsg}" "${clearFlag}"
    local ret=$?
    return ${ret}
}


writeLog 16 "\n${logTime} ${NOWSHNAME}:$#:start -->"


outmsg="${logTime} --debug:input param nums:$#
                   --logFile=[${logFile}]"
writeLog 2 "${outmsg}"


if [[ $# -ne 6 && $# -ne 7 && $# -ne 8 && $# -ne 9 ]];then
    outmsg="${logTime} input error,please input like this:
                 ${NOWSHNAME} <sftpIP> <sftpUser> <sftpPwd> <sftpRdir> <sftpLdir> <fileName>
                 or
                 ${NOWSHNAME} <trsType> <trsMode> <sftpIP> <sftpUser> <sftpPwd> <sftpRdir> <sftpLdir> <fileName>"
    writeLog 0 "${outmsg}" 2
    exit 1
fi

opFlag=1 #0:download, 1:upload
trsType=0 #0:ascii ,1:binary
trsMode=0 #0:passive mode for data transfers, 1:active mode for data transfers
if [ $# -eq 8 -o $# -eq 9 ];then
    trsType=$1
    shift
    trsMode=$1
    shift
fi

tcheck=$(echo "${trsType}"|sed -n "/^[0-1]$/p"|wc -l)
[ ${tcheck} -eq 0 ] && trsType=0

tcheck=$(echo "${trsMode}"|sed -n "/^[0-1]$/p"|wc -l)
[ ${tcheck} -eq 0 ] && trsMode=0


sftpIP=$1
sftpUser=$2
sftpPwd=$3
sftpRdir=$4  #directory on the server
sftpLdir=$5  #sftp client local directory
fileName=$6 #file name to be processed on the sftp server

#default port number
if [[ $# -eq 6 ]];then
    sftpCtrPNum=22
else
    sftpCtrPNum=$7
fi  

if [ ${opFlag} -eq 0 ];then
    opStr="downlaod"
    opCmd="mget"
else
    opStr="upload"
    opCmd="mput"
fi
if [ ${trsType} -eq 0 ];then
    typeStr="ascii"
else
    typeStr="binary"
fi

#This variable is useless and is set for compatibilit with ftp
if [ ${trsMode} -eq 0 ];then
    modeStr="passive"
    modeOpt="-p"
else
    modeStr="active"
    modeOpt="-A"
fi


outmsg="${logTime} ${NOWSHNAME} input param
     shDebugFlag=[${shDebugFlag}]
     opFlag=[${opFlag}],opStr=[${opStr}],opCmd=[${opCmd}]
     trsType=[${trsType}],typeStr=[${typeStr}]
     trsMode=[${trsMode}],modeStr=[${modeStr}],modeOpt=[${modeOpt}]
     logFile=[${logFile}]
     ---------sftp para begine--------
     ----sftpIP     =[${sftpIP}]
     ----sftpUser   =[${sftpUser}]
     ----sftpPwd    =[${sftpPwd}]
     ----sftpRdir   =[${sftpRdir}]
     ----sftpLdir   =[${sftpLdir}]
     ----fileName  =[${fileName}]
     ----sftpCtrPNum=[${sftpCtrPNum}]
     ---------sftp para end----------
     "
writeLog 4 "${outmsg}"

sftpRet=$(getOrPutSftpFile "${opFlag}" "${trsType}" "${trsMode}" "${sftpIP}" "${sftpUser}" "${sftpPwd}" "${sftpRdir}" "${sftpLdir}" "${fileName}" "${sftpCtrPNum}")
retstat=$?

logTime="$(date '+%Y/%m/%d %H:%M:%S.%N')"
writeLog 8 "\n${logTime} retstat=[${retstat}],sftpRet=[${sftpRet}]\n"

if [ "${retstat}" -eq 0 ];then
    writeLog 16 "${logTime} ${NOWSHNAME}:$#:end \n"
else
    writeLog 16 "${logTime} ${NOWSHNAME}:$#:unsuccessfull \n"
fi


exit ${retstat}

