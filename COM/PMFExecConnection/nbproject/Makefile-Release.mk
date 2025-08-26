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
	${OBJECTDIR}/MessageStore.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/PMFExecConnection.o \
	${OBJECTDIR}/TandemAdapter.o \
	${OBJECTDIR}/CommandListener.o

# C Compiler Flags
CFLAGS=-pthread

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../Lib/./libSpeedyAPI.a /MBus/lib/UFC.a /MBus/lib/TAIFEXLib.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/PMFExecConnection

/Speedy/bin/PMFExecConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/PMFExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/PMFExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/MessageStore.o: MessageStore.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../Lib -I../../UFC -I../../Migo -I../../TAIFEXLib -o ${OBJECTDIR}/MessageStore.o MessageStore.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../Lib -I../../UFC -I../../Migo -I../../TAIFEXLib -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/PMFExecConnection.o: PMFExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../Lib -I../../UFC -I../../Migo -I../../TAIFEXLib -o ${OBJECTDIR}/PMFExecConnection.o PMFExecConnection.cpp

${OBJECTDIR}/TandemAdapter.o: TandemAdapter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../Lib -I../../UFC -I../../Migo -I../../TAIFEXLib -o ${OBJECTDIR}/TandemAdapter.o TandemAdapter.cpp

${OBJECTDIR}/CommandListener.o: CommandListener.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../Lib -I../../UFC -I../../Migo -I../../TAIFEXLib -o ${OBJECTDIR}/CommandListener.o CommandListener.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} /Speedy/bin/PMFExecConnection

# Subprojects
.clean-subprojects:
