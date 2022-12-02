##################################################################
#
# FILENAME   :	Makefile
# DESCRIPT	 :	A general makefile to generate an ELF or a
# 					dynamic or a static library for C/C++ project.
# AUTHOR	 : 	vfhky  2015.08.07
# URI 		 :	https://typecodes.com/cseries/cppgeneralmakefile.html
# 
# modify     :whofusky 2020.09.26
#
##################################################################



.PHONY: all clean help
all: 



mylib :=
myso  :=
myexe :=



## Some important on-off settings. You can not be too careful about them.
DEBUG       	:= y



## Flag of generate a dynamic lib or a static lib: y means yes. If the target is a excutable file, it should be blank!
## Name of the static lib. It should be blank unless the target is a static lib, then the GEN_LIBS is y and GEN_DYN_LIB is blank.
STATIC_LIBS	:= $(if $(strip $(mylib)),$(strip $(mylib)),)
GEN_LIBS   	:= $(if $(strip $(STATIC_LIBS)),y,) 




## Flag of generate a dynamic lib: y means yes. It should be blank unless you want to generate a dynamic lib!
## Name of the dynamic lib. It should be blank unless the target is a dynamic lib, then the GEN_LIBS is y and GEN_DYN_LIB is y.
#DYNAMIC_LIBS 	:= libmyfun.so
#GEN_DYN_LIB    := y
DYNAMIC_LIBS := $(if $(strip $(myso)),$(strip $(myso)),)
GEN_DYN_LIB  := $(if $(strip $(DYNAMIC_LIBS)),y,)




## The name of target bin file.Please let it be blank unless the target is a excutable file. 
EXCUTE_BIN		:= $(if $(myexe),$(myexe),)



# Environment settings. The value of PROJECT_DIR shoule be set in the *nix system as the the absolute dir path of your project.
# PROJECT_DIR	:= /home/test_usr
#CURDIR    		:= $(PROJECT_DIR)/src/pbl
#CURDIR     		:= $(shell pwd)
CURDIR     		:= .
PROJECT_DIR     := $(CURDIR)
TMPFILE_DIR     := $(PROJECT_DIR)/tmpFile
PRG_BIN_DIR		:= $(PROJECT_DIR)/bin
PRG_LIB_DIR		:= $(PROJECT_DIR)/out_lib
PRG_INC_DIR		:= $(PROJECT_DIR)/include
DEPEND_LIB_DIR  := $(PROJECT_DIR)/lib




# Cross compile tools defined. You needn't modify these vars below generally.
AS    	:= as
LD    	:= ld
CC    	:= gcc
CXX   	:= g++
CPP   	:= $(CC) -E
AR      := ar rcs
NM      := nm
STRIP   := strip
RANLIB 	:= ranlib
STD_OPT	:= -D_GNU_SOURCE
CC    	+= -std=c99 $(STD_OPT)
CXX    	+= $(STD_OPT)




# *nix system tools defined. You needn't modify these vars below generally.
BLACK = "\e[33;0m"
RED  =  "\e[31;1m"
GREEN = "\e[32;1m"
YELLOW = "\e[33;3m"
BLUE  = "\e[34;1m"
PURPLE = "\e[35;1m"
CYAN  = "\e[36;1m"
WHITE = "\e[37;1m"
CP		:= cp
SED		:= sed
FIND	:= find
MKDIR 	:= mkdir -p
XARGS	:= xargs
MV    	:= mv
RM    	:= rm -rf




# Get .c, .cpp source files by searching from current directory.
#CUR_SRC 	:= $(shell find . -name "*.c" -o -name "*.cpp"|sed -e 's,./,,')
CUR_SRC_DIR	= $(shell ls -AxR $(CURDIR)|grep ":"|tr -d ':')
CUR_SRC 	:= $(foreach subdir,$(CUR_SRC_DIR),$(wildcard $(subdir)/*.c $(subdir)/*.cpp))
CUR_C   	:= $(filter %.c, $(CUR_SRC))
CUR_CPP 	:= $(filter %.cpp, $(CUR_SRC))




# Get the include files, object files, dependent files by searching from PRG_INC_DIR.
#CUR_OBJ	:= $(addprefix $(PRG_BIN_DIR)/,$(strip $(CUR_CPP:.cpp=.o) $(CUR_C:.c=.o)))
#CUR_OBJ	:= $(addprefix $(PRG_BIN_DIR)/,$(notdir $(CUR_CPP:.cpp=.o) $(CUR_C:.c=.o)))
#CUR_DEP	:= $(addprefix $(PRG_BIN_DIR)/,$(notdir $(CUR_CPP:.cpp=.d) $(CUR_C:.c=.d)))
#CUR_FUNOBJ    := $(shell echo $(CUR_OBJ)|sed 's,$(CURDIR)/[^/]*.o\s*,,g')
#CUR_INC_DIR = $(shell ls -AxR $(PRG_INC_DIR)|grep -v "$(PRG_INC_DIR)/json:"|grep ":"|tr -d ':')
NO_RECURSIVE_INC := noRecursive
CUR_INC_DIR = $(shell ls -AxR $(PRG_INC_DIR)|grep -v "$(PRG_INC_DIR)/$(NO_RECURSIVE_INC)"|grep ":"|tr -d ':') $(PRG_INC_DIR)/$(NO_RECURSIVE_INC)
CUR_INC 	:= $(foreach subdir,$(CUR_INC_DIR),$(subdir)/*.h)
SRC_H   	:= $(filter %.h, $(CUR_INC))
CUR_OBJ		:= $(strip $(CUR_CPP:.cpp=.o) $(CUR_C:.c=.o))
CUR_DEP		:= $(strip $(CUR_CPP:.cpp=.d) $(CUR_C:.c=.d))
SEDCURDIR   := $(shell echo $(CURDIR)|sed 's,\.,\\\.,g')
CUR_FUNOBJ  := $(shell echo $(CUR_OBJ)|sed 's,$(SEDCURDIR)/[^/]*.o\s*,,g')




# Create directory in the header files, bin and library directory.
$(foreach dirname,$(sort $(PRG_INC_DIR) $(PRG_BIN_DIR) $(PRG_LIB_DIR) $(TMPFILE_DIR) $(DEPEND_LIB_DIR)),\
  $(shell $(MKDIR) $(dirname)))



# Complie and link variables. LD_LIBS means the dynamic or static library needed for the object file.
CFLAGS     	:= $(if $(DEBUG),-g -Wall, -O2 -Wall)
CFLAGS     	+= $(if $(strip $(GEN_DYN_LIB)), $(addprefix -fPIC -I ,$(sort $(dir $(SRC_H)))), $(addprefix -I ,$(sort $(dir $(SRC_H)))))
CXXFLAGS   	= $(CFLAGS)
LDFLAGS    	:= 
LD_LIB_DIR 	:= -L $(DEPEND_LIB_DIR)
LD_LIB_TMP1 := $(foreach subdir,$(DEPEND_LIB_DIR),$(wildcard $(subdir)/*.a $(subdir)/*.so))
LD_LIB_TMP2 := $(notdir $(LD_LIB_TMP1))
LD_SYS_LIBS := -lpthread
#LD_SYS_LIBS := 
LD_LIBS	   	:= $(shell echo $(LD_LIB_TMP2)|sed 's,\(^\s*\|\s\+\)\(lib\)\(\S\+\)\(\.a\|\.so\),\1-l\3,g') $(LD_SYS_LIBS)
XLD_FLG	   	:= -Xlinker "-(" $(LDFLAGS) -Xlinker "-)"



# Add vpath.
vpath %.h $(sort $(dir $(SRC_H)))
vpath %.c $(sort $(dir $(CUR_C)))
vpath %.cpp $(sort $(dir $(CUR_CPP)))



# Generate depend files.
ifneq "$(MAKECMDGOALS)" "clean"
sinclude $(CUR_DEP)
endif



# Gen_depend(depend-file,source-file,object-file,cc). This command-package is used to generate a depend file with a postfix of .d.
define gen_depend
  @set -e;                                      \
  $(RM) $1;                                     \
  $4 $(CFLAGS) -MM $2 |                         \
  $(SED) 's,\($(notdir $3)\): ,$3: ,' > $1.tmp; \
  $(SED) -e 's/#.*//'                           \
         -e 's/^[^:]*: *//'                     \
         -e 's/ *\\$$//'                        \
         -e '/^$$/ d'                           \
         -e 's/$$/ :/' < $1.tmp >> $1.tmp;      \
  $(MV) $1.tmp $1;
endef



# Rules to generate objects file(.o) from .c or .cpp files.
$(TMPFILE_DIR)/%.o: $(CURDIR)/%.c
	@$(call gen_depend,$(patsubst %.o,%.d,$@),$<,$@,$(CC))
	$(CC) $(CFLAGS) -o $@ -c $<



$(TMPFILE_DIR)/%.o: $(CURDIR)/%.cpp
	@$(call gen_depend,$(patsubst %.o,%.d,$@),$<,$@,$(CXX))
	$(CXX) $(CXXFLAGS) -o $@ -c $<



# Gen_excbin(target,CUR_OBJ,cc). This command-package is used to generate a excutable file.
define gen_excbin
  ULT_BIN += $(PRG_BIN_DIR)/$1
  $(PRG_BIN_DIR)/$1: $2
	$3 $(LDFLAGS) $$^ $(LD_LIB_DIR) $(LD_LIBS) $(XLD_FLG) -o $$@
	@echo -e $(YELLOW)"========================Success========================"$(BLACK)
endef



# Gen_libs(libs,CUR_OBJ,cc). This command-package is used to generate a dynamic lib or a static lib.
define gen_libs
  ULT_LIBS += $(PRG_LIB_DIR)/$1
  $(PRG_LIB_DIR)/$1: $2
	$3 $(if $(strip $(GEN_DYN_LIB)),-shared $$^ $(CXXFLAGS) $(LD_LIB_DIR) $(LD_LIBS) $(XLD_FLG) -o $$@,$$@ $$^)
	@echo -e $(YELLOW)"========================Success========================"$(BLACK)
endef



# Call gen_excbin to generate a excutale file.
$(foreach bin,$(EXCUTE_BIN),$(eval $(call gen_excbin,$(bin),$(CUR_OBJ),$(CXX))))



# Call gen_libs to generate a dynamic libso.
$(foreach libso,$(DYNAMIC_LIBS),$(eval $(call gen_libs,$(libso),$(CUR_FUNOBJ),$(CXX))))



# Call gen_libs to generate a static lib.
$(foreach lib,$(STATIC_LIBS),$(eval $(call gen_libs,$(lib),$(CUR_FUNOBJ),$(AR))))



all: $(ULT_BIN) $(ULT_LIBS)



cleanobj: 
	-$(FIND) $(CURDIR) -name "*.o" -o -name "*.d" | $(XARGS) $(RM)


clean: 
	-$(FIND) $(CURDIR) -name "*.o" -o -name "*.d" | $(XARGS) $(RM)
	-$(RM) $(ULT_BIN) $(ULT_LIBS)




tprint:
	@echo CUR_SRC_DIR=[$(CUR_SRC_DIR)]
	@echo CUR_OBJ=[$(CUR_OBJ)]
	@echo CUR_FUNOBJ=[$(CUR_FUNOBJ)]
	@echo LD_LIB_TMP2=[$(LD_LIB_TMP2)]
	@echo LD_LIBS=[$(LD_LIBS)]



help:
	@echo CURDIR=[$(CURDIR)]
	@echo PROJECT_DIR=[$(PROJECT_DIR)]
	@echo TMPFILE_DIR=[$(TMPFILE_DIR)]
	@echo PRG_BIN_DIR=[$(PRG_BIN_DIR)]
	@echo PRG_LIB_DIR=[$(PRG_LIB_DIR)]
	@echo PRG_INC_DIR=[$(PRG_INC_DIR)]
	@echo DEPEND_LIB_DIR=[$(DEPEND_LIB_DIR)]
	@echo CC=[$(CC)]
	@echo CXX=[$(CXX)]
	@echo CFLAGS=[$(CFLAGS)]
	@echo CXXFLAGS=[$(CXXFLAGS)]
	@echo LDFLAGS=[$(LDFLAGS)]
	@echo LD_LIB_DIR=[$(LD_LIB_DIR)]
	@echo LD_LIB_TMP1=[$(LD_LIB_TMP1)]
	@echo LD_LIB_TMP2=[$(LD_LIB_TMP2)]
	@echo LD_LIBS=[$(LD_LIBS)]
	@echo XLD_FLG=[$(XLD_FLG)]
	@echo CUR_SRC_DIR=[$(CUR_SRC_DIR)]
	@echo CUR_SRC=[$(CUR_SRC)]
	@echo CUR_C=[$(CUR_C)]
	@echo CUR_CPP=[$(CUR_CPP)]
	@echo CUR_INC_DIR=[$(CUR_INC_DIR)]
	@echo CUR_INC=[$(CUR_INC)]
	@echo SRC_H=[$(SRC_H)]
	@echo CUR_OBJ=[$(CUR_OBJ)]
	@echo CUR_DEP=[$(CUR_DEP)]
	@echo SEDCURDIR=[$(SEDCURDIR)]
	@echo CUR_FUNOBJ=[$(CUR_FUNOBJ)]
	@echo STATIC_LIBS=[$(STATIC_LIBS)]
	@echo GEN_LIBS=[$(GEN_LIBS)]
	@echo DYNAMIC_LIBS=[$(DYNAMIC_LIBS)]
	@echo GEN_DYN_LIB=[$(GEN_DYN_LIB)]
	@echo ULT_LIBS=[$(ULT_LIBS)]


