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
	${OBJECTDIR}/PMFExecToFile.o \
	${OBJECTDIR}/main.o

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
.build-conf: ${BUILD_SUBPROJECTS} dist/Debug/GNU-Generic/pmfexectofile

dist/Debug/GNU-Generic/pmfexectofile: ${BUILD_SUBPROJECTS}

dist/Debug/GNU-Generic/pmfexectofile: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Generic
	${LINK.cc} -o dist/Debug/GNU-Generic/pmfexectofile ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/PMFExecToFile.o: PMFExecToFile.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -D__AIX -I../../UFC -o ${OBJECTDIR}/PMFExecToFile.o PMFExecToFile.cpp

${OBJECTDIR}/main.o: main.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -D__AIX -I../../UFC -o ${OBJECTDIR}/main.o main.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Generic/pmfexectofile

# Subprojects
.clean-subprojects:
