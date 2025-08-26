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
FC=
AS=

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=LINUX_Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/TUserData.o \
	${OBJECTDIR}/MLExecConnection.o \
	${OBJECTDIR}/MultilegSymbol.o \
	${OBJECTDIR}/Main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L. -L../Lib ../Lib/./libSpeedyAPI.a /MBus/lib/libUFC.a ../../TAIFEXLib/../../lib/TAIFEXLib.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-LINUX_Release.mk /Speedy/bin/MLExecConnection

/Speedy/bin/MLExecConnection: ../Lib/./libSpeedyAPI.a

/Speedy/bin/MLExecConnection: /MBus/lib/libUFC.a

/Speedy/bin/MLExecConnection: ../../TAIFEXLib/../../lib/TAIFEXLib.a

/Speedy/bin/MLExecConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/MLExecConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/TUserData.o: nbproject/Makefile-${CND_CONF}.mk TUserData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -o ${OBJECTDIR}/TUserData.o TUserData.cpp

${OBJECTDIR}/MLExecConnection.o: nbproject/Makefile-${CND_CONF}.mk MLExecConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -o ${OBJECTDIR}/MLExecConnection.o MLExecConnection.cpp

${OBJECTDIR}/MultilegSymbol.o: nbproject/Makefile-${CND_CONF}.mk MultilegSymbol.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -o ${OBJECTDIR}/MultilegSymbol.o MultilegSymbol.cpp

${OBJECTDIR}/Main.o: nbproject/Makefile-${CND_CONF}.mk Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release
	cd ../../TAIFEXLib && ${MAKE}  -f Makefile CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/LINUX_Release
	${RM} /Speedy/bin/MLExecConnection

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=LINUX_Release clean
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean
	cd ../../TAIFEXLib && ${MAKE}  -f Makefile CONF=LINUX_Release clean
