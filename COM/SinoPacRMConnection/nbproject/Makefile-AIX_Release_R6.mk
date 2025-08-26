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
OBJECTDIR=build/AIX_Release_R6/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/SinoPacRMConnection.o

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
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SinoPacExecConnection

/Speedy/bin/SinoPacExecConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/SinoPacExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SinoPacExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/SinoPacRMConnection.o: SinoPacRMConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -o ${OBJECTDIR}/SinoPacRMConnection.o SinoPacRMConnection.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release_R6
	${RM} /Speedy/bin/SinoPacExecConnection

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
