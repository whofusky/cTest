#!/bin/bash

#############################################################################
#author       :    fushikai
#date         :    20201120
#linux_version:    Red Hat Enterprise Linux Server release 6.7
#dsc          :
#    download files from sftp server
#
#invoke eg    :    
#   ./getFileSftp.sh 0 0 192.168.0.51 fczx fczx "/home/fczx/fushikai_test/sftp_dir/tdir1" "/home/fczx/tmp" "busilist_20200929_0.xml"
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
#shNAME="getFileSftp"

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
export logFile="${RMETEMAINP}/log/${shNAME}${logFNDate}.log"


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

#get temporary directory
tmpDir=${runDir}/tmpD
if [[ ! -d ${tmpDir} ]]; then
    mkdir -p ${tmpDir}
    writeLog 1 "${logTime} mkdir -p ${tmpDir}"
fi

if [ "${runDir}" == "." ];then
    prunDir=".."
else
    prunDir=$(dirname ${runDir})
fi

dRelaPath=filedo/down/back
dBackDir=${prunDir}/${dRelaPath}
if [ ! -d ${dBackDir} ];then
    if [ -f ${dBackDir} ];then
        mv ${dBackDir} ${dBackDir}.$(date '+%Y%m%d%H%M%S')
        writeLog 0 "${logTime} mv ${dBackDir} ${dBackDir}.$(date '+%Y%m%d%H%M%S')"
    fi
    mkdir -p ${dBackDir}
    writeLog 0 "${logTime} mkdir -p ${dBackDir}"
fi

opFlag=0 #0:download, 1:upload
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
sftpLdirAims=$5  #sftp client local directory
fileName=$6 #file name to be processed on the sftp server

sftpLdir=${tmpDir}

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
     ----sftpLdirAims=[${sftpLdirAims}]
     ----fileName  =[${fileName}]
     ----sftpCtrPNum=[${sftpCtrPNum}]
     ---------sftp para end----------
     "
writeLog 4 "${outmsg}"

sftpRet=$(getOrPutSftpFile "${opFlag}" "${trsType}" "${trsMode}" "${sftpIP}" "${sftpUser}" "${sftpPwd}" "${sftpRdir}" "${sftpLdir}" "${fileName}" "${sftpCtrPNum}")
retstat=$?

logTime="$(date '+%Y/%m/%d %H:%M:%S.%N')"
writeLog 1 "\n${logTime} 111retstat=[${retstat}],sftpRet=[${sftpRet}]\n"

declare -i mvFlag=0
declare -i rmFlag=0

#determine if the file is downloaded to the corresponding directory
numDFiles=$(ls -1 ${sftpLdir}/${fileName} 2>/dev/null|wc -l)
if [[ ${numDFiles} -gt 0 ]];then

    #delete files on the sftp server
    sftpRetD=$(echo "debug 3
        set net:timeout 4;set net:max-retries 4;set net:reconnect-interval-base 1;
        cd ./ || exit 1
        cd ${sftpRdir} || exit 2
        lcd ${sftpLdir} || exit 10
        mrm ${fileName} || exit 3
        bye"|lftp -u ${sftpUser},${sftpPwd} sftp://${sftpIP}:${sftpCtrPNum} 2>&1
    );
    sftpStaD=$?
    writeLog 1 "${logTime} numDFiles=[${numDFiles}],sftpStaD=[${sftpStaD}],sftpRetD=[${sftpRetD}]"
    
    #remove the carriage return contained in the file contents
    ls -1 ${sftpLdir}/${fileName}|while read trName
    do
        writeLog 8 "${logTime} ++++++${trName} "
        tr -d '\r' < "${trName}" >"${trName}.rtmp" && mv -f "${trName}.rtmp" "${trName}"
    done
    
     #move the downloaded file to the target directory
     declare -i opNum=0
     declare -i maxOpNum=100
     #ls -1 ${sftpLdir}/${fileName}|while read rfName
     for rfName in $(ls -1 ${sftpLdir}/${fileName})
     do
         mvFlag=0
         rmFlag=0
         opNum=0

         #  Multi-date source only needs to use a successfully downloaded data file

         #  Determine if the file exists
         trfName=$(getFnameOnPath ${rfName})
         statf=$?
         if [ ${funcFlag} -eq 1 ] && [ ${statf} -eq 0 ];then
             tmpRet=$(deterSameFile ${shDebugFlag} ${dBackDir} ${trfName})
            stat=$?
            if [ ${stat} -eq 2 ];then
                #if the file not exist,copy the file to the comparison directory first
                cp -f ${rfName} ${dBackDir}
                mvFlag=1
            elif [ ${stat} -eq 0 ];then
                #if the file already exists,delete the newly downloaded file
                rmFlag=1
            fi
            writeLog 8 "\n${tmpRet}\n"
         else
             mvFlag=1
         fi

         outmsg="${logTime} ++++fusktest++++mvFlag=[${mvFlag}],rmFlag=[${rmFlag}],trfName=[${trfName}],statf=[${statf}],stat=[${stat}] "
         writeLog 0 "\n${outmsg}\n"

         #delete file
         if [ ${rmFlag} -eq 1 ];then
             opNum=0
             rm -rf ${rfName}
             while [[ $? -ne 0 ]]
             do
                 let opNum++
                 if [ ${opNum} -gt ${maxOpNum} ];then
                    writeLog 0 "$(date '+%Y/%m/%d %H:%M:%S.%N') rm ${rfName} error"
                    break
                 fi
                 sleep 1
                 rm -rf ${rfName}
             done
            writeLog 0 "$(date '+%Y/%m/%d %H:%M:%S.%N') The file[${trfName}] downloaded from the [${sftpIP}:${sftpCtrPNum}${sftpRdir}]service already exists and will be deleted "
        fi

        #move files to the destination folder
         if [ ${mvFlag} -eq 1 ];then
             opNum=0
             mv ${rfName} ${sftpLdirAims}
             while [[ $? -ne 0 ]]
             do
                 let opNum++
                 if [ ${opNum} -gt ${maxOpNum} ];then
                    writeLog 0 "$(date '+%Y/%m/%d %H:%M:%S.%N') mv ${rfName} ${sftpLdirAims} error"
                    break
                 fi
                 sleep 1
                 mv ${rfName} ${sftpLdirAims}
             done
        fi

     done

#     if [ ${mvFlag] -eq 1 ];then
#         mv ${sftpLdir}/${fileName} ${sftpLdirAims}
#         while [[ $? -ne 0 ]]
#         do
#             let opNum++
#             if [ ${opNum} -gt ${maxOpNum} ];then
#                writeLog 0 "$(date '+%Y/%m/%d %H:%M:%S.%N') mv ${sftpLdir}/${fileName} ${sftpLdirAims} error"
#                break
#             fi
#             sleep 1
#             mv ${sftpLdir}/${fileName} ${sftpLdirAims}
#         done
#     fi
    
    #delte expired files
    rmExName="busilist_*"
    rmExDay=5
    if [ ${funcFlag} -eq 1 ];then
        rmExpiredFile "${dBackDir}" "${rmExDay}" "${rmExName}" >>${logFile} 2>&1
    fi
    
    writeLog 16 "$(date '+%Y/%m/%d %H:%M:%S.%N') rmFlag=[${rmFlag}], ${NOWSHNAME}:$#:end \n"
    if [ ${rmFlag} -eq 1 ];then
        exit 3
    else
        exit 0
    fi
else
    writeLog 16 "$(date '+%Y/%m/%d %H:%M:%S.%N') ${NOWSHNAME}:$#:unsuccessfull \n"
    exit 2
fi


exit 0

