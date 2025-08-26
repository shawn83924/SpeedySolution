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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc-4.3.2
CCC=g++-4.3.2
CXX=g++-4.3.2
FC=
AS=

# Macros
CND_PLATFORM=GNU-4.3.2Sun-Solaris-Sparc
CND_CONF=Solaris_Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/CancelOrderConnection.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-Wno-deprecated -fPIC -fast
CXXFLAGS=-Wno-deprecated -fPIC -fast

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread -lsocket -lnsl -lrt ../Lib/./libSpeedyAPI.a /MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Solaris_Release.mk /Speedy/bin/SpeedyEmergencyCancel

/Speedy/bin/SpeedyEmergencyCancel: ../Lib/./libSpeedyAPI.a

/Speedy/bin/SpeedyEmergencyCancel: /MBus/lib/libUFC.a

/Speedy/bin/SpeedyEmergencyCancel: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyEmergencyCancel ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -I../../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/CancelOrderConnection.o: CancelOrderConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -I../../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/CancelOrderConnection.o CancelOrderConnection.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=Solaris_GCC4.3.2_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_Release
	${RM} /Speedy/bin/SpeedyEmergencyCancel

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=Solaris_GCC4.3.2_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
