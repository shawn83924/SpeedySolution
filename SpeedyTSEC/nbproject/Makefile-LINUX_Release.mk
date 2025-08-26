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
include Makefile_SpeedyTSEC.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1183113382/Acceptor.o \
	${OBJECTDIR}/_ext/1183113382/CallStack.o \
	${OBJECTDIR}/_ext/1183113382/ConfigLexer.o \
	${OBJECTDIR}/_ext/1183113382/DataDictionary.o \
	${OBJECTDIR}/_ext/1183113382/Dictionary.o \
	${OBJECTDIR}/_ext/1183113382/FieldMap.o \
	${OBJECTDIR}/_ext/1183113382/FieldTypes.o \
	${OBJECTDIR}/_ext/1183113382/FileLog.o \
	${OBJECTDIR}/_ext/1183113382/FileStore.o \
	${OBJECTDIR}/_ext/1183113382/Group.o \
	${OBJECTDIR}/_ext/1183113382/HttpConnection.o \
	${OBJECTDIR}/_ext/1183113382/HttpMessage.o \
	${OBJECTDIR}/_ext/1183113382/HttpParser.o \
	${OBJECTDIR}/_ext/1183113382/HttpServer.o \
	${OBJECTDIR}/_ext/1183113382/Initiator.o \
	${OBJECTDIR}/_ext/1183113382/LIBXML_DOMDocument.o \
	${OBJECTDIR}/_ext/1183113382/Log.o \
	${OBJECTDIR}/_ext/1183113382/Message.o \
	${OBJECTDIR}/_ext/1183113382/MessageSorters.o \
	${OBJECTDIR}/_ext/1183113382/MessageStore.o \
	${OBJECTDIR}/_ext/1183113382/Parser.o \
	${OBJECTDIR}/_ext/1183113382/Session.o \
	${OBJECTDIR}/_ext/1183113382/SessionFactory.o \
	${OBJECTDIR}/_ext/1183113382/SessionSettings.o \
	${OBJECTDIR}/_ext/1183113382/SessionTime.o \
	${OBJECTDIR}/_ext/1183113382/Settings.o \
	${OBJECTDIR}/_ext/1183113382/SocketAcceptor.o \
	${OBJECTDIR}/_ext/1183113382/SocketConnection.o \
	${OBJECTDIR}/_ext/1183113382/SocketConnector.o \
	${OBJECTDIR}/_ext/1183113382/SocketInitiator.o \
	${OBJECTDIR}/_ext/1183113382/SocketMonitor.o \
	${OBJECTDIR}/_ext/1183113382/SocketServer.o \
	${OBJECTDIR}/_ext/1183113382/ThreadedSocketAcceptor.o \
	${OBJECTDIR}/_ext/1183113382/ThreadedSocketConnection.o \
	${OBJECTDIR}/_ext/1183113382/ThreadedSocketInitiator.o \
	${OBJECTDIR}/_ext/1183113382/Utility.o \
	${OBJECTDIR}/_ext/1183113382/strptime.o \
	${OBJECTDIR}/MarketSchedule.o \
	${OBJECTDIR}/OrderInfoStore.o \
	${OBJECTDIR}/ProcessMonitor.o \
	${OBJECTDIR}/QueryPVC.o \
	${OBJECTDIR}/Speedy.o \
	${OBJECTDIR}/SpeedyConfirmConnectionObject.o \
	${OBJECTDIR}/SpeedyFIXManager.o \
	${OBJECTDIR}/SpeedyFIXOrderClient.o \
	${OBJECTDIR}/SpeedyObjectsManager.o \
	${OBJECTDIR}/SpeedyOrderClient.o \
	${OBJECTDIR}/SpeedyOrderConnectionObject.o \
	${OBJECTDIR}/UFCFileLog.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -pthread
CXXFLAGS=-m32 -pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lxml2 /MBus/lib/TSECLib.a /MBus/lib/libUFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /Speedy/bin/SpeedyTSEC

/Speedy/bin/SpeedyTSEC: /MBus/lib/TSECLib.a

/Speedy/bin/SpeedyTSEC: /MBus/lib/libUFC.a

/Speedy/bin/SpeedyTSEC: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyTSEC ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread -s

${OBJECTDIR}/_ext/1183113382/Acceptor.o: ../../../../QuickfixTWSE/src/C++/Acceptor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Acceptor.o ../../../../QuickfixTWSE/src/C++/Acceptor.cpp

${OBJECTDIR}/_ext/1183113382/CallStack.o: ../../../../QuickfixTWSE/src/C++/CallStack.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/CallStack.o ../../../../QuickfixTWSE/src/C++/CallStack.cpp

${OBJECTDIR}/_ext/1183113382/ConfigLexer.o: ../../../../QuickfixTWSE/src/C++/ConfigLexer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/ConfigLexer.o ../../../../QuickfixTWSE/src/C++/ConfigLexer.cpp

${OBJECTDIR}/_ext/1183113382/DataDictionary.o: ../../../../QuickfixTWSE/src/C++/DataDictionary.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/DataDictionary.o ../../../../QuickfixTWSE/src/C++/DataDictionary.cpp

${OBJECTDIR}/_ext/1183113382/Dictionary.o: ../../../../QuickfixTWSE/src/C++/Dictionary.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Dictionary.o ../../../../QuickfixTWSE/src/C++/Dictionary.cpp

${OBJECTDIR}/_ext/1183113382/FieldMap.o: ../../../../QuickfixTWSE/src/C++/FieldMap.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/FieldMap.o ../../../../QuickfixTWSE/src/C++/FieldMap.cpp

${OBJECTDIR}/_ext/1183113382/FieldTypes.o: ../../../../QuickfixTWSE/src/C++/FieldTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/FieldTypes.o ../../../../QuickfixTWSE/src/C++/FieldTypes.cpp

${OBJECTDIR}/_ext/1183113382/FileLog.o: ../../../../QuickfixTWSE/src/C++/FileLog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/FileLog.o ../../../../QuickfixTWSE/src/C++/FileLog.cpp

${OBJECTDIR}/_ext/1183113382/FileStore.o: ../../../../QuickfixTWSE/src/C++/FileStore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/FileStore.o ../../../../QuickfixTWSE/src/C++/FileStore.cpp

${OBJECTDIR}/_ext/1183113382/Group.o: ../../../../QuickfixTWSE/src/C++/Group.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Group.o ../../../../QuickfixTWSE/src/C++/Group.cpp

${OBJECTDIR}/_ext/1183113382/HttpConnection.o: ../../../../QuickfixTWSE/src/C++/HttpConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/HttpConnection.o ../../../../QuickfixTWSE/src/C++/HttpConnection.cpp

${OBJECTDIR}/_ext/1183113382/HttpMessage.o: ../../../../QuickfixTWSE/src/C++/HttpMessage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/HttpMessage.o ../../../../QuickfixTWSE/src/C++/HttpMessage.cpp

${OBJECTDIR}/_ext/1183113382/HttpParser.o: ../../../../QuickfixTWSE/src/C++/HttpParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/HttpParser.o ../../../../QuickfixTWSE/src/C++/HttpParser.cpp

${OBJECTDIR}/_ext/1183113382/HttpServer.o: ../../../../QuickfixTWSE/src/C++/HttpServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/HttpServer.o ../../../../QuickfixTWSE/src/C++/HttpServer.cpp

${OBJECTDIR}/_ext/1183113382/Initiator.o: ../../../../QuickfixTWSE/src/C++/Initiator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Initiator.o ../../../../QuickfixTWSE/src/C++/Initiator.cpp

${OBJECTDIR}/_ext/1183113382/LIBXML_DOMDocument.o: ../../../../QuickfixTWSE/src/C++/LIBXML_DOMDocument.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/LIBXML_DOMDocument.o ../../../../QuickfixTWSE/src/C++/LIBXML_DOMDocument.cpp

${OBJECTDIR}/_ext/1183113382/Log.o: ../../../../QuickfixTWSE/src/C++/Log.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Log.o ../../../../QuickfixTWSE/src/C++/Log.cpp

${OBJECTDIR}/_ext/1183113382/Message.o: ../../../../QuickfixTWSE/src/C++/Message.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Message.o ../../../../QuickfixTWSE/src/C++/Message.cpp

${OBJECTDIR}/_ext/1183113382/MessageSorters.o: ../../../../QuickfixTWSE/src/C++/MessageSorters.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/MessageSorters.o ../../../../QuickfixTWSE/src/C++/MessageSorters.cpp

${OBJECTDIR}/_ext/1183113382/MessageStore.o: ../../../../QuickfixTWSE/src/C++/MessageStore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/MessageStore.o ../../../../QuickfixTWSE/src/C++/MessageStore.cpp

${OBJECTDIR}/_ext/1183113382/Parser.o: ../../../../QuickfixTWSE/src/C++/Parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Parser.o ../../../../QuickfixTWSE/src/C++/Parser.cpp

${OBJECTDIR}/_ext/1183113382/Session.o: ../../../../QuickfixTWSE/src/C++/Session.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Session.o ../../../../QuickfixTWSE/src/C++/Session.cpp

${OBJECTDIR}/_ext/1183113382/SessionFactory.o: ../../../../QuickfixTWSE/src/C++/SessionFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SessionFactory.o ../../../../QuickfixTWSE/src/C++/SessionFactory.cpp

${OBJECTDIR}/_ext/1183113382/SessionSettings.o: ../../../../QuickfixTWSE/src/C++/SessionSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SessionSettings.o ../../../../QuickfixTWSE/src/C++/SessionSettings.cpp

${OBJECTDIR}/_ext/1183113382/SessionTime.o: ../../../../QuickfixTWSE/src/C++/SessionTime.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SessionTime.o ../../../../QuickfixTWSE/src/C++/SessionTime.cpp

${OBJECTDIR}/_ext/1183113382/Settings.o: ../../../../QuickfixTWSE/src/C++/Settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Settings.o ../../../../QuickfixTWSE/src/C++/Settings.cpp

${OBJECTDIR}/_ext/1183113382/SocketAcceptor.o: ../../../../QuickfixTWSE/src/C++/SocketAcceptor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SocketAcceptor.o ../../../../QuickfixTWSE/src/C++/SocketAcceptor.cpp

${OBJECTDIR}/_ext/1183113382/SocketConnection.o: ../../../../QuickfixTWSE/src/C++/SocketConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SocketConnection.o ../../../../QuickfixTWSE/src/C++/SocketConnection.cpp

${OBJECTDIR}/_ext/1183113382/SocketConnector.o: ../../../../QuickfixTWSE/src/C++/SocketConnector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SocketConnector.o ../../../../QuickfixTWSE/src/C++/SocketConnector.cpp

${OBJECTDIR}/_ext/1183113382/SocketInitiator.o: ../../../../QuickfixTWSE/src/C++/SocketInitiator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SocketInitiator.o ../../../../QuickfixTWSE/src/C++/SocketInitiator.cpp

${OBJECTDIR}/_ext/1183113382/SocketMonitor.o: ../../../../QuickfixTWSE/src/C++/SocketMonitor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SocketMonitor.o ../../../../QuickfixTWSE/src/C++/SocketMonitor.cpp

${OBJECTDIR}/_ext/1183113382/SocketServer.o: ../../../../QuickfixTWSE/src/C++/SocketServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/SocketServer.o ../../../../QuickfixTWSE/src/C++/SocketServer.cpp

${OBJECTDIR}/_ext/1183113382/ThreadedSocketAcceptor.o: ../../../../QuickfixTWSE/src/C++/ThreadedSocketAcceptor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/ThreadedSocketAcceptor.o ../../../../QuickfixTWSE/src/C++/ThreadedSocketAcceptor.cpp

${OBJECTDIR}/_ext/1183113382/ThreadedSocketConnection.o: ../../../../QuickfixTWSE/src/C++/ThreadedSocketConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/ThreadedSocketConnection.o ../../../../QuickfixTWSE/src/C++/ThreadedSocketConnection.cpp

${OBJECTDIR}/_ext/1183113382/ThreadedSocketInitiator.o: ../../../../QuickfixTWSE/src/C++/ThreadedSocketInitiator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/ThreadedSocketInitiator.o ../../../../QuickfixTWSE/src/C++/ThreadedSocketInitiator.cpp

${OBJECTDIR}/_ext/1183113382/Utility.o: ../../../../QuickfixTWSE/src/C++/Utility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/Utility.o ../../../../QuickfixTWSE/src/C++/Utility.cpp

${OBJECTDIR}/_ext/1183113382/strptime.o: ../../../../QuickfixTWSE/src/C++/strptime.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1183113382
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1183113382/strptime.o ../../../../QuickfixTWSE/src/C++/strptime.c

${OBJECTDIR}/MarketSchedule.o: MarketSchedule.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketSchedule.o MarketSchedule.cpp

${OBJECTDIR}/OrderInfoStore.o: OrderInfoStore.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderInfoStore.o OrderInfoStore.cpp

${OBJECTDIR}/ProcessMonitor.o: ProcessMonitor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ProcessMonitor.o ProcessMonitor.cpp

${OBJECTDIR}/QueryPVC.o: QueryPVC.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/QueryPVC.o QueryPVC.cpp

${OBJECTDIR}/Speedy.o: Speedy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Speedy.o Speedy.cpp

${OBJECTDIR}/SpeedyConfirmConnectionObject.o: SpeedyConfirmConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyConfirmConnectionObject.o SpeedyConfirmConnectionObject.cpp

${OBJECTDIR}/SpeedyFIXManager.o: SpeedyFIXManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyFIXManager.o SpeedyFIXManager.cpp

${OBJECTDIR}/SpeedyFIXOrderClient.o: SpeedyFIXOrderClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyFIXOrderClient.o SpeedyFIXOrderClient.cpp

${OBJECTDIR}/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyObjectsManager.o SpeedyObjectsManager.cpp

${OBJECTDIR}/SpeedyOrderClient.o: SpeedyOrderClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyOrderClient.o SpeedyOrderClient.cpp

${OBJECTDIR}/SpeedyOrderConnectionObject.o: SpeedyOrderConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SpeedyOrderConnectionObject.o SpeedyOrderConnectionObject.cpp

${OBJECTDIR}/UFCFileLog.o: UFCFileLog.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -D__LINUX -I../UFC -I../Migo -I../ExchangeUtility -I../../../../quickfix -I../../../../quickfix/src/C++ -I/usr/include/libxml2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UFCFileLog.o UFCFileLog.cpp

# Subprojects
.build-subprojects:
	cd ../TSECLib && ${MAKE}  -f Makefile_TSECLib.mk CONF=LINUX_Release
	cd ../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /Speedy/bin/SpeedyTSEC

# Subprojects
.clean-subprojects:
	cd ../TSECLib && ${MAKE}  -f Makefile_TSECLib.mk CONF=LINUX_Release clean
	cd ../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=LINUX_Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
