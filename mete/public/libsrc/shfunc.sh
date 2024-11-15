#!/bin/bash

#############################################################################
#author       :    fushikai
#date         :    20190215
#linux_version:    Red Hat Enterprise Linux Server release 6.7 
#dsc          :   
#    Compare the differences between the two files,if any,back up the target
#       file and copy the source file to the target file
#
#eg           :
#   
#
#############################################################################

function getFnameOnPath() #get the file name in the path string
{
    if [ $# -ne 1 ];then
        echo "  Error: function getFnameOnPath input parameters not eq 1!"
        return 1
    fi

    allName="$1"
    if [ -z "${allName}" ];then
        echo "  Error: function getFnameOnPath input parameters is null!"
        return 2;
    fi

    slashNum=$(echo ${allName}|grep "/"|wc -l)
    if [ ${slashNum} -eq 0 ];then
        echo ${allName}
        return 0 
    fi

    fName=$(echo ${allName}|awk -F'/' '{print $NF}')
    echo ${fName}

    return 0
}

function getPathOnFname() #get the path value in the path string(the path does not have / at the end)
{

    if [ $# -ne 1 ];then
        echo "  Error: function getPathOnFname input parameters not eq 1!"
        return 1
    fi

    if [  -z "$1" ];then
        echo "  Error: function getPathOnFname input parameters is null!"
        return 2
    fi
    
    dirStr=$(echo "$1"|awk -F'/' '{for(i=1;i<NF;i++){printf "%s/",$i}}'|sed 's/\/$//g')
    if [ -z "${dirStr}" ];then
        dirStr="."
    fi

    echo "${dirStr}"
    return 0
}

function getlsattrI() #get immutable  (i) file attributes
{
    [ $# -ne 1 ] && return 1
    fOrd=$1
    [ ! -e "${fOrd}" ] && return 2

    tAtrtChr=$(lsattr -d ${fOrd}|awk '{print $1}'|cut -c5)

    echo "${tAtrtChr}"
    return 0
}

function addattrI() #add  immutable  (i) file attributes on a Linux file system
{
    [ $# -ne 1 ] && return 1
    fOrd=$1
    tAtrtChr=$(getlsattrI ${fOrd})
    ret=$?
    [ ${ret} -ne 0 ] && return ${ret}

    if [ "${tAtrtChr}" == "-" ];then
        echo "chattr +i ${fOrd}"
        chattr +i ${fOrd}
    fi

    return 0
}

function delattrI() #delete  immutable  (i) file attributes on a Linux file system
{
    [ $# -ne 1 ] && return 1
    fOrd=$1
    tAtrtChr=$(getlsattrI ${fOrd})
    ret=$?
    [ ${ret} -ne 0 ] && return ${ret}

    if [ "${tAtrtChr}" == "i" ];then
        echo "chattr -i ${fOrd}"
        chattr -i ${fOrd}
    fi

    return 0
}


function updateFile() #if the file $1 is different from the md5 code of $2 or $2 does not exist then cp $1 $2
{
    which md5sum &>/dev/null
    cmdstat=$?
    if [ ${cmdstat} -ne 0 ];then
        return 1
    fi
    if [ $# -ne 2 -a $# -ne 3 ];then
        return 2
    fi
    
    srcFile=$1
    dstFile=$2
    [ $# -eq 3 ] && bakdir="$3"
    
    if [ ! -f ${srcFile} ];then
        echo ""
        echo "----ERROR:file [${srcFile}] does not exist!"
        echo ""
        return 3
    fi
    if [ ! -f ${dstFile} ];then
        echo "cp ${srcFile} ${dstFile}"
        cp ${srcFile} ${dstFile}
        return 0
    fi
    
    srcmd5=$(md5sum ${srcFile}|awk '{print $1}')
    dstmd5=$(md5sum ${dstFile}|awk '{print $1}')
    #echo "----srcmd5=[${srcmd5}]"
    #echo "----dstmd5=[${dstmd5}]"
    if [[ "${srcmd5}" != "${dstmd5}" ]];then
        iflag=0 
        tAtrtChr=$(getlsattrI ${dstFile})
        ret=$?
        if [ ${ret} -ne 0 ];then
            iflag=0 
        elif [ "${tAtrtChr}" == "i" ];then
            iflag=1
            delattrI ${dstFile}
        fi
        
        if [ -d "${bakdir}" ];then
            bakF=$(getFnameOnPath "${dstFile}")
            ret=$?
            if [ ${ret} -eq 0 ];then
                tbakdir=$(echo "${bakdir}"|sed 's/\/$//g')
                timestamp=$(date +%Y%m%d%H%M%S)
                cp "${dstFile}" "${tbakdir}/${bakF}_${timestamp}"
                echo -e "\n\tback following:\tcp \"${dstFile}\" \"${tbakdir}/${bakF}_${timestamp}\"\n"
            fi

        fi

        echo "cp ${srcFile} ${dstFile}"
        cp ${srcFile} ${dstFile}
        if [ $? -ne 0 ];then
            echo " rm -rf ${dstFile}"
            \rm -rf "${dstFile}"
            cp ${srcFile} ${dstFile}
        fi

        if [ ${iflag} -eq 1 ];then
            addattrI ${dstFile}
        fi

        return 0
    else
        #echo -e "\n\tHint: [${srcFile}] is NOT DIFFERENT from [${dstFile}] and does not need to be copied!\n\n"
        return 100
    fi
}

function updateFile2() #updateFile2 <source_file> [<target_file>/<target_dir>]
{
    minInNum=2
    if [ $# -lt ${minInNum} ];then
        echo "  ERROR[1]: The inpput parameter of shell script [$0] should be no less than ${minInNum} !"
        return 1
    fi

    sFile=$1
    dFile=$2


    #verify the source file
    if [ ! -f "${sFile}" ];then
        echo "  ERROR[2]: Source file [${dFile}] type error!"
        return 2

    elif [ ! -r "${sFile}" ];then
        echo "  ERROR[3]: The source file [${dFile}] does not have permission to read!"
        return 3
    fi

    #verify the target file
    if [  -d "${dFile}" ];then
        if [ ! -w "${dFile}" ];then
            echo "  ERROR[4]: The target directory [${dFile}] does not have permission to write!"
            return 4
        fi
        if [ ! -x "${dFile}" ];then
            echo "  ERROR[5]: The target directory [${dFile}] does not have permission to executable!"
            return 5
        fi
        bakDir="${dFile}"
        tmpsFile=$(getFnameOnPath "${sFile}")
        tdDir=$(echo "${dFile}"|sed 's/\/$//g')
        tdFile="${tdDir}/${tmpsFile}"
        if [ -f "${tdFile}" ];then
            if [ ! -w "${tdFile}" ];then
                echo "  ERROR[6]: The target file [${tdFile}] does not have permission to write!"
                return 6
            fi
            dFile="${tdFile}"
        fi
    elif [ -f "${dFile}"  ];then
        if [ ! -w "${dFile}" ];then
            echo "  ERROR[7]: The target file [${dFile}] does not have permission to write!"
            return 7
        fi
        bakDir=$(getPathOnFname "${dFile}")
        retStat=$?
        if [ ${retStat} -eq 0 -a ! -d ${bakDir} ];then
            echo "  ERROR[8]: The directory [${bakDir}] of the target file [${dFile}] does not exists !"
            return 8
        elif [ ${retStat} -ne 0 ];then
            echo "  ERROR[9]: getPathOnFname \"${dFile}\" return message[${bakDir}]!"
            return 9
        fi
    elif [ ! -e "${dFile}"  ];then
        echo "  ERROR[10]: Target file [${dFile}] does not exist!"
        return 10
    else
        echo "  ERROR[11]: Target file [${dFile}] type error!"
        return 11
    fi

    updateFile "${sFile}" "${dFile}" "${bakDir}"
    retStat=$?
    exitStat=0
    [ ${retStat} -ne 100 ] && exitStat=${retStat}

    return ${exitStat}

}


