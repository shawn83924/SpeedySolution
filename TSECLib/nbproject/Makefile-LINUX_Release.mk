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
include Makefile_TSECLib.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/292091738/TOrderRecord.o \
	${OBJECTDIR}/_ext/292091738/Utility.o \
	${OBJECTDIR}/AMessages.o \
	${OBJECTDIR}/EMessages.o \
	${OBJECTDIR}/ExMessages.o \
	${OBJECTDIR}/ExchangeConst.o \
	${OBJECTDIR}/FMessages.o \
	${OBJECTDIR}/IBMX25Connection.o \
	${OBJECTDIR}/LMessages.o \
	${OBJECTDIR}/LinkSubSystem.o \
	${OBJECTDIR}/MessageHeader.o \
	${OBJECTDIR}/OMessages.o \
	${OBJECTDIR}/PMessages.o \
	${OBJECTDIR}/RMessages.o \
	${OBJECTDIR}/SMessages.o \
	${OBJECTDIR}/SUNX25Connection.o \
	${OBJECTDIR}/SocketConnection.o \
	${OBJECTDIR}/TMessages.o \
	${OBJECTDIR}/VMessages.o


# C Compiler Flags
CFLAGS=-pthread -fPIC

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /MBus/lib/TSECLib.a

/MBus/lib/TSECLib.a: ${OBJECTFILES}
	${MKDIR} -p /MBus/lib
	${RM} /MBus/lib/TSECLib.a
	${AR} -rv /MBus/lib/TSECLib.a ${OBJECTFILES} 
	$(RANLIB) /MBus/lib/TSECLib.a

${OBJECTDIR}/_ext/292091738/TOrderRecord.o: ../ExchangeUtility/TOrderRecord.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/292091738
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/292091738/TOrderRecord.o ../ExchangeUtility/TOrderRecord.cpp

${OBJECTDIR}/_ext/292091738/Utility.o: ../ExchangeUtility/Utility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/292091738
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/292091738/Utility.o ../ExchangeUtility/Utility.cpp

${OBJECTDIR}/AMessages.o: AMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AMessages.o AMessages.cpp

${OBJECTDIR}/EMessages.o: EMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EMessages.o EMessages.cpp

${OBJECTDIR}/ExMessages.o: ExMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ExMessages.o ExMessages.cpp

${OBJECTDIR}/ExchangeConst.o: ExchangeConst.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ExchangeConst.o ExchangeConst.cpp

${OBJECTDIR}/FMessages.o: FMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FMessages.o FMessages.cpp

${OBJECTDIR}/IBMX25Connection.o: IBMX25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IBMX25Connection.o IBMX25Connection.cpp

${OBJECTDIR}/LMessages.o: LMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LMessages.o LMessages.cpp

${OBJECTDIR}/LinkSubSystem.o: LinkSubSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LinkSubSystem.o LinkSubSystem.cpp

${OBJECTDIR}/MessageHeader.o: MessageHeader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MessageHeader.o MessageHeader.cpp

${OBJECTDIR}/OMessages.o: OMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OMessages.o OMessages.cpp

${OBJECTDIR}/PMessages.o: PMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PMessages.o PMessages.cpp

${OBJECTDIR}/RMessages.o: RMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RMessages.o RMessages.cpp

${OBJECTDIR}/SMessages.o: SMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SMessages.o SMessages.cpp

${OBJECTDIR}/SUNX25Connection.o: SUNX25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SUNX25Connection.o SUNX25Connection.cpp

${OBJECTDIR}/SocketConnection.o: SocketConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SocketConnection.o SocketConnection.cpp

${OBJECTDIR}/TMessages.o: TMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TMessages.o TMessages.cpp

${OBJECTDIR}/VMessages.o: VMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../ExchangeUtility -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VMessages.o VMessages.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /MBus/lib/TSECLib.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
