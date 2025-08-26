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
OBJECTDIR=build/AIX_Release/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/SinoPacExecConnection.o \
	${OBJECTDIR}/Main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/tibco/tibrv/lib /MBus/lib/MessageDeliver_Lib.a /MBus/lib/UFC.a -ltibrv -ltibrvcm -ltibrvcmq -ltibrvft -ltibrvcpp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SinoPacExecConnection

/Speedy/bin/SinoPacExecConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/SinoPacExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SinoPacExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SinoPacExecConnection.o: SinoPacExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -o ${OBJECTDIR}/SinoPacExecConnection.o SinoPacExecConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -I/usr/tibco/tibrv/include -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release
	${RM} /Speedy/bin/SinoPacExecConnection

# Subprojects
.clean-subprojects:
