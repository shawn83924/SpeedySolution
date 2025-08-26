#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add custumized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=g77

# Include project Makefile
include Makefile

# Object Files
OBJECTFILES= \
	build/Debug/GNU-Solaris-Sparc/Publisher.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=\
	-lsocket \
	-lnsl \
	-lpthread \
	/MBus/lib/Migo.a \
	/MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /MBus/Tools/Publisher

/MBus/Tools/Publisher: ${BUILD_SUBPROJECTS}

/MBus/Tools/Publisher: ${OBJECTFILES}
	@${MKDIR} -p /MBus/Tools
	${LINK.cc} -o /MBus/Tools/Publisher ${OBJECTFILES} ${LDLIBSOPTIONS} 

build/Debug/GNU-Solaris-Sparc/Publisher.o: Publisher.cpp 
	@${MKDIR} -p build/Debug/GNU-Solaris-Sparc
	$(COMPILE.cc) -g -I../../UFC -I../../Migo -o build/Debug/GNU-Solaris-Sparc/Publisher.o Publisher.cpp

# Subprojects
.build-subprojects:
	cd ../../Migo; ${MAKE} -f Makefile_Migo.mk CONF=Debug
	cd ../../UFC; ${MAKE} -f Makefile_UFC.mk CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} /MBus/Tools/Publisher

# Subprojects
.clean-subprojects:
	cd ../../Migo; ${MAKE} -f Makefile_Migo.mk CONF=Debug clean
	cd ../../UFC; ${MAKE} -f Makefile_UFC.mk CONF=Debug clean
