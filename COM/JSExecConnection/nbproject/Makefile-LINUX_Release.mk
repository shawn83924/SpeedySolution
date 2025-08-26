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
	${OBJECTDIR}/JSExecConnectionMain.o \
	${OBJECTDIR}/JihSunEexcutionConnection.o \
	${OBJECTDIR}/MBusAdapter.o \
	${OBJECTDIR}/TandemAdapter.o \
	${OBJECTDIR}/TandemObjects.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -fPIC -Wno-deprecated
CXXFLAGS=-m32 -fPIC -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../TAIFEXLib/../../lib/TAIFEXLib.a /MBus/lib/libSpeedyAPI.a /MBus/lib/libUFCPlus.a /MBus/lib/libUFC.a -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/JSExecConnection

/Speedy/bin/JSExecConnection: ../../TAIFEXLib/../../lib/TAIFEXLib.a

/Speedy/bin/JSExecConnection: /MBus/lib/libSpeedyAPI.a

/Speedy/bin/JSExecConnection: /MBus/lib/libUFCPlus.a

/Speedy/bin/JSExecConnection: /MBus/lib/libUFC.a

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
	cd ../../TAIFEXLib && ${MAKE}  -f Makefile CONF=LINUX_Release
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release
	cd ../../UFCPlus && ${MAKE}  -f Makefile CONF=LINUX_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/JSExecConnection

# Subprojects
.clean-subprojects:
	cd ../../TAIFEXLib && ${MAKE}  -f Makefile CONF=LINUX_Release clean
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release clean
	cd ../../UFCPlus && ${MAKE}  -f Makefile CONF=LINUX_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
