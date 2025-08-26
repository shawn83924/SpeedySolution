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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=g77
AS=

# Macros
CND_PLATFORM=GNU-Solaris-Sparc
CND_CONF=AIX_Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile_MBus.mk

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/MBusMain.o \
	${OBJECTDIR}/SendingThreadPool.o \
	${OBJECTDIR}/MBusServer.o

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
	${MAKE}  -f nbproject/Makefile-AIX_Debug.mk /Speedy/bin/MBus

/Speedy/bin/MBus: /MBus/lib/UFC.a

/Speedy/bin/MBus: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/MBus ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/MBusMain.o: nbproject/Makefile-${CND_CONF}.mk MBusMain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -I../Migo -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/MBusMain.o MBusMain.cpp

${OBJECTDIR}/SendingThreadPool.o: nbproject/Makefile-${CND_CONF}.mk SendingThreadPool.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -I../Migo -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/SendingThreadPool.o SendingThreadPool.cpp

${OBJECTDIR}/MBusServer.o: nbproject/Makefile-${CND_CONF}.mk MBusServer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -I../Migo -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/MBusServer.o MBusServer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Debug
	${RM} /Speedy/bin/MBus

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
