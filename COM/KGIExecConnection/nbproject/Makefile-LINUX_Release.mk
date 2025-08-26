#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=LINUX_Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/KGIExecConnection.o \
	${OBJECTDIR}/Main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -Wno-deprecated
CXXFLAGS=-m32 -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/libSpeedyAPI.a /MBus/lib/libUFC.a -lrt -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/KGIExecConnection

/Speedy/bin/KGIExecConnection: /MBus/lib/libSpeedyAPI.a

/Speedy/bin/KGIExecConnection: /MBus/lib/libUFC.a

/Speedy/bin/KGIExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/KGIExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/KGIExecConnection.o: KGIExecConnection.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_REENTRANT -D__LINUX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/KGIExecConnection.o KGIExecConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_REENTRANT -D__LINUX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
