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
include Makefile_SpeedyManager.mk

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/UserDB.o \
	${OBJECTDIR}/SpeedyManager.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads -Wno-deprecated
CXXFLAGS=-pthreads -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lsocket -lnsl -lpthread -lrt /MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Solaris_GCC4.3.2_Release.mk /Speedy/bin/SpeedyManager

/Speedy/bin/SpeedyManager: /MBus/lib/libUFC.a

/Speedy/bin/SpeedyManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthreads -o /Speedy/bin/SpeedyManager ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/UserDB.o: nbproject/Makefile-${CND_CONF}.mk UserDB.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/UserDB.o UserDB.cpp

${OBJECTDIR}/SpeedyManager.o: nbproject/Makefile-${CND_CONF}.mk SpeedyManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -I../UFC -MMD -MP -MF $@.d -o ${OBJECTDIR}/SpeedyManager.o SpeedyManager.cpp

# Subprojects
.build-subprojects:
	cd ../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Solaris_GCC4.3.2_Release
	${RM} /Speedy/bin/SpeedyManager

# Subprojects
.clean-subprojects:
	cd ../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
