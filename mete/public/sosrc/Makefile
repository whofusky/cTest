#!/bin/bash

WORK_DIR=$(PWD)

#LIB_DIR := ../lib/
LIB_DIR := solib/
STATIC_LIB_SUFFIX := .so
###################################################
GFLAG = -g -Wall -fPIC
##编译参数
################################################### 
XCC=g++ $(GFLAG)
CCS=g++ -shared
AR=ar
##主要编译命令
################################################### 

SYSINCL = -I /usr/include
PUBINCL = -I include/dependBase
OTHERINCL =  -I include/toolh
CUSTOMS_HEAD = -I include

INCLUDES = $(SYSINCL) $(PUBINCL) $(OTHERINCL) $(CUSTOMS_HEAD)

####################################################
OBJ_DIR := compileTmpFiles/

#########################make test begine###########################
CUSTOMS_HEAD_test := -I include/test
LIB_NAME_test := test
LIB_TARGET_test := $(LIB_DIR)lib$(LIB_NAME_test)$(STATIC_LIB_SUFFIX)

SRC_DIR_test := src/test

SRC_CPP_Files_test := 
SRC_CPP_Files_test += $(foreach dir, $(SRC_DIR_test), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_test := 
SRC_OBJ_Files_test += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_test))
TARGET_OBJ_Files_test :=
TARGET_OBJ_Files_test += $(foreach file, $(SRC_OBJ_Files_test), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_test :=
TARGET_OBJ_DIR_test += $(foreach file, $(SRC_DIR_test), $(OBJ_DIR)$(file))

OPERATION_DEPEND_test := test_dir_depend
$(LIB_TARGET_test): $(OPERATION_DEPEND_test) $(TARGET_OBJ_Files_test)
	$(CCS) -o $(LIB_TARGET_test) $(TARGET_OBJ_Files_test)
$(OPERATION_DEPEND_test):
	$(foreach dir, $(TARGET_OBJ_DIR_test), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_test): $(SRC_OBJ_Files_test)
$(SRC_OBJ_Files_test): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_test) $< -o $(OBJ_DIR)$@
#########################test end###########################

#########################make APDU begine###########################
CUSTOMS_HEAD_APDU := -I include/APDU
LIB_NAME_APDU := APDU
LIB_TARGET_APDU := $(LIB_DIR)lib$(LIB_NAME_APDU)$(STATIC_LIB_SUFFIX)

SRC_DIR_APDU := src/APDU

SRC_CPP_Files_APDU := 
SRC_CPP_Files_APDU += $(foreach dir, $(SRC_DIR_APDU), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_APDU := 
SRC_OBJ_Files_APDU += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_APDU))
TARGET_OBJ_Files_APDU :=
TARGET_OBJ_Files_APDU += $(foreach file, $(SRC_OBJ_Files_APDU), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_APDU :=
TARGET_OBJ_DIR_APDU += $(foreach file, $(SRC_DIR_APDU), $(OBJ_DIR)$(file))

OPERATION_DEPEND_APDU := APDU_dir_depend
$(LIB_TARGET_APDU): $(OPERATION_DEPEND_APDU) $(TARGET_OBJ_Files_APDU)
	$(CCS) -o $(LIB_TARGET_APDU) $(TARGET_OBJ_Files_APDU)
$(OPERATION_DEPEND_APDU):
	$(foreach dir, $(TARGET_OBJ_DIR_APDU), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_APDU): $(SRC_OBJ_Files_APDU)
$(SRC_OBJ_Files_APDU): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_APDU) $< -o $(OBJ_DIR)$@
#########################APDU end###########################


#########################make MODBUS begine###########################
CUSTOMS_HEAD_MODBUS := -I include/MODBUS
LIB_NAME_MODBUS := MODBUS
LIB_TARGET_MODBUS := $(LIB_DIR)lib$(LIB_NAME_MODBUS)$(STATIC_LIB_SUFFIX)

SRC_DIR_MODBUS := src/MODBUS

SRC_CPP_Files_MODBUS := 
SRC_CPP_Files_MODBUS += $(foreach dir, $(SRC_DIR_MODBUS), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_MODBUS := 
SRC_OBJ_Files_MODBUS += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_MODBUS))
TARGET_OBJ_Files_MODBUS :=
TARGET_OBJ_Files_MODBUS += $(foreach file, $(SRC_OBJ_Files_MODBUS), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_MODBUS :=
TARGET_OBJ_DIR_MODBUS += $(foreach file, $(SRC_DIR_MODBUS), $(OBJ_DIR)$(file))

OPERATION_DEPEND_MODBUS := MODBUS_dir_depend
$(LIB_TARGET_MODBUS): $(OPERATION_DEPEND_MODBUS) $(TARGET_OBJ_Files_MODBUS)
	$(CCS) -o $(LIB_TARGET_MODBUS) $(TARGET_OBJ_Files_MODBUS)
$(OPERATION_DEPEND_MODBUS):
	$(foreach dir, $(TARGET_OBJ_DIR_MODBUS), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_MODBUS): $(SRC_OBJ_Files_MODBUS)
$(SRC_OBJ_Files_MODBUS): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_MODBUS) $< -o $(OBJ_DIR)$@
#########################MODBUS end###########################



allSo := $(LIB_TARGET_test) $(LIB_TARGET_APDU) $(LIB_TARGET_MODBUS)

all: $(allSo)


apdu:$(LIB_TARGET_APDU)
modbus:$(LIB_TARGET_MODBUS)


#######################################################

.PHONY: clean cls
clean:
	@echo ""
	 -rm -rf $(OBJ_DIR)
	-rm -f $(allSo)
	@echo ""
cls:
	@echo "" 
	-rm -rf $(OBJ_DIR)
	@echo ""