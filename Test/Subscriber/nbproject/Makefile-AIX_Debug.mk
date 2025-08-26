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

# Object Files
OBJECTFILES= \
	build/AIX_Debug/GNU-Generic/Subscriber.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=\
	-lpthread \
	/MBus/lib/Migo.a \
	/MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /MBus/Tools/Subscriber

/MBus/Tools/Subscriber: ${BUILD_SUBPROJECTS}

/MBus/Tools/Subscriber: ${OBJECTFILES}
	${MKDIR} -p /MBus/Tools
	${LINK.cc} -pthread -o /MBus/Tools/Subscriber ${OBJECTFILES} ${LDLIBSOPTIONS} 

build/AIX_Debug/GNU-Generic/Subscriber.o: Subscriber.cpp 
	${MKDIR} -p build/AIX_Debug/GNU-Generic
	$(COMPILE.cc) -g -Wall -D__AIX -I../../UFC -I../../Migo -o build/AIX_Debug/GNU-Generic/Subscriber.o Subscriber.cpp

# Subprojects
.build-subprojects:
	cd ../../Migo && ${MAKE} -f Makefile_Migo.mk CONF=AIX_Debug
	cd ../../UFC && ${MAKE} -f Makefile_UFC.mk CONF=AIX_Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Debug
	${RM} /MBus/Tools/Subscriber

# Subprojects
.clean-subprojects:
	cd ../../Migo && ${MAKE} -f Makefile_Migo.mk CONF=AIX_Debug clean
	cd ../../UFC && ${MAKE} -f Makefile_UFC.mk CONF=AIX_Debug clean
