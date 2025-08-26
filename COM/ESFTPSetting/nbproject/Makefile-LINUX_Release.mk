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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=LINUX_Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/GCRMConnection.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/PEasyFTPClient.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32 -Wno-deprecated
CXXFLAGS=-m32 -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=-m32

# Assembler Flags
ASFLAGS=--32

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/libSpeedyAPI.a /MBus/lib/libUFC.a -lpthread ../../curl_32/lib/.libs/libcurl.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/ESFTPSetting

/Speedy/bin/ESFTPSetting: /MBus/lib/libSpeedyAPI.a

/Speedy/bin/ESFTPSetting: /MBus/lib/libUFC.a

/Speedy/bin/ESFTPSetting: ../../curl_32/lib/.libs/libcurl.a

/Speedy/bin/ESFTPSetting: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/ESFTPSetting ${OBJECTFILES} ${LDLIBSOPTIONS} -lrt

${OBJECTDIR}/GCRMConnection.o: GCRMConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -I../../curl_32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GCRMConnection.o GCRMConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -I../../curl_32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/PEasyFTPClient.o: PEasyFTPClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -I../../curl_32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PEasyFTPClient.o PEasyFTPClient.cpp

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/ESFTPSetting

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
