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
	${OBJECTDIR}/SpeedyFIFORiskManager.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/MessageDispatcher.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SpeedyFIFORiskManager

/Speedy/bin/SpeedyFIFORiskManager: ${BUILD_SUBPROJECTS}

/Speedy/bin/SpeedyFIFORiskManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SpeedyFIFORiskManager ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SpeedyFIFORiskManager.o: SpeedyFIFORiskManager.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/SpeedyFIFORiskManager.o SpeedyFIFORiskManager.cc

${OBJECTDIR}/Main.o: Main.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cc

${OBJECTDIR}/MessageDispatcher.o: MessageDispatcher.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/MessageDispatcher.o MessageDispatcher.cc

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} /Speedy/bin/SpeedyFIFORiskManager

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
