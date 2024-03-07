#!/bin/bash
#
##############################################################################
#author    :    fu.sky
#date      :    2022-10-08_16:03:02
#desc      :
#               更新当前目录下配置在do_file_include,do_file_src,do_file_other中的文件
#               到上层目录其他文件夹下
#               (更新条件: 其他目录有且md5码不一至才替换)
#
#
##############################################################################
#

#需要更新include的头文件
#do_file_include[0]=""

#需要更新的src源码文件
#do_file_src[0]=""

#需要更新的其他文件(相对当前目录)
#do_file_other[0]=""
#do_file_other[1]=""



tPwd=$(pwd)
tCurDirLast="${tPwd##*/}"
#echo "${tCurDirLast}"
baseDir=$(dirname $0)
findDir="${baseDir}/../"



do_include_num=${#do_file_include[*]}
do_src_num=${#do_file_src[*]}
do_other_num=${#do_file_other[*]}




function F_getFileName()
{
    #local fname="$1"
    echo "${1##*/}"
}


function F_lsSrcFile()
{

    local i
    for((i=0;i<${do_include_num};i++));do
        ls -1 "${baseDir}/include/"${do_file_include[$i]} 2>/dev/null
    done
    for((i=0;i<${do_src_num};i++));do
        ls -1 "${baseDir}/src/"${do_file_src[$i]} 2>/dev/null
    done
    for((i=0;i<${do_other_num};i++));do
        ls -1 "${baseDir}/"${do_file_other[$i]} 2>/dev/null
    done

}


function F_lsDstFile()
{
    local fname="$1"
    fname=$(F_getFileName "${fname}")

    #echo "${FUNCNAME}:fname=[${fname}]"
    find "${findDir}" -name "${fname}" -print  2>/dev/null|grep -v "/${tCurDirLast}/"

}

function F_updateFile()
{
    local srcFile="$1"
    local dstFile="$2"
    [ -z "${srcFile}" ] && return 1
    [ -z "${dstFile}" ] && return 1
    if [ "x${srcFile}" = "x${dstFile}" ];then
        return 0
    fi
    if [[ ! -f "${srcFile}" || ! -f "${dstFile}" ]];then
        return 1
    fi
    
    local srcMd5; local dstMd5;
    srcMd5=$(md5sum "${srcFile}"|awk '{print $1}')
    dstMd5=$(md5sum "${dstFile}"|awk '{print $1}')

    #echo -e "${FUNCNAME}:INFO:srcMd5=[${srcMd5}]"
    #echo -e "${FUNCNAME}:INFO:dstMd5=[${dstMd5}]"
    if [[ "x${srcMd5}" != "x${dstMd5}" ]];then
        echo -e "\t${FUNCNAME}:INFO:[cp ${srcFile} ${dstFile}]"
        \cp "${srcFile}" "${dstFile}"
    fi

}

function F_doUpdate()
{
    local i; local j;
    for i in $(F_lsSrcFile)
    do
        #echo "${FUNCNAME}:debug:for:[$i]"
        for j in $(F_lsDstFile "$i")
        do
            F_updateFile "$i" "$j"
        done
    done

}

function F_test()
{
    echo "baseDir=[${baseDir}]"
    echo "findDir=[${findDir}]"
    for i in $(F_lsSrcFile)
    do
        echo "${FUNCNAME}:debug:for:[$i]"
        F_lsDstFile "$i"
    done

}
main()
{
    #F_test
    F_doUpdate
}
main
