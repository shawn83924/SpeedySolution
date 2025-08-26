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
	${OBJECTDIR}/MessageStore.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/PMFExecConnection.o \
	${OBJECTDIR}/TandemAdapter.o \
	${OBJECTDIR}/CommandListener.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/UFC.a ../Lib/./libSpeedyAPI.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Debug/GNU-Generic/pmfexecconnection

dist/Debug/GNU-Generic/pmfexecconnection: ${BUILD_SUBPROJECTS}

dist/Debug/GNU-Generic/pmfexecconnection: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Generic
	${LINK.cc} -o dist/Debug/GNU-Generic/pmfexecconnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/MessageStore.o: MessageStore.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -I../../UFC -I../Lib -o ${OBJECTDIR}/MessageStore.o MessageStore.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -I../../UFC -I../Lib -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/PMFExecConnection.o: PMFExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -I../../UFC -I../Lib -o ${OBJECTDIR}/PMFExecConnection.o PMFExecConnection.cpp

${OBJECTDIR}/TandemAdapter.o: TandemAdapter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -I../../UFC -I../Lib -o ${OBJECTDIR}/TandemAdapter.o TandemAdapter.cpp

${OBJECTDIR}/CommandListener.o: CommandListener.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -I../../UFC -I../Lib -o ${OBJECTDIR}/CommandListener.o CommandListener.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Generic/pmfexecconnection

# Subprojects
.clean-subprojects:
