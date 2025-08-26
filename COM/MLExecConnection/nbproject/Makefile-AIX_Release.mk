#
# Generated Makefile - do not edit!
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
AS=

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=AIX_Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/TUserData.o \
	${OBJECTDIR}/MLExecConnection.o \
	${OBJECTDIR}/MultilegSymbol.o \
	${OBJECTDIR}/Main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../Lib/UFC.a ../Lib/MessageDeliver_Lib.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-AIX_Release.mk dist/AIX_Release/GNU-Linux-x86/mlexecconnection

dist/AIX_Release/GNU-Linux-x86/mlexecconnection: ../Lib/UFC.a

dist/AIX_Release/GNU-Linux-x86/mlexecconnection: ../Lib/MessageDeliver_Lib.a

dist/AIX_Release/GNU-Linux-x86/mlexecconnection: ${OBJECTFILES}
	${MKDIR} -p dist/AIX_Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mlexecconnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/TUserData.o: nbproject/Makefile-${CND_CONF}.mk TUserData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/TUserData.o TUserData.cpp

${OBJECTDIR}/MLExecConnection.o: nbproject/Makefile-${CND_CONF}.mk MLExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/MLExecConnection.o MLExecConnection.cpp

${OBJECTDIR}/MultilegSymbol.o: nbproject/Makefile-${CND_CONF}.mk MultilegSymbol.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/MultilegSymbol.o MultilegSymbol.cpp

${OBJECTDIR}/Main.o: nbproject/Makefile-${CND_CONF}.mk Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/AIX_Release
	${RM} dist/AIX_Release/GNU-Linux-x86/mlexecconnection

# Subprojects
.clean-subprojects:
