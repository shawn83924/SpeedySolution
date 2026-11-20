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
CND_CONF=Solaris_Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile_Migo.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/MApp.o \
	${OBJECTDIR}/MComponent.o \
	${OBJECTDIR}/MException.o \
	${OBJECTDIR}/MNode.o \
	${OBJECTDIR}/MSubscriber.o \
	${OBJECTDIR}/MTree.o \
	${OBJECTDIR}/MigoHeader.o \
	${OBJECTDIR}/Sigo.o \
	${OBJECTDIR}/SubjectDB.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads -fPIC
CXXFLAGS=-pthreads -fPIC

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /MBus/lib/Migo.a

/MBus/lib/Migo.a: ${OBJECTFILES}
	${MKDIR} -p /MBus/lib
	${RM} /MBus/lib/Migo.a
	${AR} -rv /MBus/lib/Migo.a ${OBJECTFILES} 
	$(RANLIB) /MBus/lib/Migo.a

${OBJECTDIR}/MApp.o: MApp.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MApp.o MApp.cpp

${OBJECTDIR}/MComponent.o: MComponent.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MComponent.o MComponent.cpp

${OBJECTDIR}/MException.o: MException.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MException.o MException.cpp

${OBJECTDIR}/MNode.o: MNode.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MNode.o MNode.cpp

${OBJECTDIR}/MSubscriber.o: MSubscriber.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MSubscriber.o MSubscriber.cpp

${OBJECTDIR}/MTree.o: MTree.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MTree.o MTree.cpp

${OBJECTDIR}/MigoHeader.o: MigoHeader.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MigoHeader.o MigoHeader.cpp

${OBJECTDIR}/Sigo.o: Sigo.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Sigo.o Sigo.cpp

${OBJECTDIR}/SubjectDB.o: SubjectDB.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SubjectDB.o SubjectDB.cpp

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
