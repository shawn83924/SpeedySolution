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
CND_CONF=LINUX_Release_64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/CapitalExecutionConnection.o \
	${OBJECTDIR}/MBusAdapter.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/NoticeServer.o \
	${OBJECTDIR}/TandemAdapter.o \
	${OBJECTDIR}/TandemObject.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 -Wno-deprecated
CXXFLAGS=-m64 -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../TAIFEXLib/../../lib/TAIFEXLib_64.a /MBus/lib/libSpeedyAPI_64.a /MBus/lib/libUFCPlus_64.a ../../UFC/../Release/libUFC_64.a -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/CapitalExecConnectionTcp

/Speedy/bin/CapitalExecConnectionTcp: ../../TAIFEXLib/../../lib/TAIFEXLib_64.a

/Speedy/bin/CapitalExecConnectionTcp: /MBus/lib/libSpeedyAPI_64.a

/Speedy/bin/CapitalExecConnectionTcp: /MBus/lib/libUFCPlus_64.a

/Speedy/bin/CapitalExecConnectionTcp: ../../UFC/../Release/libUFC_64.a

/Speedy/bin/CapitalExecConnectionTcp: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/CapitalExecConnectionTcp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/CapitalExecutionConnection.o: CapitalExecutionConnection.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../Migo -I../../UFCPlus -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CapitalExecutionConnection.o CapitalExecutionConnection.cpp

${OBJECTDIR}/MBusAdapter.o: MBusAdapter.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../Migo -I../../UFCPlus -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MBusAdapter.o MBusAdapter.cpp

${OBJECTDIR}/Main.o: Main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../Migo -I../../UFCPlus -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/NoticeServer.o: NoticeServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../Migo -I../../UFCPlus -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NoticeServer.o NoticeServer.cpp

${OBJECTDIR}/TandemAdapter.o: TandemAdapter.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../Migo -I../../UFCPlus -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TandemAdapter.o TandemAdapter.cpp

${OBJECTDIR}/TandemObject.o: TandemObject.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../Migo -I../../UFCPlus -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TandemObject.o TandemObject.cpp

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
