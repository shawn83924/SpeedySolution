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
CND_CONF=AIX_Release_R6
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
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads
CXXFLAGS=-pthreads

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/tibco/tibrv/lib ../Lib/./libSpeedyAPI.a /MBus/lib/UFC.a -ltibrv -ltibrvcm -ltibrvcmq -ltibrvft -ltibrvcpp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SinoPacExecConnection

/Speedy/bin/SinoPacExecConnection: ../Lib/./libSpeedyAPI.a

/Speedy/bin/SinoPacExecConnection: /MBus/lib/UFC.a

/Speedy/bin/SinoPacExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SinoPacExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/GCRMConnection.o: GCRMConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -o ${OBJECTDIR}/GCRMConnection.o GCRMConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/SinoPacExecConnection

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
