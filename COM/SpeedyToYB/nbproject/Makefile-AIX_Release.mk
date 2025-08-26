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
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/YuantaExecConnection.o

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
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SpeedyToYB

/Speedy/bin/SpeedyToYB: ${BUILD_SUBPROJECTS}

/Speedy/bin/SpeedyToYB: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SpeedyToYB ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/YuantaExecConnection.o: YuantaExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/YuantaExecConnection.o YuantaExecConnection.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release
	${RM} /Speedy/bin/SpeedyToYB

# Subprojects
.clean-subprojects:
