#!/bin/bash
. /home/fczx/.bash_profile

#描述：此脚本用于fs2.0气象下载器程序部署时建立主目录下的子目录，并生成配置文件
#条件：已经建立程序包部署的主目录，如整个程序部署到/home/fczx/IIImete目录
#      则此脚本应入在/home/fczx/IIImete目录下运行

#获得当前目录
PWD_DIR=$(pwd) 
CUR_DIR=$(dirname ${PWD_DIR})
cd ${CUR_DIR}

#需要在当前目录下建立的固定目录变量
RUN_BIN=bin
CFG_DIR=cfg
SH_DIR=sh
LOG_DIR=log
WINP=filedo/winplan
WINPY=filedo/winplan/yes
WINPD=filedo/winplan/do
DOWNP=filedo/down
DOWNPS2=filedo/down/ser2
DOWNPY=filedo/down/yes
DOWNPS1=filedo/down/ser1
HEALTHP=filedo/health
HEALTHPIY=filedo/health/integyes
HEALTHPI=filedo/health/integ
HEALTHPF=filedo/health/frmII
HEALTHPY=filedo/health/yes
HEALTHPD=filedo/health/do
HEALTHPU=filedo/health/up
#反隔发送的文件夹，需要修改成实际的目录
MIDDLEP=${CUR_DIR}/filedo/middle
STALP=filedo/statList
STALPT=filedo/statList/trig

#执行建立目录动作
if [ ! -d ${RUN_BIN} ];then
    mkdir -p ${RUN_BIN}
fi
if [ ! -d ${CFG_DIR} ];then
    mkdir -p ${CFG_DIR}
fi 
if [ ! -d ${SH_DIR} ];then
    mkdir -p ${SH_DIR}
fi
if [ ! -d ${LOG_DIR} ];then
    mkdir -p ${LOG_DIR}
fi
if [ ! -d ${WINP} ];then
    mkdir -p ${WINP}
fi
if [ ! -d ${WINPY} ];then
    mkdir -p ${WINPY}
fi
if [ ! -d ${WINPD} ];then
    mkdir -p ${WINPD}
fi
if [ ! -d ${DOWNP} ];then
    mkdir -p ${DOWNP}
fi
if [ ! -d ${DOWNPS2} ];then
    mkdir -p ${DOWNPS2}
fi
if [ ! -d ${DOWNPY} ];then
    mkdir -p ${DOWNPY}
fi
if [ ! -d ${DOWNPS1} ];then
    mkdir -p ${DOWNPS1}
fi
if [ ! -d ${HEALTHP} ];then
    mkdir -p ${HEALTHP}
fi
if [ ! -d ${HEALTHPIY} ];then
    mkdir -p ${HEALTHPIY}
fi
if [ ! -d ${HEALTHPI} ];then
    mkdir -p ${HEALTHPI}
fi
if [ ! -d ${HEALTHPF} ];then
    mkdir -p ${HEALTHPF}
fi
if [ ! -d ${HEALTHPY} ];then
    mkdir -p ${HEALTHPY}
fi
if [ ! -d ${HEALTHPD} ];then
    mkdir -p ${HEALTHPD}
fi
if [ ! -d ${HEALTHPU} ];then
    mkdir -p ${HEALTHPU}
fi
if [ ! -d ${MIDDLEP} ];then
    mkdir -p ${MIDDLEP}
fi
if [ ! -d ${STALP} ];then
    mkdir -p ${STALP}
fi
if [ ! -d ${STALPT} ];then
    mkdir -p ${STALPT}
fi

downFtpIP=192.168.0.154
downFtpPort=21
downFtpUser=Administrator
downFtpPwd=qwer1234
downFtpCycle=20
downTiemOut=92000
downFtpDir=/tmp

upFtpIP=192.168.0.154
upFtpPort=21
upFtpUser=Administrator
upFtpPwd=qwer1234
upFtpCycle=20
upTiemOut=20000
upFtpDir=/up

#配置文件名
CFGFILE=${CUR_DIR}/${CFG_DIR}/locaCfg.xml.ck
>${CFGFILE}

#生成配置文件
echo '<?xml version="1.0" encoding="UTF-8" standalone="no" ?>' >>${CFGFILE}
echo '<!-- 气象服务器本地配置-->' >>${CFGFILE}
echo '<ToDo>' >>${CFGFILE}
echo '    <logCfg> haha <!--日志配置 -->' >>${CFGFILE}
echo '        <debugFlag>1</debugFlag> <!--0 不打印dubug,1打印debug -->' >>${CFGFILE}
echo '        <errlog>' >>${CFGFILE}
echo '            <path>'${CUR_DIR}/${LOG_DIR}'</path>' >>${CFGFILE}
echo '            <name>err</name>' >>${CFGFILE}
echo '        </errlog>' >>${CFGFILE}
echo '        <mainLog>' >>${CFGFILE}
echo '            <path>'${CUR_DIR}/${LOG_DIR}'</path>' >>${CFGFILE}
echo '            <name>Main</name>' >>${CFGFILE}
echo '        </mainLog>' >>${CFGFILE}
echo '        <getFtpLog>' >>${CFGFILE}
echo '            <path>'${CUR_DIR}/${LOG_DIR}'</path>' >>${CFGFILE}
echo '            <name>other</name>' >>${CFGFILE}
echo '        </getFtpLog>' >>${CFGFILE}
echo '    </logCfg>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    <shTool> <!--公共shell工具-->' >>${CFGFILE}
echo '        <getPidNum>'${CUR_DIR}/${SH_DIR}'/getPidNum.sh</getPidNum>' >>${CFGFILE}
echo '    </shTool>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    <GetDown ftpnum="1"> <!--业务清单下载配置 -->' >>${CFGFILE}
echo '        <Cycle> <!--空闲时，循环检查是否有下载文件时间间隔设置-->' >>${CFGFILE}
echo '            <second>'${downFtpCycle}'</second>' >>${CFGFILE}
echo '            <microSecond>0</microSecond>'  >>${CFGFILE}
echo '            <repet>-1</repet> <!--重复次数:大于0为具体的重复次数，小于0为无限循环-->' >>${CFGFILE}
echo '        </Cycle>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <FtpSer ip="'${downFtpIP}'" port="'${downFtpPort}'" user="'${downFtpUser}'" pwd="'${downFtpPwd}'" timeoutMS="'${downTiemOut}'">' >>${CFGFILE}
echo '            <ChkShName>'${CUR_DIR}/${SH_DIR}'/getFserStas.sh</ChkShName>' >>${CFGFILE}
echo '            <DoShName>'${CUR_DIR}/${SH_DIR}'/getFileFtp.sh</DoShName>' >>${CFGFILE}
echo '            <RemoteDir>'${downFtpDir}'</RemoteDir>' >>${CFGFILE}
echo '            <LocalDir>'${CUR_DIR}/${DOWNPS1}'</LocalDir>' >>${CFGFILE}
echo '            <FileDes num="1">' >>${CFGFILE}
echo '                <FileName>busilist_*.xml</FileName>' >>${CFGFILE}
echo '            </FileDes>' >>${CFGFILE}
echo '        </FtpSer>' >>${CFGFILE}
echo '    </GetDown>'     >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    <PutUp ftpnum="1"> <!--健康报告上传 -->' >>${CFGFILE}
echo '        <Cycle> <!--往上传目录移动文件的时间间隔，上传只要有文件就上传，所以此时间间隔也是控制上传的频率-->' >>${CFGFILE}
echo '            <second>'${upFtpCycle}'</second>' >>${CFGFILE}
echo '            <microSecond>0</microSecond>' >>${CFGFILE}
echo '            <repet>-1</repet> <!--重复次数:大于0为具体的重复次数，小于0为无限循环-->' >>${CFGFILE}
echo '        </Cycle>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <FtpSer ip="'${upFtpIP}'" port="'${upFtpPort}'" user="'${upFtpUser}'" pwd="'${upFtpPwd}'" timeoutMS="'${upTiemOut}'">' >>${CFGFILE}
echo '            <ChkShName>'${CUR_DIR}/${SH_DIR}'/putFserStas.sh</ChkShName>' >>${CFGFILE}
echo '            <DoShName>'${CUR_DIR}/${SH_DIR}'putFileFtp.sh</DoShName>' >>${CFGFILE}
echo '            <RemoteDir>'${upFtpDir}'</RemoteDir>' >>${CFGFILE}
echo '            <LocalDir>'${CUR_DIR}/${HEALTHPU}'</LocalDir>' >>${CFGFILE}
echo '            <FileDes num="1">' >>${CFGFILE}
echo '                <FileName>"intehealth20170720.xml"</FileName>' >>${CFGFILE}
echo '            </FileDes>' >>${CFGFILE}
echo '        </FtpSer>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    </PutUp>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    <healthRp>' >>${CFGFILE}
echo '        <fieldName>fusk_qx_xz1</fieldName> <!--场站名称此值暂定配置文件配，场内前台UI有配置此便-->' >>${CFGFILE}
echo '        <hostNmae>qx_pc</hostNmae> <!--宿主名称-->' >>${CFGFILE}
echo '        <theMonitor>气象服务子系统 </theMonitor> <!--监视者-->' >>${CFGFILE}
echo '    </healthRp> ' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    <doList> <!--业务清单处理配置 -->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <monInteFlag>1</monInteFlag> <!--是否启用综合监视器标识(综合监视器即一个监视器监视多个目录):1启用,0不启用 -->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <srcList>'${CUR_DIR}/${DOWNPY}'</srcList> <!--ftp下载完成后转移至此目录，也是业务清单处理的触发目录-->' >>${CFGFILE}
echo '        <toAnti>'${MIDDLEP}'</toAnti> <!--反隔摆渡文件路径 -->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <staLisTrig>'${CUR_DIR}/${STALPT}'</staLisTrig> <!--业务清单提供情况 任务触发目录 -->' >>${CFGFILE}
echo '        <doStaPath>'${CUR_DIR}/${STALP}'</doStaPath> <!--业务清单提供情况生成目录 -->' >>${CFGFILE}
echo '        <doStaName>stat</doStaName> <!--业务清单提供情况文件名前缀 -->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <doWPlanPath>'${CUR_DIR}/${WINPD}'</doWPlanPath> <!--III区测风塔采集业务处理计划xml生成目录 -->' >>${CFGFILE}
echo '        <doWPlanName>winPlan</doWPlanName> <!--III区测风塔采集业务处理计划xml文件名前缀 -->' >>${CFGFILE}
echo '        <cmpWPlanPath>'${CUR_DIR}/${WINPY}'</cmpWPlanPath> <!-- 测风采集计划生成完毕目录，也是测风数据处理的触发目录 -->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <doHPath>'${CUR_DIR}/${HEALTHPD}'</doHPath> <!--生成健康报告目录 -->' >>${CFGFILE}
echo '        <preNameH>health</preNameH> <!--健康报告文件名前缀 -->' >>${CFGFILE}
echo '        <yesHPath>'${CUR_DIR}/${HEALTHPY}'</yesHPath> <!--健康报告完成目录 也是综合健康报告触发目录 -->' >>${CFGFILE}
echo '        <frmIIHPath>'${CUR_DIR}/${HEALTHPF}'</frmIIHPath> <!--场内上传的健康报叿也是综合健康报告触发目录-->' >>${CFGFILE}
echo '        <integHPath>'${CUR_DIR}/${HEALTHPI}'</integHPath> <!--综合健康报告生成目录-->' >>${CFGFILE}
echo '        <preNameInte>intehealth</preNameInte> <!--综合健康报告文件名前缀 -->' >>${CFGFILE}
echo '        <nameInteRegex>intehealth[0-9]{8,8}.xml</nameInteRegex> <!--综合健康报告文件名POSIX正则表达式 -->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '        <yesIntegHPath>'${CUR_DIR}/${HEALTHPIY}'</yesIntegHPath> <!--综合健康报告完成目录-->' >>${CFGFILE}
echo '        <upHPath>'${CUR_DIR}/${HEALTHPU}'</upHPath> <!--综合健康报告上传目录-->' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    </doList>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '    <windMeasure> <!--测风塔本地配置-->' >>${CFGFILE}
echo '        <accThree>0</accThree> <!--是否从III区接入测风塔业务 -->' >>${CFGFILE}
echo '    </windMeasure>' >>${CFGFILE}
echo ' ' >>${CFGFILE}
echo '</ToDo>' >>${CFGFILE}

exit 0


