#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
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
FC=

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/AIX_Release/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/SpeedyOrderConnection.o \
	${OBJECTDIR}/Main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../Lib/UFC.a ../Lib/MessageDeliver_Lib.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/AIX_Release/GNU-Generic/speedyorderconnection

dist/AIX_Release/GNU-Generic/speedyorderconnection: ${OBJECTFILES}
	${MKDIR} -p dist/AIX_Release/GNU-Generic
	${LINK.cc} -o dist/AIX_Release/GNU-Generic/speedyorderconnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SpeedyOrderConnection.o: SpeedyOrderConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/SpeedyOrderConnection.o SpeedyOrderConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/AIX_Release
	${RM} dist/AIX_Release/GNU-Generic/speedyorderconnection

# Subprojects
.clean-subprojects:
