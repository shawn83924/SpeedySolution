#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add custumized code.
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
FC=gfortran

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/AIX_Release_EC/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/SinoPacExecConnection.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/SinoPacFIXConnection.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/tibco/tibrv/lib ../Lib/./libSpeedyAPI.a /MBus/lib/UFC.a -ltibrv -ltibrvcm -ltibrvcmq -ltibrvft -ltibrvcpp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SinoPacFIXExecConnection

/Speedy/bin/SinoPacFIXExecConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/SinoPacFIXExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SinoPacFIXExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SinoPacExecConnection.o: SinoPacExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -D_FOR_EC -I/usr/tibco/tibrv/include -I../../Migo -I../../UFC -o ${OBJECTDIR}/SinoPacExecConnection.o SinoPacExecConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -D_FOR_EC -I/usr/tibco/tibrv/include -I../../Migo -I../../UFC -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/SinoPacFIXConnection.o: SinoPacFIXConnection.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -D_FOR_EC -I/usr/tibco/tibrv/include -I../../Migo -I../../UFC -o ${OBJECTDIR}/SinoPacFIXConnection.o SinoPacFIXConnection.cc

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release_EC
	${RM} /Speedy/bin/SinoPacFIXExecConnection

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
