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
CND_CONF=Solaris_Debug
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
CCFLAGS=-pthreads
CXXFLAGS=-pthreads

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lsocket -lnsl -lpthread /MBus/lib/UFC.a /MBus/lib/Migo.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Solaris_Debug.mk /Speedy/bin/MBus

/Speedy/bin/MBus: /MBus/lib/UFC.a

/Speedy/bin/MBus: /MBus/lib/Migo.a

/Speedy/bin/MBus: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthreads -o /Speedy/bin/MBus ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/MBusMain.o: nbproject/Makefile-${CND_CONF}.mk MBusMain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D_SOLARIS -I../Migo -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/MBusMain.o MBusMain.cpp

${OBJECTDIR}/SendingThreadPool.o: nbproject/Makefile-${CND_CONF}.mk SendingThreadPool.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D_SOLARIS -I../Migo -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/SendingThreadPool.o SendingThreadPool.cpp

${OBJECTDIR}/MBusServer.o: nbproject/Makefile-${CND_CONF}.mk MBusServer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D_SOLARIS -I../Migo -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/MBusServer.o MBusServer.cpp

# Subprojects
.build-subprojects:
	cd ../UFC && ${MAKE} -f Makefile_UFC.mk CONF=Solaris_Debug
	cd ../Migo && ${MAKE} -f Makefile_Migo.mk CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_Debug
	${RM} /Speedy/bin/MBus

# Subprojects
.clean-subprojects:
	cd ../UFC && ${MAKE} -f Makefile_UFC.mk CONF=Solaris_Debug clean
	cd ../Migo && ${MAKE} -f Makefile_Migo.mk CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
