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
FC=g77
AS=

# Macros
CND_PLATFORM=GNU-Solaris-Sparc
CND_CONF=Solaris_Release_FCM
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Win32X25Connection.o \
	${OBJECTDIR}/TMPInt8.o \
	${OBJECTDIR}/TMPInt32.o \
	${OBJECTDIR}/LinkSubSystemTAIFEX.o \
	${OBJECTDIR}/_ext/_DOTDOT/ExchangeUtility/Utility.o \
	${OBJECTDIR}/TMPSymbol.o \
	${OBJECTDIR}/LinkSubSystem.o \
	${OBJECTDIR}/FMessages.o \
	${OBJECTDIR}/TMPDataFormat.o \
	${OBJECTDIR}/TMPType.o \
	${OBJECTDIR}/MessageHeader.o \
	${OBJECTDIR}/TMPInt64.o \
	${OBJECTDIR}/SocketConnection.o \
	${OBJECTDIR}/IBMX25Connection.o \
	${OBJECTDIR}/TMPCharArray.o \
	${OBJECTDIR}/XBIODefine.o \
	${OBJECTDIR}/SUNX25Connection.o \
	${OBJECTDIR}/TMPMessage.o \
	${OBJECTDIR}/LMessages.o \
	${OBJECTDIR}/CMessages.o \
	${OBJECTDIR}/RMessages.o \
	${OBJECTDIR}/TMPInt16.o \
	${OBJECTDIR}/LinkSubSystemFCM.o \
	${OBJECTDIR}/MultilegSymbol.o \
	${OBJECTDIR}/TMPTime.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads -fPIC -Wno-deprecated
CXXFLAGS=-pthreads -fPIC -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Solaris_Release_FCM.mk ../lib/libTAIFEXLibFCM.a

../lib/libTAIFEXLibFCM.a: ${OBJECTFILES}
	${MKDIR} -p ../lib
	${RM} ../lib/libTAIFEXLibFCM.a
	${AR} rv ../lib/libTAIFEXLibFCM.a ${OBJECTFILES} 
	$(RANLIB) ../lib/libTAIFEXLibFCM.a

${OBJECTDIR}/Win32X25Connection.o: nbproject/Makefile-${CND_CONF}.mk Win32X25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/Win32X25Connection.o Win32X25Connection.cpp

${OBJECTDIR}/TMPInt8.o: nbproject/Makefile-${CND_CONF}.mk TMPInt8.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPInt8.o TMPInt8.cpp

${OBJECTDIR}/TMPInt32.o: nbproject/Makefile-${CND_CONF}.mk TMPInt32.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPInt32.o TMPInt32.cpp

${OBJECTDIR}/LinkSubSystemTAIFEX.o: nbproject/Makefile-${CND_CONF}.mk LinkSubSystemTAIFEX.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinkSubSystemTAIFEX.o LinkSubSystemTAIFEX.cpp

${OBJECTDIR}/_ext/_DOTDOT/ExchangeUtility/Utility.o: nbproject/Makefile-${CND_CONF}.mk ../ExchangeUtility/Utility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/ExchangeUtility
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/ExchangeUtility/Utility.o ../ExchangeUtility/Utility.cpp

${OBJECTDIR}/TMPSymbol.o: nbproject/Makefile-${CND_CONF}.mk TMPSymbol.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPSymbol.o TMPSymbol.cpp

${OBJECTDIR}/LinkSubSystem.o: nbproject/Makefile-${CND_CONF}.mk LinkSubSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinkSubSystem.o LinkSubSystem.cpp

${OBJECTDIR}/FMessages.o: nbproject/Makefile-${CND_CONF}.mk FMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/FMessages.o FMessages.cpp

${OBJECTDIR}/TMPDataFormat.o: nbproject/Makefile-${CND_CONF}.mk TMPDataFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPDataFormat.o TMPDataFormat.cpp

${OBJECTDIR}/TMPType.o: nbproject/Makefile-${CND_CONF}.mk TMPType.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPType.o TMPType.cpp

${OBJECTDIR}/MessageHeader.o: nbproject/Makefile-${CND_CONF}.mk MessageHeader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/MessageHeader.o MessageHeader.cpp

${OBJECTDIR}/TMPInt64.o: nbproject/Makefile-${CND_CONF}.mk TMPInt64.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPInt64.o TMPInt64.cpp

${OBJECTDIR}/SocketConnection.o: nbproject/Makefile-${CND_CONF}.mk SocketConnection.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/SocketConnection.o SocketConnection.cc

${OBJECTDIR}/IBMX25Connection.o: nbproject/Makefile-${CND_CONF}.mk IBMX25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/IBMX25Connection.o IBMX25Connection.cpp

${OBJECTDIR}/TMPCharArray.o: nbproject/Makefile-${CND_CONF}.mk TMPCharArray.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPCharArray.o TMPCharArray.cpp

${OBJECTDIR}/XBIODefine.o: nbproject/Makefile-${CND_CONF}.mk XBIODefine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/XBIODefine.o XBIODefine.cpp

${OBJECTDIR}/SUNX25Connection.o: nbproject/Makefile-${CND_CONF}.mk SUNX25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/SUNX25Connection.o SUNX25Connection.cpp

${OBJECTDIR}/TMPMessage.o: nbproject/Makefile-${CND_CONF}.mk TMPMessage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPMessage.o TMPMessage.cpp

${OBJECTDIR}/LMessages.o: nbproject/Makefile-${CND_CONF}.mk LMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/LMessages.o LMessages.cpp

${OBJECTDIR}/CMessages.o: nbproject/Makefile-${CND_CONF}.mk CMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/CMessages.o CMessages.cpp

${OBJECTDIR}/RMessages.o: nbproject/Makefile-${CND_CONF}.mk RMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/RMessages.o RMessages.cpp

${OBJECTDIR}/TMPInt16.o: nbproject/Makefile-${CND_CONF}.mk TMPInt16.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPInt16.o TMPInt16.cpp

${OBJECTDIR}/LinkSubSystemFCM.o: nbproject/Makefile-${CND_CONF}.mk LinkSubSystemFCM.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinkSubSystemFCM.o LinkSubSystemFCM.cpp

${OBJECTDIR}/MultilegSymbol.o: nbproject/Makefile-${CND_CONF}.mk MultilegSymbol.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/MultilegSymbol.o MultilegSymbol.cpp

${OBJECTDIR}/TMPTime.o: nbproject/Makefile-${CND_CONF}.mk TMPTime.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -Wall -D__SOLARIS -D_FOR_FCM -MMD -MP -MF $@.d -o ${OBJECTDIR}/TMPTime.o TMPTime.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Solaris_Release_FCM
	${RM} ../lib/libTAIFEXLibFCM.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
