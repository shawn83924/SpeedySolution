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
	${OBJECTDIR}/GCRMConnection.o \
	${OBJECTDIR}/Main.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -Wno-deprecated
CXXFLAGS=-m32 -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/tibco/tibrv/lib /MBus/lib/libSpeedyAPI.a /MBus/lib/libUFC.a -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/GCRiskManager

/Speedy/bin/GCRiskManager: /MBus/lib/libSpeedyAPI.a

/Speedy/bin/GCRiskManager: /MBus/lib/libUFC.a

/Speedy/bin/GCRiskManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/GCRiskManager ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/GCRMConnection.o: GCRMConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -D_REENTRANT -I../OrderMessage -I../OrderConnection -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GCRMConnection.o GCRMConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -D_REENTRANT -I../OrderMessage -I../OrderConnection -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/GCRiskManager

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
