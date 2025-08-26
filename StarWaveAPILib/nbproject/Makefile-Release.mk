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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ChartServerConnection/ChartServerConnection.o \
	${OBJECTDIR}/MarketDataConnection/CMEMessageHandler.o \
	${OBJECTDIR}/MarketDataConnection/ChinaMessageHandler.o \
	${OBJECTDIR}/MarketDataConnection/MarketDataConnection.o \
	${OBJECTDIR}/MarketDataConnection/TAIFEXMessageHandler.o \
	${OBJECTDIR}/MarketDataConnection/TMDThreadApp.o \
	${OBJECTDIR}/MarketDataConnection/TWSEMessageHandler.o \
	${OBJECTDIR}/MarketDataConnection/Utility.o \
	${OBJECTDIR}/MarketDataMessage/DepthObject.o \
	${OBJECTDIR}/MarketDataMessage/MarketDataMessage.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -pthread -fPIC
CXXFLAGS=-m32 -pthread -fPIC

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../Release/StarWaveAPI.a

../Release/StarWaveAPI.a: ${OBJECTFILES}
	${MKDIR} -p ../Release
	${RM} ../Release/StarWaveAPI.a
	${AR} -rv ../Release/StarWaveAPI.a ${OBJECTFILES} 
	$(RANLIB) ../Release/StarWaveAPI.a

${OBJECTDIR}/ChartServerConnection/ChartServerConnection.o: ChartServerConnection/ChartServerConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/ChartServerConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ChartServerConnection/ChartServerConnection.o ChartServerConnection/ChartServerConnection.cpp

${OBJECTDIR}/MarketDataConnection/CMEMessageHandler.o: MarketDataConnection/CMEMessageHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/CMEMessageHandler.o MarketDataConnection/CMEMessageHandler.cpp

${OBJECTDIR}/MarketDataConnection/ChinaMessageHandler.o: MarketDataConnection/ChinaMessageHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/ChinaMessageHandler.o MarketDataConnection/ChinaMessageHandler.cpp

${OBJECTDIR}/MarketDataConnection/MarketDataConnection.o: MarketDataConnection/MarketDataConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/MarketDataConnection.o MarketDataConnection/MarketDataConnection.cpp

${OBJECTDIR}/MarketDataConnection/TAIFEXMessageHandler.o: MarketDataConnection/TAIFEXMessageHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/TAIFEXMessageHandler.o MarketDataConnection/TAIFEXMessageHandler.cpp

${OBJECTDIR}/MarketDataConnection/TMDThreadApp.o: MarketDataConnection/TMDThreadApp.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/TMDThreadApp.o MarketDataConnection/TMDThreadApp.cpp

${OBJECTDIR}/MarketDataConnection/TWSEMessageHandler.o: MarketDataConnection/TWSEMessageHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/TWSEMessageHandler.o MarketDataConnection/TWSEMessageHandler.cpp

${OBJECTDIR}/MarketDataConnection/Utility.o: MarketDataConnection/Utility.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataConnection/Utility.o MarketDataConnection/Utility.cpp

${OBJECTDIR}/MarketDataMessage/DepthObject.o: MarketDataMessage/DepthObject.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataMessage/DepthObject.o MarketDataMessage/DepthObject.cpp

${OBJECTDIR}/MarketDataMessage/MarketDataMessage.o: MarketDataMessage/MarketDataMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__LINUX -IMarketDataConnection -IMarketDataMessage -I../Migo -I../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataMessage/MarketDataMessage.o MarketDataMessage/MarketDataMessage.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
