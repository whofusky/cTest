#!/bin/bash

WORK_DIR=$(PWD)


###################################################
##BINDIR:bin文件目录
BINDIR=$(WORK_DIR)

##
###################################################

SYSINCL = -I /usr/include
PUBINCL = -I include -I include/libh
OTHERINCL = -I include/ThrMeteM
CUSTOMS_HEAD = -I include/other

SYSLIBS = -lpthread
OSOURLIBS = -Llib -ltinyxml2 -ljson_linux-gcc-9_libmt
PUBLIBS = -Llib -lFileMonitor -lEventManager \
     -lInfra -lBase
OTHERLIBS = -Llib -lhlthRprtWrtXml -lmete -lbaseFunc  
CUSTOMS_LIB =

#动态连接库
#DYNLIB = -L$(WORK_DIR)/lib -lmetesh
DYNLIB = 

INCLUDES = $(SYSINCL) $(PUBINCL) $(OTHERINCL) $(CUSTOMS_HEAD)
LIBS = $(CUSTOMS_LIB) $(OTHERLIBS) $(PUBLIBS) $(OSOURLIBS) $(SYSLIBS) $(DYNLIB)
##编译时头文件和链接库

###################################################
GFLAG = -g -Wall
##编译参数

###################################################
XCC=g++ $(GFLAG)
##主要编译命令

###################################################

OBJ_DIR := compileTmpFiles/

#######################ThrMeteM  begine###########################
TARGET_BIN := ThrMeteM
SRC_DIR := src/ThrMeteM

SRC_CPP_FILES :=
SRC_CPP_FILES += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))

SRC_O_FILES :=
SRC_O_FILES += $(patsubst %.cpp, %.o, $(SRC_CPP_FILES))

TARGET_O_FILES :=
TARGET_O_FILES := $(foreach file, $(SRC_O_FILES), $(OBJ_DIR)$(file))

TARGET_O_DIR :=
TARGET_O_DIR += $(foreach dir, $(SRC_DIR), $(OBJ_DIR)$(dir))


THR_OPERATION_DEPEND := dir_depend
$(TARGET_BIN) : $(THR_OPERATION_DEPEND) $(TARGET_O_FILES)
	$(XCC) -o $(BINDIR)/$@ $(TARGET_O_FILES) $(LIBS)

$(THR_OPERATION_DEPEND) :
	$(foreach dir, $(TARGET_O_DIR), $(shell mkdir -p $(dir)))
$(TARGET_O_FILES) : $(SRC_O_FILES)
$(SRC_O_FILES) : %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $< -o $(OBJ_DIR)$@
#######################ThrMeteM  end###########################
 
#######################Mtst  begine###########################
TARGET_BIN_Mtst := Mtst
SRC_DIR_Mtst := src/Mtst

SRC_CPP_FILES_Mtst :=
SRC_CPP_FILES_Mtst += $(foreach dir, $(SRC_DIR_Mtst), $(wildcard $(dir)/*.cpp))

SRC_O_FILES_Mtst :=
SRC_O_FILES_Mtst += $(patsubst %.cpp, %.o, $(SRC_CPP_FILES_Mtst))

TARGET_O_FILES_Mtst :=
TARGET_O_FILES_Mtst := $(foreach file, $(SRC_O_FILES_Mtst), $(OBJ_DIR)$(file))

TARGET_O_DIR_Mtst :=
TARGET_O_DIR_Mtst += $(foreach dir, $(SRC_DIR_Mtst), $(OBJ_DIR)$(dir))

THR_OPERATION_DEPEND_Mtst := dir_depend_Mtst
$(TARGET_BIN_Mtst) : $(THR_OPERATION_DEPEND_Mtst) $(TARGET_O_FILES_Mtst)
	$(XCC) -o $(BINDIR)/$@ $(TARGET_O_FILES_Mtst) $(LIBS)

$(THR_OPERATION_DEPEND_Mtst) :
	$(foreach dir, $(TARGET_O_DIR_Mtst), $(shell mkdir -p $(dir)))
$(TARGET_O_FILES_Mtst) : $(SRC_O_FILES_Mtst)
$(SRC_O_FILES_Mtst) : %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $< -o $(OBJ_DIR)$@
#######################Mtst  end###########################

#######################stopG  begine###########################
TARGET_BIN_stopG := stopG
SRC_DIR_stopG := src/stopG

SRC_CPP_FILES_stopG :=
SRC_CPP_FILES_stopG += $(foreach dir, $(SRC_DIR_stopG), $(wildcard $(dir)/*.cpp))

SRC_O_FILES_stopG :=
SRC_O_FILES_stopG += $(patsubst %.cpp, %.o, $(SRC_CPP_FILES_stopG))

TARGET_O_FILES_stopG :=
TARGET_O_FILES_stopG := $(foreach file, $(SRC_O_FILES_stopG), $(OBJ_DIR)$(file))

TARGET_O_DIR_stopG :=
TARGET_O_DIR_stopG += $(foreach dir, $(SRC_DIR_stopG), $(OBJ_DIR)$(dir))

THR_OPERATION_DEPEND_stopG := dir_depend_stopG
$(TARGET_BIN_stopG) : $(THR_OPERATION_DEPEND_stopG) $(TARGET_O_FILES_stopG)
	$(XCC) -o $(BINDIR)/$@ $(TARGET_O_FILES_stopG) $(LIBS)

$(THR_OPERATION_DEPEND_stopG) :
	$(foreach dir, $(TARGET_O_DIR_stopG), $(shell mkdir -p $(dir)))
$(TARGET_O_FILES_stopG) : $(SRC_O_FILES_stopG)
$(SRC_O_FILES_stopG) : %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $< -o $(OBJ_DIR)$@
#######################stopG  end###########################

ALL_TARGET_BIN := $(TARGET_BIN) $(TARGET_BIN_Mtst) $(TARGET_BIN_stopG)

all: $(ALL_TARGET_BIN)

	

.PHONY: clean
clean:
	@echo ""
	-rm -rf $(OBJ_DIR)
	-rm -f $(ALL_TARGET_BIN)
	@echo ""
	
