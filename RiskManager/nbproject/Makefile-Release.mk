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
	${OBJECTDIR}/WriteLog.o \
	${OBJECTDIR}/CheckManager.o \
	${OBJECTDIR}/AccountStore.o \
	${OBJECTDIR}/RiskManager.o

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
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/RiskManager

/Speedy/bin/RiskManager: ${BUILD_SUBPROJECTS}

/Speedy/bin/RiskManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/RiskManager ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/WriteLog.o: WriteLog.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../UFC -o ${OBJECTDIR}/WriteLog.o WriteLog.cpp

${OBJECTDIR}/CheckManager.o: CheckManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../UFC -o ${OBJECTDIR}/CheckManager.o CheckManager.cpp

${OBJECTDIR}/AccountStore.o: AccountStore.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../UFC -o ${OBJECTDIR}/AccountStore.o AccountStore.cpp

${OBJECTDIR}/RiskManager.o: RiskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -D__AIX -I../UFC -o ${OBJECTDIR}/RiskManager.o RiskManager.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} /Speedy/bin/RiskManager

# Subprojects
.clean-subprojects:
