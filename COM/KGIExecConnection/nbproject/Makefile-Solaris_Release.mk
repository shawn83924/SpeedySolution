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
FC=g77

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Solaris_Release/GNU-Solaris-Sparc

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/KGIExecConnection.o \
	${OBJECTDIR}/Main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads
CXXFLAGS=-pthreads

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lsocket -lnsl -lpthread -lrt /MBus/lib/MessageDeliver_Lib.a /MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/KGIExecConnection

/Speedy/bin/KGIExecConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/KGIExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthreads -o /Speedy/bin/KGIExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/KGIExecConnection.o: KGIExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -I../../UFC -I../../Migo -o ${OBJECTDIR}/KGIExecConnection.o KGIExecConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_Release
	${RM} /Speedy/bin/KGIExecConnection

# Subprojects
.clean-subprojects:
