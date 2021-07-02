#pragma once
#ifndef __APP_RL_DEF
#define __APP_RL_DEF
/*////////////////////////////////////////////////////////////////////
 规则文件解析中最大支持10个组类，每组类有元素数由1至10的共个组，合计100组，
 目前只实现了其中6组。对于不同应用的检查，可改写这些组，改写时应尽量保证组
 内元素数一致，即将元素多的放到可容纳元素多的组中，元素不足容纳数时，用空串
 占位，当元素数超过所有组的最大容纳数时，需要扩展为新组。当要检查需要的实际
 内容多于6组时，也需要将超出部分扩展成新组，扩展新组时需要在规则文件解析代
 码中增加相应处理特别要注意改写时保证OPTGRP中组枚举值的定义与实际扩展的一致。
 以下定义的空间为预分配空间，需要专用接口为其设定专用值，以满足对不同应用领
 域规则文件的限制需要
////
//
-----用于预测系统2.0的应用规则------>*/
/*const char* RL_ID = "sysCfg_Rl";
const char* TGT_ID = "root";
#define INFOFIELD_NUM       (3)
const char* infoLvlNm[INFOFIELD_NUM] = { "sysInfo_Rl","hstInfo_Rl","appInfo_Rl" };
*//*
-----用于预测系统2.0的应用规则------>*/
const char* RL_ID = "";// = "dataStgCfg_Rl";
const char* TGT_ID = "";// = "root";
unsigned char infoFieldNum = 0;
#define INFOFIELD_NUM       (10)
const char* infoLvlNm[INFOFIELD_NUM] = { "", "", "", "", "", "", "", "", "", ""};

//第1组类：OPTGRP=1~10
const char* opt1_1Cnst[1] = { "" };//optGrp1--元素数1
const char* opt1_2Cnst[2] = { "", "" }; //optGrp1--元素数2
const char* opt1_3Cnst[3] = { "", "", "" }; //optGrp1--元素数3
const char* opt1_4Cnst[4] = { "", "", "", "" }; //optGrp1--元素数4
const char* opt1_5Cnst[5] = { "", "", "", "", "" }; //optGrp1--元素数5
const char* opt1_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp1--元素数6
const char* opt1_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp1--元素数7
const char* opt1_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp1--元素数8
const char* opt1_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp1--元素数9
const char* opt1_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp1--元素数10

//第2组类：OPTGRP=11~20
const char* opt2_1Cnst[1] = { "" };//optGrp2--元素数1
const char* opt2_2Cnst[2] = { "", "" }; //optGrp2--元素数2
const char* opt2_3Cnst[3] = { "", "", "" }; //optGrp2--元素数3
const char* opt2_4Cnst[4] = { "", "", "", "" }; //optGrp2--元素数4
const char* opt2_5Cnst[5] = { "", "", "", "", "" }; //optGrp2--元素数5
const char* opt2_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp2--元素数6
const char* opt2_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp2--元素数7
const char* opt2_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp2--元素数8
const char* opt2_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp2--元素数9
const char* opt2_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp2--元素数10

//第3组类：OPTGRP=21~30
const char* opt3_1Cnst[1] = { "" };//optGrp3--元素数1
const char* opt3_2Cnst[2] = { "", "" }; //optGrp3--元素数2
const char* opt3_3Cnst[3] = { "", "", "" }; //optGrp3--元素数3
const char* opt3_4Cnst[4] = { "", "", "", "" }; //optGrp3--元素数4
const char* opt3_5Cnst[5] = { "", "", "", "", "" }; //optGrp3--元素数5
const char* opt3_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp3--元素数6
const char* opt3_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp3--元素数7
const char* opt3_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp3--元素数8
const char* opt3_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp3--元素数9
const char* opt3_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp3--元素数10

//第4组类：OPTGRP=31~40
const char* opt4_1Cnst[1] = { "" };//optGrp4--元素数1
const char* opt4_2Cnst[2] = { "", "" }; //optGrp4--元素数2
const char* opt4_3Cnst[3] = { "", "", "" }; //optGrp4--元素数3
const char* opt4_4Cnst[4] = { "", "", "", "" }; //optGrp4--元素数4

const char* opt4_5Cnst[5] = { "", "", "", "", "" }; //optGrp4--元素数5
const char* opt4_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp4--元素数6
const char* opt4_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp4--元素数7
const char* opt4_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp4--元素数8
const char* opt4_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp4--元素数9
const char* opt4_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp4--元素数10

//第5组类：OPTGRP=41~50
const char* opt5_1Cnst[1] = { "" };//optGrp5--元素数1
const char* opt5_2Cnst[2] = { "", "" }; //optGrp5--元素数2
const char* opt5_3Cnst[3] = { "", "", "" }; //optGrp5--元素数3
const char* opt5_4Cnst[4] = { "", "", "", "" }; //optGrp5--元素数4
const char* opt5_5Cnst[5] = { "", "", "", "", "" }; //optGrp5--元素数5
const char* opt5_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp5--元素数6
const char* opt5_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp5--元素数7
const char* opt5_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp5--元素数8
const char* opt5_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp5--元素数9
const char* opt5_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp5--元素数10

//第6组类：OPTGRP=51~60
const char* opt6_1Cnst[1] = { "" };//optGrp6--元素数1
const char* opt6_2Cnst[2] = { "", "" }; //optGrp6--元素数2
const char* opt6_3Cnst[3] = { "", "", "" }; //optGrp6--元素数3
const char* opt6_4Cnst[4] = { "", "", "", "" }; //optGrp6--元素数4
const char* opt6_5Cnst[5] = { "", "", "", "", "" }; //optGrp6--元素数5
const char* opt6_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp6--元素数6
const char* opt6_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp6--元素数7
const char* opt6_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp6--元素数8
const char* opt6_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp6--元素数9
const char* opt6_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp6--元素数10

//第7组类：OPTGRP=61~70
const char* opt7_1Cnst[1] = { "" };//optGrp7--元素数1
const char* opt7_2Cnst[2] = { "", "" }; //optGrp7--元素数2
const char* opt7_3Cnst[3] = { "", "", "" }; //optGrp7--元素数3
const char* opt7_4Cnst[4] = { "", "", "", "" }; //optGrp7--元素数4
const char* opt7_5Cnst[5] = { "", "", "", "", "" }; //optGrp7--元素数5
const char* opt7_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp7--元素数6
const char* opt7_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp7--元素数7
const char* opt7_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp7--元素数8
const char* opt7_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp7--元素数9
const char* opt7_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp7--元素数10

//第8组类：OPTGRP=71~80
const char* opt8_1Cnst[1] = { "" };//optGrp8--元素数1
const char* opt8_2Cnst[2] = { "", "" }; //optGrp8--元素数2
const char* opt8_3Cnst[3] = { "", "", "" }; //optGrp8--元素数3
const char* opt8_4Cnst[4] = { "", "", "", "" }; //optGrp8--元素数4
const char* opt8_5Cnst[5] = { "", "", "", "", "" }; //optGrp8--元素数5
const char* opt8_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp8--元素数6
const char* opt8_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp8--元素数7
const char* opt8_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp8--元素数8
const char* opt8_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp8--元素数9
const char* opt8_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp8--元素数10

//第9组类：OPTGRP=81~90
const char* opt9_1Cnst[1] = { "" };//optGrp9--元素数1
const char* opt9_2Cnst[2] = { "", "" }; //optGrp9--元素数2
const char* opt9_3Cnst[3] = { "", "", "" }; //optGrp9--元素数3
const char* opt9_4Cnst[4] = { "", "", "", "" }; //optGrp9--元素数4
const char* opt9_5Cnst[5] = { "", "", "", "", "" }; //optGrp9--元素数5
const char* opt9_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp9--元素数6
const char* opt9_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp9--元素数7
const char* opt9_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp9--元素数8
const char* opt9_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp9--元素数9
const char* opt9_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp9--元素数10

//第10组类：OPTGRP=91~100
const char* opt10_1Cnst[1] = { "" };//optGrp10--元素数1
const char* opt10_2Cnst[2] = { "", "" }; //optGrp10--元素数2
const char* opt10_3Cnst[3] = { "", "", "" }; //optGrp10--元素数3
const char* opt10_4Cnst[4] = { "", "", "", "" }; //optGrp10--元素数4
const char* opt10_5Cnst[5] = { "", "", "", "", "" }; //optGrp10--元素数5
const char* opt10_6Cnst[6] = { "", "", "", "", "", "" }; //optGrp10--元素数6
const char* opt10_7Cnst[7] = { "", "", "", "", "", "", "" }; //optGrp10--元素数7
const char* opt10_8Cnst[8] = { "", "", "", "", "", "", "", "" }; //optGrp10--元素数8
const char* opt10_9Cnst[9] = { "", "", "", "", "", "", "", "", "" }; //optGrp10--元素数9
const char* opt10_10Cnst[10] = { "", "", "", "", "", "", "", "", "" }; //optGrp10--元素数10


/*<------不同系统的规则头文件容----
////////////////////////////////////////////////////////////////////*/
#endif
