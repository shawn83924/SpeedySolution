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
	${OBJECTDIR}/JSExecConnectionMain.o \
	${OBJECTDIR}/JihSunEexcutionConnection.o \
	${OBJECTDIR}/MBusAdapter.o \
	${OBJECTDIR}/TandemAdapter.o \
	${OBJECTDIR}/TandemObjects.o


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
LDLIBSOPTIONS=/MBus/lib/libSpeedyAPI_64.a ../../UFC/../Release/libUFC_64.a /MBus/lib/libUFCPlus_64.a ../../TAIFEXLib/../../lib/TAIFEXLib_64.a -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/JSExecConnection

/Speedy/bin/JSExecConnection: /MBus/lib/libSpeedyAPI_64.a

/Speedy/bin/JSExecConnection: ../../UFC/../Release/libUFC_64.a

/Speedy/bin/JSExecConnection: /MBus/lib/libUFCPlus_64.a

/Speedy/bin/JSExecConnection: ../../TAIFEXLib/../../lib/TAIFEXLib_64.a

/Speedy/bin/JSExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/JSExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/JSExecConnectionMain.o: JSExecConnectionMain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../UFCPlus -I../../Migo -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/JSExecConnectionMain.o JSExecConnectionMain.cpp

${OBJECTDIR}/JihSunEexcutionConnection.o: JihSunEexcutionConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../UFCPlus -I../../Migo -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/JihSunEexcutionConnection.o JihSunEexcutionConnection.cpp

${OBJECTDIR}/MBusAdapter.o: MBusAdapter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../UFCPlus -I../../Migo -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MBusAdapter.o MBusAdapter.cpp

${OBJECTDIR}/TandemAdapter.o: TandemAdapter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../UFCPlus -I../../Migo -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TandemAdapter.o TandemAdapter.cpp

${OBJECTDIR}/TandemObjects.o: TandemObjects.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D_REENTRANT -D__LINUX -I../../UFC -I../../UFCPlus -I../../Migo -I../Lib -I../../TAIFEXLib -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TandemObjects.o TandemObjects.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/JSExecConnection

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
