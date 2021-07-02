#!/bin/bash
.SUFFIXES:.cpp .i .s .o

###################################################
HOMEDIR=$(HOME)
PUBDIR=$(HOMEDIR)/public
BINDIR=$(HOMEDIR)/bin
##主要内容说明
##HOMEDIR:用户主目录
##PUBDIR:公共函数代码目录
##BINDIR:bin文件目录
##
###################################################

SYSINCL = -I/usr/include
PUBINCL = -I$(PUBDIR)/include
OTHERINCL = -I$(HOMEDIR)/include
CUSTOMS_HEAD =

PUBLIBS = -L$(PUBDIR)/lib
OTHERLIBS =
CUSTOMS_LIB =

#动态连接库
DYNLIB =

INCLUDES = $(SYSINCL) $(PUBINCL) $(OTHERINCL) $(CUSTOMS_HEAD)
LIBS = $(PUBLIBS) $(OTHERLIBS) $(CUSTOMS_LIB) $(DYNLIB)
##编译时头文件和链接库

###################################################
GFLAG = -g -Wall -fPIC
##编译参数

###################################################
XCC=g++ $(GFLAG)
##主要编译命令

###################################################


###################################################
REMIND_MSG ="input ERROR,please input like this: make Rtest"

_ALL:
	@echo $(REMIND_MSG)

#.cpp预处理	
.cpp.i:
	$(XCC) $(INCLUDES) -E $< -o $(<:.cpp=.i)

#对预处理文件.i生成汇编代码.s
.i.s:
	$(XCC) -S $< -o $(<:.i=.s)

#将汇编文件.s编译为目标文件.o
.s.o:
	$(XCC) -c $< -o $(<:.s=.o)	
    
#.cpp为自己编译代码
.cpp.o:
	@echo ""
	$(XCC) $(INCLUDES) -E $< -o $(<:.cpp=.i)
	$(XCC) $(INCLUDES) -S $(<:.cpp=.i) -o $(<:.cpp=.s)	
	$(XCC) -c $(<:.cpp=.s) -o $(<:.cpp=.o)	

clean:
	@echo ""
	rm -f *.i
	rm -f *.s	
	rm -f *.o	
	@echo ""	
	
cls:
	@echo ""
	rm -f *.i
	rm -f *.s	
	rm -f *.o	
	@echo ""
######################################################
all:libmetesh.so
so:libmetesh.so

OBJ1=soLib.o

######################################################

#将 目标文件.o 生成动态库
libmetesh.so:$(OBJ1)
	g++ -shared -o $(PUBDIR)/libso/libmetesh.so $(OBJ1)

	
