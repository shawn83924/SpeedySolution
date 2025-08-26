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
CND_CONF=LINUX_Release64
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
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64 -Wno-deprecated
CXXFLAGS=-m64 -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/tibco/tibrv/lib /MBus/lib/libSpeedyAPI_64.a /MBus/lib/libUFC_64.a -lpthread /usr/local/lib/libcurl.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/GCRiskManager

/Speedy/bin/GCRiskManager: /MBus/lib/libSpeedyAPI_64.a

/Speedy/bin/GCRiskManager: /MBus/lib/libUFC_64.a

/Speedy/bin/GCRiskManager: /usr/local/lib/libcurl.a

/Speedy/bin/GCRiskManager: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/GCRiskManager ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/GCRMConnection.o: GCRMConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../OrderMessage -I../OrderConnection -I../../UFC -I../../Migo -I../../curl_32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GCRMConnection.o GCRMConnection.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../OrderMessage -I../OrderConnection -I../../UFC -I../../Migo -I../../curl_32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/PEasyFTPClient.o: PEasyFTPClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../OrderMessage -I../OrderConnection -I../../UFC -I../../Migo -I../../curl_32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PEasyFTPClient.o PEasyFTPClient.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release_64
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release_64

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/GCRiskManager

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release_64 clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release_64 clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
