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
	${OBJECTDIR}/SQMessage.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/SystexOrderConnection.o

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
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SystexOrderConnection

/Speedy/bin/SystexOrderConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/SystexOrderConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/SystexOrderConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SQMessage.o: SQMessage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/SQMessage.o SQMessage.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/SystexOrderConnection.o: SystexOrderConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/SystexOrderConnection.o SystexOrderConnection.cpp

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release
	${RM} /Speedy/bin/SystexOrderConnection

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Release clean
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
