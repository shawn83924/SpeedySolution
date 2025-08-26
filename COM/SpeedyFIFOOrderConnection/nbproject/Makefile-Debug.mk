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
OBJECTDIR=build/Debug/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/MessageDispatcher.o \
	${OBJECTDIR}/SpeedyFIFOOrderConnection.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Debug/GNU-Generic/speedyfifoorderconnection

dist/Debug/GNU-Generic/speedyfifoorderconnection: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Generic
	${LINK.cc} -o dist/Debug/GNU-Generic/speedyfifoorderconnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -o ${OBJECTDIR}/Main.o Main.cc

${OBJECTDIR}/MessageDispatcher.o: MessageDispatcher.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -o ${OBJECTDIR}/MessageDispatcher.o MessageDispatcher.cc

${OBJECTDIR}/SpeedyFIFOOrderConnection.o: SpeedyFIFOOrderConnection.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -o ${OBJECTDIR}/SpeedyFIFOOrderConnection.o SpeedyFIFOOrderConnection.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Generic/speedyfifoorderconnection

# Subprojects
.clean-subprojects:
