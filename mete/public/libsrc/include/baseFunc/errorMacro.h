#ifndef _ERRORMACRO_H_
#define _ERRORMACRO_H_

#define ERR_POINTER_NULL     -0x01  //指针变量为空报错
#define ERR_RETURN_NULL      -0x02  //返回指针变量为空报错
#define ERR_XML_CONFIG       -0x03  //xml配置错误
#define ERR_XML_LOAD         -0x04  //加载xml文件错误
#define ERR_INPUT_OR_CONF    -0x05 //输入或配置错误
#define ERR_INPUT_VALUE      -0x06 //输入值错误
#define ERR_FTP_CONNECT      -0x07 //ftp连接错误
#define ERR_FTP_USERPWD      -0x08 //ftp用户名或密码错误
#define ERR_FTP_RPATH        -0x09 //ftp服务器路径错误
#define ERR_FTP_RNOFILE      -0x0A //ftp服务器无文件
#define ERR_FTP_LNOFILE      -0x0B //ftp本地无文件
#define ERR_FTP_TIMEOUT      -0x0C //ftp超时
#define ERR_SYS_FUNCRET      -0x0D //系统函数返回错误
#define ERR_DIY_FUNCRET      -0x0D //自定义函数返回错误
#define ERR_XML_VALUE        -0x0E //xml的配置值错误
#define ERR_XML_ADDNODE      -0x0F //xml添加节点错误
#define ERR_FTP_OUTTIME      -0x10 //FTP超时



#endif // _ERRORMACRO_H_
