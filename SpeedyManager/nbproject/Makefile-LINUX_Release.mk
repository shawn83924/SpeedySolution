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
include Makefile_SpeedyManager.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/SpeedyManager.o \
	${OBJECTDIR}/UserDB.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -pthread
CXXFLAGS=-m32 -pthread

# Fortran Compiler Flags
FFLAGS=-s -m32

# Assembler Flags
ASFLAGS=--32

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SpeedyManager

/Speedy/bin/SpeedyManager: /MBus/lib/libUFC.a

/Speedy/bin/SpeedyManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyManager ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread

${OBJECTDIR}/SpeedyManager.o: SpeedyManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyManager.o SpeedyManager.cpp

${OBJECTDIR}/UserDB.o: UserDB.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UserDB.o UserDB.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd ../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/SpeedyManager

# Subprojects
.clean-subprojects:
	cd ../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
