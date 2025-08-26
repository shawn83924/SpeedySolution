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
	${OBJECTDIR}/MessageDispatcher.o \
	${OBJECTDIR}/SpeedyQueueRiskManager.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -pthread
CXXFLAGS=-m32 -pthread

# Fortran Compiler Flags
FFLAGS=-m32

# Assembler Flags
ASFLAGS=--32

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SpeedyQueueRiskManager

/Speedy/bin/SpeedyQueueRiskManager: /MBus/lib/libUFC.a

/Speedy/bin/SpeedyQueueRiskManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SpeedyQueueRiskManager ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -MMD -MP -MF $@.d -o ${OBJECTDIR}/Main.o Main.cc

${OBJECTDIR}/MessageDispatcher.o: MessageDispatcher.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -MMD -MP -MF $@.d -o ${OBJECTDIR}/MessageDispatcher.o MessageDispatcher.cc

${OBJECTDIR}/SpeedyQueueRiskManager.o: SpeedyQueueRiskManager.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -MMD -MP -MF $@.d -o ${OBJECTDIR}/SpeedyQueueRiskManager.o SpeedyQueueRiskManager.cc

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/SpeedyQueueRiskManager

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
