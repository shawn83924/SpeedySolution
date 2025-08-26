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
CND_CONF=Solaris_GCC4.3.2_Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ChinaTrustExecConnection.o \
	${OBJECTDIR}/Main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads -Wno-deprecated -fast
CXXFLAGS=-pthreads -Wno-deprecated -fast

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lsocket -lnsl -lpthread -lrt ../Lib/./libSpeedyAPI.a /MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Solaris_GCC4.3.2_Release.mk /Speedy/bin/ChinaTrustExecConnection

/Speedy/bin/ChinaTrustExecConnection: ../Lib/./libSpeedyAPI.a

/Speedy/bin/ChinaTrustExecConnection: /MBus/lib/libUFC.a

/Speedy/bin/ChinaTrustExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/ChinaTrustExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/ChinaTrustExecConnection.o: ChinaTrustExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -I../../Migo -I../../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/ChinaTrustExecConnection.o ChinaTrustExecConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -I../../Migo -I../../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=Solaris_GCC4.3.2_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_GCC4.3.2_Release
	${RM} /Speedy/bin/ChinaTrustExecConnection

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=Solaris_GCC4.3.2_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
