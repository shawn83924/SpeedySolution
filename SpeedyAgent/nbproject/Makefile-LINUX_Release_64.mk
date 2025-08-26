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
include Makefile_SpeedyAgent.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/8fedd00c/SpeedyManager.o \
	${OBJECTDIR}/_ext/8fedd00c/UserDB.o \
	${OBJECTDIR}/LogParserThread.o \
	${OBJECTDIR}/ProcessUtility.o \
	${OBJECTDIR}/SpeedyAgent.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64 -pthread -std=c++0x
CXXFLAGS=-m64 -pthread -std=c++0x

# Fortran Compiler Flags
FFLAGS=-m64

# Assembler Flags
ASFLAGS=--64

# Link Libraries and Options
LDLIBSOPTIONS=-Wl,-rpath,'/Speedy/lib' -L/Speedy/lib -lUFC_64 -ldl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SpeedyAgent

/Speedy/bin/SpeedyAgent: /Speedy/lib/libUFC_64.so

/Speedy/bin/SpeedyAgent: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyAgent ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread -lrt

${OBJECTDIR}/_ext/8fedd00c/SpeedyManager.o: ../SpeedyManager/SpeedyManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8fedd00c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../SpeedyManager -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8fedd00c/SpeedyManager.o ../SpeedyManager/SpeedyManager.cpp

${OBJECTDIR}/_ext/8fedd00c/UserDB.o: ../SpeedyManager/UserDB.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8fedd00c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../SpeedyManager -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8fedd00c/UserDB.o ../SpeedyManager/UserDB.cpp

${OBJECTDIR}/LogParserThread.o: LogParserThread.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../SpeedyManager -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogParserThread.o LogParserThread.cc

${OBJECTDIR}/ProcessUtility.o: ProcessUtility.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../SpeedyManager -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProcessUtility.o ProcessUtility.cc

${OBJECTDIR}/SpeedyAgent.o: SpeedyAgent.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../SpeedyManager -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyAgent.o SpeedyAgent.cpp

# Subprojects
.build-subprojects:
	cd ../UFCD && ${MAKE}  -f Makefile CONF=LINUX_Release_64

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r /Speedy/bin/libUFC_64.so
	${RM} /Speedy/bin/SpeedyAgent

# Subprojects
.clean-subprojects:
	cd ../UFCD && ${MAKE}  -f Makefile CONF=LINUX_Release_64 clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
