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
OBJECTDIR=build/Release/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/SpeedyFIFOOrderConnection.o \
	${OBJECTDIR}/MessageDispatcher.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../Lib/./libSpeedyAPI.a /MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SpeedyFIFOOrderConnection

/Speedy/bin/SpeedyFIFOOrderConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/SpeedyFIFOOrderConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyFIFOOrderConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cc

${OBJECTDIR}/SpeedyFIFOOrderConnection.o: SpeedyFIFOOrderConnection.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/SpeedyFIFOOrderConnection.o SpeedyFIFOOrderConnection.cc

${OBJECTDIR}/MessageDispatcher.o: MessageDispatcher.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/MessageDispatcher.o MessageDispatcher.cc

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} /Speedy/bin/SpeedyFIFOOrderConnection

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
