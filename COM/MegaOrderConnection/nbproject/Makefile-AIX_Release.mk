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
CND_PLATFORM=GNU-Generic
CND_DLIB_EXT=so
CND_CONF=AIX_Release
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
	${OBJECTDIR}/SpeedyQueueOrderConnection.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../UFC/../bin/libUFC.a /MBus/lib/libSpeedyAPI.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/MegaOrderConnection

/Speedy/bin/MegaOrderConnection: ../../UFC/../bin/libUFC.a

/Speedy/bin/MegaOrderConnection: /MBus/lib/libSpeedyAPI.a

/Speedy/bin/MegaOrderConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/MegaOrderConnection ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread

${OBJECTDIR}/Main.o: Main.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../ExchangeUtility -I../../UFC -I../../Migo -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cc

${OBJECTDIR}/MessageDispatcher.o: MessageDispatcher.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../ExchangeUtility -I../../UFC -I../../Migo -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MessageDispatcher.o MessageDispatcher.cc

${OBJECTDIR}/SpeedyQueueOrderConnection.o: SpeedyQueueOrderConnection.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../ExchangeUtility -I../../UFC -I../../Migo -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyQueueOrderConnection.o SpeedyQueueOrderConnection.cc

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
