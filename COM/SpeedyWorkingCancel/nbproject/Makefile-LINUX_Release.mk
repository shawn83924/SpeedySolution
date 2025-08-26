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
CND_PLATFORM=GNU-Linux-x86
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
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/SpeedyOrderConnection.o \
	${OBJECTDIR}/WorkingOrder.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -pthread -Wno-deprecated
CXXFLAGS=-m32 -pthread -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=-s -m32

# Assembler Flags
ASFLAGS=--32

# Link Libraries and Options
LDLIBSOPTIONS=-L. -L../Lib ../Lib/libSpeedyAPI.a ../Lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/LINUX_Release/GNU-Linux-x86/SpeedyOrderConnection

dist/LINUX_Release/GNU-Linux-x86/SpeedyOrderConnection: ../Lib/libSpeedyAPI.a

dist/LINUX_Release/GNU-Linux-x86/SpeedyOrderConnection: ../Lib/libUFC.a

dist/LINUX_Release/GNU-Linux-x86/SpeedyOrderConnection: ${OBJECTFILES}
	${MKDIR} -p dist/LINUX_Release/GNU-Linux-x86
	${LINK.cc} -o dist/LINUX_Release/GNU-Linux-x86/SpeedyOrderConnection ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/SpeedyOrderConnection.o: SpeedyOrderConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyOrderConnection.o SpeedyOrderConnection.cpp

${OBJECTDIR}/WorkingOrder.o: WorkingOrder.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WorkingOrder.o WorkingOrder.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/LINUX_Release/GNU-Linux-x86/SpeedyOrderConnection

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
