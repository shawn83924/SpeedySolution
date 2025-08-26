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
CND_CONF=AIX_Debug
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
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SpeedyAgent

/Speedy/bin/SpeedyAgent: /MBus/lib/UFC.a

/Speedy/bin/SpeedyAgent: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyAgent ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread

${OBJECTDIR}/_ext/8fedd00c/SpeedyManager.o: ../SpeedyManager/SpeedyManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8fedd00c
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8fedd00c/SpeedyManager.o ../SpeedyManager/SpeedyManager.cpp

${OBJECTDIR}/_ext/8fedd00c/UserDB.o: ../SpeedyManager/UserDB.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8fedd00c
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8fedd00c/UserDB.o ../SpeedyManager/UserDB.cpp

${OBJECTDIR}/LogParserThread.o: LogParserThread.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogParserThread.o LogParserThread.cc

${OBJECTDIR}/ProcessUtility.o: ProcessUtility.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProcessUtility.o ProcessUtility.cc

${OBJECTDIR}/SpeedyAgent.o: SpeedyAgent.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyAgent.o SpeedyAgent.cpp

# Subprojects
.build-subprojects:
	cd ../UFC && ${MAKE} -f Makefile_UFC.mk CONF=AIX_Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../UFC && ${MAKE} -f Makefile_UFC.mk CONF=AIX_Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
