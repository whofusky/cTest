#!/bin/bash

#fusk
#2018/02/02
#自定义工具，目的是更新库文件到相应程序目录

#echo $#
#echo $0
#echo $1"--"

baseDir=$(dirname $0)
#operPath=/home/fczx/src
operPath="${baseDir}/../.."

fncFile=${baseDir}/shfunc.sh

#Load shell function file
. ${fncFile}

echo -e "\n开始更新库文件到相应目录,【如无更新输出则说明没有需要更新的】\n"

find ${operPath}/public/libsrc/lib  -name "*.a" -print|while read aa      
do
    find ${operPath} -path ${operPath}'/public/libsrc/lib' -prune -o -name $(basename $aa) -print|while read bb
    do
        updateFile2 "${aa}" "${bb}"
    done
done 

echo -e "\nscript [$0] execution completed !!\n"
exit 0

