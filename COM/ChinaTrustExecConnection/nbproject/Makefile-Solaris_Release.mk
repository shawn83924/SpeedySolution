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
FC=g77

# Macros
PLATFORM=GNU-Solaris-Sparc

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Solaris_Release/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/ChinaTrustExecConnection.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads
CXXFLAGS=-pthreads

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/MessageDeliver_Lib.a /MBus/lib/UFC.a -lsocket -lnsl -lpthread -lrt

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Solaris_Release.mk /Speedy/bin/ChinaTrustExecConnection

/Speedy/bin/ChinaTrustExecConnection: /MBus/lib/MessageDeliver_Lib.a

/Speedy/bin/ChinaTrustExecConnection: /MBus/lib/UFC.a

/Speedy/bin/ChinaTrustExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/ChinaTrustExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -D__SOLARIS -I../../Migo -I../../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/ChinaTrustExecConnection.o: ChinaTrustExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -D__SOLARIS -I../../Migo -I../../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/ChinaTrustExecConnection.o ChinaTrustExecConnection.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_Release
	${RM} /Speedy/bin/ChinaTrustExecConnection

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
