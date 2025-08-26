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
	${OBJECTDIR}/FubonDBExecConnection.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/UFC.a ../Lib/./libSpeedyAPI.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/FubonDBExecConnection

/Speedy/bin/FubonDBExecConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/FubonDBExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/FubonDBExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/FubonDBExecConnection.o: FubonDBExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/FubonDBExecConnection.o FubonDBExecConnection.cpp

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release
	${RM} /Speedy/bin/FubonDBExecConnection

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
