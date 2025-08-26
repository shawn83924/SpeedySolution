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
CND_CONF=LINUX_Release_Capital
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/CaptialExecConnection.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/TUserData.o


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
LDLIBSOPTIONS=-lrt ../../TAIFEXLib/../../lib/TAIFEXLib.a ../Lib/./libSpeedyAPI.a /MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/CapitalExecConnection

/Speedy/bin/CapitalExecConnection: ../../TAIFEXLib/../../lib/TAIFEXLib.a

/Speedy/bin/CapitalExecConnection: ../Lib/./libSpeedyAPI.a

/Speedy/bin/CapitalExecConnection: /MBus/lib/libUFC.a

/Speedy/bin/CapitalExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	g++ -o /Speedy/bin/CapitalExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread

${OBJECTDIR}/CaptialExecConnection.o: CaptialExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__CAPITAL -D__LINUX -I../../UFC -I../../Migo -I../Lib -I../Lib/OrderConnection -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CaptialExecConnection.o CaptialExecConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__CAPITAL -D__LINUX -I../../UFC -I../../Migo -I../Lib -I../Lib/OrderConnection -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/TUserData.o: TUserData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__CAPITAL -D__LINUX -I../../UFC -I../../Migo -I../Lib -I../Lib/OrderConnection -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TUserData.o TUserData.cpp

# Subprojects
.build-subprojects:
	cd ../../TAIFEXLib && ${MAKE}  -f Makefile CONF=LINUX_Release
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/CapitalExecConnection

# Subprojects
.clean-subprojects:
	cd ../../TAIFEXLib && ${MAKE}  -f Makefile CONF=LINUX_Release clean
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
