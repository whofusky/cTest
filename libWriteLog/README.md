# libWriteLog说明

此目录下的源码是自己闲来没事用c/c++写的，用于方便写日志的功能接口；
适用环境为Linux 操作系统，如果是windows操作系统，可能需要将
源码作少许修改才能用。

## 文件夹下目录结构说明

```
.
├── include
│   └── WriteLog.h    #写日志功能代码的头文件
├── src
│   └── WriteLog.cpp  #写日志功能的c/c++源码
├── Makefile          #makefile文件，方便将源码封装成.a或.so的库文件
├── out_lib
│   ├── libWriteLog.a #用make mylib=libWriteLog.a 将写日志接口封装成库文件
│   └── libWriteLog.so#用make myso=libWriteLog.so 将写日志接口封装成库文件
├── main.cpp         #为了测试写日志功能代码写的临时测试代码
├── bin
│   └── tt           #用make myexe=tt编译的测试代码可执行文件
└── README.md        #markdown 格式的readme文件
```

## 关于写日志功能的说明

```
/******************************************************************************/

// 说明:

// 1.日志默认输出到程序运行同级目录下的 LOG/$YYYY/$MM/$DD.log
//
//         其中$YYYY: 为运行程序的时刻对应的年份
//             $MM  : 为运行程序的时刻对应的月份
//             $DD  : 为运行程序的时刻对应的日
//
//         例如：当2021-06-14 这一天运行程序则会生成
//               LOG/2021/06/14.log


// 2.不采用默认输出日志路径时需要用WriteLogInit的接口进行设置

// 3.日志输出控制通过logLevel控制，初始设置此值有2种方法:
//   (1) 在程序中用WriteLogInit接口进行设置
//   (2) 在程序运行的同级目录下新建立文件LogLevel.ini
//       文件内容为LogLevel = 7 其中7为日志级别，数值
//       参考logLevel enum定义

// 4.如果不设置logLevel，则所有的日志都默认输出

// 5. logLevel的控制逻辑是 小于等于 logLevel的日志级别都会输出
//    例如: logLevel 设置为LOGERROR 则在日志输出中如下日志都会输出
//              LOGERROR,LOGCRIT,LOGALERT,LOGEMERG

// 6. 代码中有获取程序当前运行路径名的buf定义是1024，因此当前只支持
//    程序运行绝对路径名字符串长度小于1024的可执行程序输出日志


/******************************************************************************/
```

