#!/bin/bash

#fusk
#2018/02/02
#自定义工具，目的是更新库文件到相应程序目录

#echo $#
#echo $0
#echo $1"--"

#basePath=/home/fczx/src
basePath=/home/fusky/mygit/cTest/mete

echo ""
echo "开始更新库文件到相应目录\n"
echo ""

if [ $# -eq 1 ];then
    echo "begine update $1 ------->"
    find ${basePath} -path ${basePath}'/public/libsrc/lib' -prune -o -name $1 -print|while read bb
    do
    echo "before update state:"$(ls -l $bb)
    cp $1 $bb
    if [ $? -eq 0 ];then
	echo "update sucess :     "$(ls -l $bb)
    else
	echo "update err"
    fi
    echo ""
    done

else

ls -1 *.a|while read aa      
do
    echo "begine update $aa ------->"
    find ${basePath} -path ${basePath}'/public/libsrc/lib' -prune -o -name $aa -print|while read bb
    do
    echo "before update state:"$(ls -l $bb)
    cp $aa $bb
    if [ $? -eq 0 ];then
         echo "update sucess :     "$(ls -l $bb)
    else
        echo "update err"
    fi
    echo ""
    done

    echo ""
    echo ""
done 

fi
