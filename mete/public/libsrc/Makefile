#!/bin/bash

WORK_DIR=$(PWD)

#LIB_DIR := ../lib/
LIB_DIR := lib/
STATIC_LIB_SUFFIX := .a
###################################################
GFLAG = -g -Wall
##编译参数
################################################### 
XCC=g++ $(GFLAG)
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


#########################make mete begine###########################
CUSTOMS_HEAD_mete := -I include/mete -I include/baseFunc
LIB_NAME_mete := mete
LIB_TARGET_mete := $(LIB_DIR)lib$(LIB_NAME_mete)$(STATIC_LIB_SUFFIX)

SRC_DIR_mete := src/mete

SRC_CPP_Files_mete := 
SRC_CPP_Files_mete += $(foreach dir, $(SRC_DIR_mete), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_mete := 
SRC_OBJ_Files_mete += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_mete))
TARGET_OBJ_Files_mete :=
TARGET_OBJ_Files_mete += $(foreach file, $(SRC_OBJ_Files_mete), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_mete :=
TARGET_OBJ_DIR_mete += $(foreach file, $(SRC_DIR_mete), $(OBJ_DIR)$(file))

mete:$(LIB_TARGET_mete)

OPERATION_DEPEND_mete := mete_dir_depend
$(LIB_TARGET_mete): $(OPERATION_DEPEND_mete) $(TARGET_OBJ_Files_mete)
	$(AR) -rv $(LIB_TARGET_mete) $(TARGET_OBJ_Files_mete)
$(OPERATION_DEPEND_mete):
	$(foreach dir, $(TARGET_OBJ_DIR_mete), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_mete): $(SRC_OBJ_Files_mete)
$(SRC_OBJ_Files_mete): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_mete) $< -o $(OBJ_DIR)$@
#########################mete end###########################


#########################make baseFunc begine###########################
CUSTOMS_HEAD_baseFunc := -I include/baseFunc
LIB_NAME_baseFunc := baseFunc
LIB_TARGET_baseFunc := $(LIB_DIR)lib$(LIB_NAME_baseFunc)$(STATIC_LIB_SUFFIX)

SRC_DIR_baseFunc := src/baseFunc

SRC_CPP_Files_baseFunc := 
SRC_CPP_Files_baseFunc += $(foreach dir, $(SRC_DIR_baseFunc), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_baseFunc := 
SRC_OBJ_Files_baseFunc += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_baseFunc))
TARGET_OBJ_Files_baseFunc :=
TARGET_OBJ_Files_baseFunc += $(foreach file, $(SRC_OBJ_Files_baseFunc), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_baseFunc :=
TARGET_OBJ_DIR_baseFunc += $(foreach file, $(SRC_DIR_baseFunc), $(OBJ_DIR)$(file))

baseFunc:$(LIB_TARGET_baseFunc)

OPERATION_DEPEND_baseFunc := baseFunc_dir_depend
$(LIB_TARGET_baseFunc): $(OPERATION_DEPEND_baseFunc) $(TARGET_OBJ_Files_baseFunc)
	$(AR) -rv $(LIB_TARGET_baseFunc) $(TARGET_OBJ_Files_baseFunc)
$(OPERATION_DEPEND_baseFunc):
	$(foreach dir, $(TARGET_OBJ_DIR_baseFunc), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_baseFunc): $(SRC_OBJ_Files_baseFunc)
$(SRC_OBJ_Files_baseFunc): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_baseFunc) $< -o $(OBJ_DIR)$@
#########################baseFunc end###########################


#########################make hlthRprtWrtXml begine###########################
CUSTOMS_HEAD_hlth := -I include/hlthRprtWrtXml
LIB_NAME_hlth := hlthRprtWrtXml
LIB_TARGET_hlth := $(LIB_DIR)lib$(LIB_NAME_hlth)$(STATIC_LIB_SUFFIX)

SRC_DIR_hlth := src/hlthRprtWrtXml

SRC_CPP_Files_hlth := 
SRC_CPP_Files_hlth += $(foreach dir, $(SRC_DIR_hlth), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_hlth := 
SRC_OBJ_Files_hlth += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_hlth))
TARGET_OBJ_Files_hlth :=
TARGET_OBJ_Files_hlth += $(foreach file, $(SRC_OBJ_Files_hlth), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_hlth :=
TARGET_OBJ_DIR_hlth += $(foreach file, $(SRC_DIR_hlth), $(OBJ_DIR)$(file))

hlthRprtWrtXml:$(LIB_TARGET_hlth)

OPERATION_DEPEND_hlth := hlth_dir_depend
$(LIB_TARGET_hlth): $(OPERATION_DEPEND_hlth) $(TARGET_OBJ_Files_hlth)
	$(AR) -rv $(LIB_TARGET_hlth) $(TARGET_OBJ_Files_hlth)
$(OPERATION_DEPEND_hlth):
	$(foreach dir, $(TARGET_OBJ_DIR_hlth), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_hlth): $(SRC_OBJ_Files_hlth)
$(SRC_OBJ_Files_hlth): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_hlth) $< -o $(OBJ_DIR)$@
#########################hlthRprtWrtXml end###########################

#########################make 102slave begine###########################
CUSTOMS_HEAD_102slave := -I include/102slave
LIB_NAME_102slave := 102slave
LIB_TARGET_102slave := $(LIB_DIR)lib$(LIB_NAME_102slave)$(STATIC_LIB_SUFFIX)

SRC_DIR_102slave := src/102slave

SRC_CPP_Files_102slave := 
SRC_CPP_Files_102slave += $(foreach dir, $(SRC_DIR_102slave), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_102slave := 
SRC_OBJ_Files_102slave += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_102slave))
TARGET_OBJ_Files_102slave :=
TARGET_OBJ_Files_102slave += $(foreach file, $(SRC_OBJ_Files_102slave), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_102slave :=
TARGET_OBJ_DIR_102slave += $(foreach file, $(SRC_DIR_102slave), $(OBJ_DIR)$(file))

102slave:$(LIB_TARGET_102slave)

OPERATION_DEPEND_102slave := 102slave_dir_depend
$(LIB_TARGET_102slave): $(OPERATION_DEPEND_102slave) $(TARGET_OBJ_Files_102slave)
	$(AR) -rv $(LIB_TARGET_102slave) $(TARGET_OBJ_Files_102slave)
$(OPERATION_DEPEND_102slave):
	$(foreach dir, $(TARGET_OBJ_DIR_102slave), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_102slave): $(SRC_OBJ_Files_102slave)
$(SRC_OBJ_Files_102slave): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_102slave) $< -o $(OBJ_DIR)$@
#########################102slave end###########################

#########################make xmlDataRep begine###########################
CUSTOMS_HEAD_xmlDataRep := -I include/xmlUniveralParse/xmlDataRep
LIB_NAME_xmlDataRep := xmlDataRep
LIB_TARGET_xmlDataRep := $(LIB_DIR)lib$(LIB_NAME_xmlDataRep)$(STATIC_LIB_SUFFIX)

SRC_DIR_xmlDataRep := src/xmlUniveralParse/xmlDataRep

SRC_CPP_Files_xmlDataRep := 
SRC_CPP_Files_xmlDataRep += $(foreach dir, $(SRC_DIR_xmlDataRep), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_xmlDataRep := 
SRC_OBJ_Files_xmlDataRep += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_xmlDataRep))
TARGET_OBJ_Files_xmlDataRep :=
TARGET_OBJ_Files_xmlDataRep += $(foreach file, $(SRC_OBJ_Files_xmlDataRep), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_xmlDataRep :=
TARGET_OBJ_DIR_xmlDataRep += $(foreach file, $(SRC_DIR_xmlDataRep), $(OBJ_DIR)$(file))

xmlDataRep:$(LIB_TARGET_xmlDataRep)

OPERATION_DEPEND_xmlDataRep := xmlDataRep_dir_depend
$(LIB_TARGET_xmlDataRep): $(OPERATION_DEPEND_xmlDataRep) $(TARGET_OBJ_Files_xmlDataRep)
	$(AR) -rv $(LIB_TARGET_xmlDataRep) $(TARGET_OBJ_Files_xmlDataRep)
$(OPERATION_DEPEND_xmlDataRep):
	$(foreach dir, $(TARGET_OBJ_DIR_xmlDataRep), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_xmlDataRep): $(SRC_OBJ_Files_xmlDataRep)
$(SRC_OBJ_Files_xmlDataRep): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_xmlDataRep) $< -o $(OBJ_DIR)$@
#########################xmlDataRep end###########################



#########################make xmlParse begine###########################
CUSTOMS_HEAD_xmlParse := -I include/xmlUniveralParse/xmlParse \
	 -I include/xmlUniveralParse/xmlDataRep
LIB_NAME_xmlParse := xmlParse
LIB_TARGET_xmlParse := $(LIB_DIR)lib$(LIB_NAME_xmlParse)$(STATIC_LIB_SUFFIX)

SRC_DIR_xmlParse := src/xmlUniveralParse/xmlParse

SRC_CPP_Files_xmlParse := 
SRC_CPP_Files_xmlParse += $(foreach dir, $(SRC_DIR_xmlParse), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_xmlParse := 
SRC_OBJ_Files_xmlParse += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_xmlParse))
TARGET_OBJ_Files_xmlParse :=
TARGET_OBJ_Files_xmlParse += $(foreach file, $(SRC_OBJ_Files_xmlParse), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_xmlParse :=
TARGET_OBJ_DIR_xmlParse += $(foreach file, $(SRC_DIR_xmlParse), $(OBJ_DIR)$(file))

xmlParse:$(LIB_TARGET_xmlParse)

OPERATION_DEPEND_xmlParse := xmlParse_dir_depend
$(LIB_TARGET_xmlParse): $(OPERATION_DEPEND_xmlParse) $(TARGET_OBJ_Files_xmlParse)
	$(AR) -rv $(LIB_TARGET_xmlParse) $(TARGET_OBJ_Files_xmlParse)
$(OPERATION_DEPEND_xmlParse):
	$(foreach dir, $(TARGET_OBJ_DIR_xmlParse), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_xmlParse): $(SRC_OBJ_Files_xmlParse)
$(SRC_OBJ_Files_xmlParse): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_xmlParse) $< -o $(OBJ_DIR)$@
#########################xmlParse end###########################


#########################make xmlAppItfc begine###########################
CUSTOMS_HEAD_xmlAppItfc := -I include/xmlUniveralParse/xmlAppItfc \
	-I include/xmlUniveralParse/xmlParse -I include/xmlUniveralParse/xmlDataRep
LIB_NAME_xmlAppItfc := xmlAppItfc
LIB_TARGET_xmlAppItfc := $(LIB_DIR)lib$(LIB_NAME_xmlAppItfc)$(STATIC_LIB_SUFFIX)

SRC_DIR_xmlAppItfc := src/xmlUniveralParse/xmlAppItfc

SRC_CPP_Files_xmlAppItfc := 
SRC_CPP_Files_xmlAppItfc += $(foreach dir, $(SRC_DIR_xmlAppItfc), $(wildcard $(dir)/*.cpp))
SRC_OBJ_Files_xmlAppItfc := 
SRC_OBJ_Files_xmlAppItfc += $(patsubst %.cpp, %.o, $(SRC_CPP_Files_xmlAppItfc))
TARGET_OBJ_Files_xmlAppItfc :=
TARGET_OBJ_Files_xmlAppItfc += $(foreach file, $(SRC_OBJ_Files_xmlAppItfc), $(OBJ_DIR)$(file))
TARGET_OBJ_DIR_xmlAppItfc :=
TARGET_OBJ_DIR_xmlAppItfc += $(foreach file, $(SRC_DIR_xmlAppItfc), $(OBJ_DIR)$(file))

xmlAppItfc:$(LIB_TARGET_xmlAppItfc)

OPERATION_DEPEND_xmlAppItfc := xmlAppItfc_dir_depend
$(LIB_TARGET_xmlAppItfc): $(OPERATION_DEPEND_xmlAppItfc) $(TARGET_OBJ_Files_xmlAppItfc)
	$(AR) -rv $(LIB_TARGET_xmlAppItfc) $(TARGET_OBJ_Files_xmlAppItfc)
$(OPERATION_DEPEND_xmlAppItfc):
	$(foreach dir, $(TARGET_OBJ_DIR_xmlAppItfc), $(shell mkdir -p $(dir)))
$(TARGET_OBJ_Files_xmlAppItfc): $(SRC_OBJ_Files_xmlAppItfc)
$(SRC_OBJ_Files_xmlAppItfc): %.o : %.cpp
	$(XCC) -c  $(INCLUDES) $(CUSTOMS_HEAD_xmlAppItfc) $< -o $(OBJ_DIR)$@
#########################xmlAppItfc end###########################

allLib := $(LIB_TARGET_mete) $(LIB_TARGET_baseFunc) $(LIB_TARGET_hlth)  \
	$(LIB_TARGET_xmlDataRep) $(LIB_TARGET_xmlParse) $(LIB_TARGET_xmlAppItfc)

all:$(allLib)

#######################################################

.PHONY: clean cls
clean:
	@echo ""
	-rm -rf $(OBJ_DIR)
	-rm -f $(allLib)
	@echo ""
cls:
	@echo ""
	-rm -rf $(OBJ_DIR)
	@echo ""
