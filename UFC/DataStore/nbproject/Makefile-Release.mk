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
CND_PLATFORM=GNU-Solaris-Sparc
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include DataStore-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Exception.o \
	${OBJECTDIR}/Field.o \
	${OBJECTDIR}/Record.o \
	${OBJECTDIR}/Schema.o \
	${OBJECTDIR}/Table.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /MBus/lib/DataStore.a

/MBus/lib/DataStore.a: ${OBJECTFILES}
	${MKDIR} -p /MBus/lib
	${RM} /MBus/lib/DataStore.a
	${AR} -rv /MBus/lib/DataStore.a ${OBJECTFILES} 
	$(RANLIB) /MBus/lib/DataStore.a

${OBJECTDIR}/Exception.o: Exception.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__SOLARIS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Exception.o Exception.cpp

${OBJECTDIR}/Field.o: Field.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__SOLARIS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Field.o Field.cpp

${OBJECTDIR}/Record.o: Record.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__SOLARIS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Record.o Record.cpp

${OBJECTDIR}/Schema.o: Schema.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__SOLARIS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Schema.o Schema.cpp

${OBJECTDIR}/Table.o: Table.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__SOLARIS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Table.o Table.cpp

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
