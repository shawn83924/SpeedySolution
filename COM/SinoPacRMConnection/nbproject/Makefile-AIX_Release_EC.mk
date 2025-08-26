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
LDLIBSOPTIONS=-L/usr/tibco/tibrv/lib /MBus/lib/UFC.a ../Lib/./libSpeedyAPI.a -ltibrv -ltibrvcm -ltibrvcmq -ltibrvft -ltibrvcpp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SinoPacRMConnection

/Speedy/bin/SinoPacRMConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/SinoPacRMConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SinoPacRMConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../Migo -I../../UFC -I/usr/tibco/tibrv/include -I../Lib/OrderConnection -I../Lib/OrderMessage -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/SinoPacRMConnection.o: SinoPacRMConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../Migo -I../../UFC -I/usr/tibco/tibrv/include -I../Lib/OrderConnection -I../Lib/OrderMessage -o ${OBJECTDIR}/SinoPacRMConnection.o SinoPacRMConnection.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release_EC
	${RM} /Speedy/bin/SinoPacRMConnection

# Subprojects
.clean-subprojects:
