#!/bin/bash
#
##############################################################################
#author    :    fu.sky
#date      :    2022-10-08_16:03:02
#desc      :
#               更新输出库文件目录(out_lib)下的库文件到上次目录其他文件夹下
#               (更新: 其他目录有且md5码不一至才替换)
#
#
##############################################################################
#

baseDir=$(dirname $0)
srcDir="${baseDir}/out_lib"
findDir="${baseDir}/../"

function F_getFileName()
{
    local fname="$1"
    echo "${fname##*/}"
    return 0
}

function F_lsSrcFile()
{
    [ ! -d "${srcDir}" ] && return 1

    ls -1 "${srcDir}/"lib* 2>/dev/null
    find "${baseDir}/include"  "*.h" -print 2>/dev/null|grep -v "/noRecursive/"

    return 0
}


function F_lsDstLib()
{
    local fname="$1"
    fname=$(F_getFileName "${fname}")
    #echo "${FUNCNAME}:fname=[${fname}]"
    find "${findDir}" -name "${fname}" -print  2>/dev/null|grep -v '/out_lib/'

    return 0
}

function F_updateFile()
{
    local srcFile="$1"
    local dstFile="$2"
    [ -z "${srcFile}" ] && return 1
    [ -z "${dstFile}" ] && return 1
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

    return 0
}

function F_doUpdate()
{
    local i; local j;
    for i in $(F_lsSrcFile)
    do
        #echo "${FUNCNAME}:debug:for:[$i]"
        for j in $(F_lsDstLib "$i")
        do
            F_updateFile "$i" "$j"
        done
    done

    return 0
}

function F_test()
{
    echo "baseDir=[${baseDir}]"
    echo "srcDir=[${srcDir}]"
    echo "findDir=[${findDir}]"
    for i in $(F_lsSrcFile)
    do
        echo "${FUNCNAME}:debug:for:[$i]"
        F_lsDstLib "$i"
    done

    return 0
}
main()
{
    #F_test
    F_doUpdate
    return 0
}
main
