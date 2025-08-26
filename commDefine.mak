#####################################################################
# Author  : Allen Lee
# Purpose : pre Define options, flags, defines, linking librarys ...
#           for gnu compiler on each platform.
#           you should follow this rule if you are develop base on
#           YTF.
#####################################################################

#--------------------------------------------------------------------------------
# **** important ****
# please use "gcc -print-file-name=specs" or "gcc -dumpspecs" to check specs file
# On Solaris
# when DEFIND  "-pthreads" is equivalent to "-D_REENTRANT -D_PTHREADS"
# when Linking "-pthreads" is equivalent to "-lpthread"
# On AIX
# when DEFIND  "-pthread" is equivalent to "-D_THREAD_SAFE"
# when Linking "-pthread" is equivalent to "-lpthread -lc"
#--------------------------------------------------------------------------------

CC = gcc
CXX = g++

################################ options setting by gxx version
# for gcc 4.2 before 
#VOPT_CXX = -MMD -Wall -Wshadow -Werror

# for gcc 4.2 above
#VOPT_CXX = -MMD -Wall -Wno-write-strings -Wshadow -Werror

# for gcc 4.3 above
VOPT_CXX = -MMD -Wall -std=c++0x -Wno-write-strings -Wshadow -Werror

################################ OS name
#os_name = aix
#os_name = solaris
os_name = linux

################################ compile mode ( debug or release )
#compile_mode = debug
compile_mode = release

################################ default thread mode ( mt or st )
#thread_mode = st
thread_mode = mt

################################ default nbits mode ( 32 or 64 )
#nbits_mode = 32
nbits_mode = 64

################################ library mode ( static or shared )
#library_mode = static
library_mode = shared


#--------------------------------------------------------------------------------
# nbits_mode setting ( 32 or 64 )
#--------------------------------------------------------------------------------
ifeq ($(nbits_mode),32)
    NBITS_DEF := -D_NBITS32 -D__NBITS32
    NBITS_PATH := 32
    ifeq ($(os_name),aix)
        NBITS_C := -maix32
    else
        NBITS_C := -m32
    endif
else
    NBITS_DEF := -D_NBITS64 -D__NBITS64
    NBITS_PATH := 64
    ifeq ($(os_name),aix)
        NBITS_C := -maix64
    else
        NBITS_C := -m64
    endif
endif

#--------------------------------------------------------------------------------
# thread_mode setting ( mt or st )
#--------------------------------------------------------------------------------
ifeq ($(thread_mode),st)
    TMODE_DEF := -D_STMODE -D__STMODE
    TMODE_PATH := st
else
    TMODE_DEF := -D_MTSAFE -D__MTSAFE
    TMODE_PATH := mt
endif

#--------------------------------------------------------------------------------
# os_name setting ( aix solaris linux )
#--------------------------------------------------------------------------------
ifeq ($(os_name),aix)
    ifeq ($(thread_mode),st)
        PLATF_DEF := -D_AIX -D__AIX -D_AIX5L
        PLATF_LDL :=
    else
        PLATF_DEF := -pthread -D_AIX -D__AIX -D_AIX5L
        PLATF_LDL := -pthread
    endif
else
    ifeq ($(os_name),solaris)
        ifeq ($(thread_mode),st)
            PLATF_DEF := -D_SOLARIS -D__SOLARIS
            PLATF_LDL := -lsocket -lnsl -lrt
        else
            PLATF_DEF := -pthreads -D_SOLARIS -D__SOLARIS
            PLATF_LDL := -pthreads -lsocket -lnsl -lrt
        endif
    else
        ifeq ($(thread_mode),st)
            PLATF_DEF := -D_LINUX -D__LINUX
            PLATF_LDL := -lrt
        else
            PLATF_DEF := -pthread -D_LINUX -D__LINUX
            PLATF_LDL := -pthread -lrt
        endif
    endif
endif

DEFINE_VAL := $(PLATF_DEF) $(TMODE_DEF) $(NBITS_DEF)

#--------------------------------------------------------------------------------
# library_mode setting ( static or shared )
#--------------------------------------------------------------------------------
ifeq ($(library_mode),static)
    BUILD_DIR := ../Build/
    LDLIBS := $(PLATF_LDL)
    LIB_LDLIBS := $(PLATF_LDL)
	OPT_C := $(NBITS_C) -MMD -Werror $(DEFINE_VAL)
	OPT_CXX := $(NBITS_C) $(VOPT_CXX) $(DEFINE_VAL)
else
    BUILD_DIR := ../soBuild/
    LDLIBS := $(PLATF_LDL)
    LIB_LDLIBS := -shared -fPIC $(PLATF_LDL)
 	OPT_C := -fPIC $(NBITS_C) -MMD -Werror $(DEFINE_VAL)
	OPT_CXX := -fPIC $(NBITS_C) $(VOPT_CXX) $(DEFINE_VAL)
endif

#--------------------------------------------------------------------------------
# Common setting
#--------------------------------------------------------------------------------
#BUILD_DIR := $(BUILD_DIR)$(TMODE_PATH)$(NBITS_PATH)
#BIN_DIR := ../bin/$(TMODE_PATH)$(NBITS_PATH)
#TBIN_DIR := ../tbin/$(TMODE_PATH)$(NBITS_PATH)

BIN_DIR := ../bin
TBIN_DIR := ../tbin
LIB_DIR := ../lib

#LDLIBS := $(PLATF_LDL)
#OPT_C := $(NBITS_C) -MMD -Werror $(DEFINE_VAL)
#OPT_CXX := $(NBITS_C) $(VOPT_CXX) $(DEFINE_VAL)

#--------------------------------------------------------------------------------
# Default FLAG setting
#--------------------------------------------------------------------------------
ifeq ($(compile_mode),debug)
    CFLAG := -g $(OPT_C) -D_DEBUG -D__DEBUG
    CXXFLAG := -g $(OPT_CXX) -D_DEBUG -D__DEBUG
else
    CFLAG := -O $(OPT_C)
    CXXFLAG := -O $(OPT_CXX)
endif
