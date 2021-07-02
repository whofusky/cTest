#!/bin/bash
#
################################################################################
#
# Author : fusky
# Date   : 2020-11-29
# Dsc    : 将2.0气象下载器配置文件配置的下载脚本名修改为ftp 或 sftp
#
# Version:
#
################################################################################
#

baseDir="$(dirname $0)"


#定义用到的ftp 或 sftp 脚本名
ftpShs[0]="getFserStas.sh" ;       sftpShs[0]="getsftpSta.sh"   ;        
ftpShs[1]="getFileFtp.sh"  ;       sftpShs[1]="getFileSftp.sh"  ;       
ftpShs[2]="putFserStas.sh" ;       sftpShs[2]="putsftpSta.sh"   ;        
ftpShs[3]="putFileFtp.sh"  ;       sftpShs[3]="putFileSftp.sh"  ;       


#meteCfgFile="/fczx/wpfs10/mete/cfg/locaCfg.xml"
meteCfgFile="${baseDir}/locaCfg.xml"

if [ $# -ge 1 ];then
    meteCfgFile="$1"
fi

if [ ! -f "${meteCfgFile}" ];then
    echo -e "\n\t ERROR: meteCfgFile [ ${meteCfgFile} ] not exist!\n"
    exit 1
fi

doftpFlag=0



doNum=${#ftpShs[*]}
if [ ${doNum} -lt 1 ];then
    echo -e "\n\t 没有配置要修改的脚本名，请先修改[$0]脚本，添加要修改的sftp或ftp脚本名\n"
    exit 2
fi

prompctl="
        请输入如下数字，选择相应的操作:
            [1]. 将配置文件中的下载脚本修改为ftp
            [2]. 将配置文件中的下载脚本修改为sftp
            [3]. 退出
        你的选择是: "

while ((1))
do
    read -n 1 -p "${prompctl}" opType

    if [[ ${opType} -ne 1 && ${opType} -ne 2 && ${opType} -ne 3 ]];then
        echo ""
        echo "--------------------ERROR:--------:Input errors,please re-enter!"
        continue
    fi

    break

done

[ ${opType} -eq 3 ] && echo "" && exit 0

if [ ${opType} -eq 1 ];then
    doftpFlag=1
else
    doftpFlag=0
fi


echo -e "\n\t将要处理的气象配置文件为: [ ${meteCfgFile} ]\n"

if [ ${doftpFlag} -eq 1 ];then  
    echo -e "将配置文件配置的下载脚本修改为ftp"
    for((i=0;i<${doNum};i++))
    do
        sed -i "s/\b${sftpShs[$i]}\b/${ftpShs[$i]}/g" ${meteCfgFile}
    done
else  
    echo -e "将配置文件配置的下载脚本修改为sftp"
    for((i=0;i<${doNum};i++))
    do
        sed -i "s/\b${ftpShs[$i]}\b/${sftpShs[$i]}/g" ${meteCfgFile}
    done
fi

echo -e "\n\t[$0] exe complete!\n"
exit 0

