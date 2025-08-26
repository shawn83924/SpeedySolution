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
	build/Solaris_Release/GNU-Solaris-Sparc/Subscriber.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=\
	-lsocket \
	-lnsl \
	-lpthread \
	/MBus/lib/Migo.a \
	/MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /MBus/Tools/Subscriber

/MBus/Tools/Subscriber: ${BUILD_SUBPROJECTS}

/MBus/Tools/Subscriber: ${OBJECTFILES}
	${MKDIR} -p /MBus/Tools
	${LINK.cc} -pthreads -o /MBus/Tools/Subscriber ${OBJECTFILES} ${LDLIBSOPTIONS} 

build/Solaris_Release/GNU-Solaris-Sparc/Subscriber.o: Subscriber.cpp 
	${MKDIR} -p build/Solaris_Release/GNU-Solaris-Sparc
	$(COMPILE.cc) -O2 -Wall -I../../UFC -I../../Migo -o build/Solaris_Release/GNU-Solaris-Sparc/Subscriber.o Subscriber.cpp

# Subprojects
.build-subprojects:
	cd ../../Migo && ${MAKE} -f Makefile_Migo.mk CONF=Release
	cd ../../UFC && ${MAKE} -f Makefile_UFC.mk CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_Release
	${RM} /MBus/Tools/Subscriber

# Subprojects
.clean-subprojects:
	cd ../../Migo && ${MAKE} -f Makefile_Migo.mk CONF=Release clean
	cd ../../UFC && ${MAKE} -f Makefile_UFC.mk CONF=Release clean
