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
CND_CONF=LINUX_Release_64
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
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64 -pthread -Wno-deprecated
CXXFLAGS=-m64 -pthread -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=-s -m64

# Assembler Flags
ASFLAGS=--64

# Link Libraries and Options
LDLIBSOPTIONS=-L. -L../Lib /MBus/lib/libSpeedyAPI_64.a /MBus/lib/libUFC_64.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SpeedyWorkingCancel

/Speedy/bin/SpeedyWorkingCancel: /MBus/lib/libSpeedyAPI_64.a

/Speedy/bin/SpeedyWorkingCancel: /MBus/lib/libUFC_64.a

/Speedy/bin/SpeedyWorkingCancel: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyWorkingCancel ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread

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
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release_64

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/SpeedyWorkingCancel

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release_64 clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
