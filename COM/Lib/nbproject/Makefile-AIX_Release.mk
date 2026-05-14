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
CND_CONF=AIX_Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/b6418c09/Utility_Exc.o \
	${OBJECTDIR}/CA/CABasicObjects.o \
	${OBJECTDIR}/EmbadedResource.o \
	${OBJECTDIR}/MarketDataMessage/TMDBaseMessage.o \
	${OBJECTDIR}/MarketDataMessage/TNewsMessage.o \
	${OBJECTDIR}/MessageDeliver/TMdListener.o \
	${OBJECTDIR}/MessageDeliver/TMdMessage.o \
	${OBJECTDIR}/MessageDeliver/TMdTransport.o \
	${OBJECTDIR}/MessageDeliver/TThreadApp.o \
	${OBJECTDIR}/OrderConnection/FuturesSymbolUtility.o \
	${OBJECTDIR}/OrderConnection/HandleExchangeConfirm.o \
	${OBJECTDIR}/OrderConnection/HandleExchangeFilled.o \
	${OBJECTDIR}/OrderConnection/HandleForeignExchangeConfirm.o \
	${OBJECTDIR}/OrderConnection/HandleForeignExchangeFilled.o \
	${OBJECTDIR}/OrderConnection/HandleTouchOrderResponse.o \
	${OBJECTDIR}/OrderConnection/MessageRender.o \
	${OBJECTDIR}/OrderConnection/RecoverThread.o \
	${OBJECTDIR}/OrderConnection/SendMessageToExchange.o \
	${OBJECTDIR}/OrderConnection/SendTouchOrderCmd.o \
	${OBJECTDIR}/OrderConnection/TTaifexConnection.o \
	${OBJECTDIR}/OrderMessage/TBaseMessage.o \
	${OBJECTDIR}/OrderMessage/TCancelOrderMessage.o \
	${OBJECTDIR}/OrderMessage/TExecutionReportMessage.o \
	${OBJECTDIR}/OrderMessage/TNetworkID.o \
	${OBJECTDIR}/OrderMessage/TNewOrderMessage.o \
	${OBJECTDIR}/OrderMessage/TOrderStatusRequest.o \
	${OBJECTDIR}/OrderMessage/TQuoteCancelMessage.o \
	${OBJECTDIR}/OrderMessage/TQuoteMessage.o \
	${OBJECTDIR}/OrderMessage/TQuoteRequestMessage.o \
	${OBJECTDIR}/OrderMessage/TReplaceOrderMessage.o \
	${OBJECTDIR}/OrderMessage/TTouchOrderCommand.o \
	${OBJECTDIR}/OrderMessage/TTriggeringCondition.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread -fPIC
CXXFLAGS=-pthread -fPIC

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /MBus/lib/MessageDeliver_Lib.a

/MBus/lib/MessageDeliver_Lib.a: ${OBJECTFILES}
	${MKDIR} -p /MBus/lib
	${RM} /MBus/lib/MessageDeliver_Lib.a
	${AR} -rv /MBus/lib/MessageDeliver_Lib.a ${OBJECTFILES} 
	$(RANLIB) /MBus/lib/MessageDeliver_Lib.a

${OBJECTDIR}/_ext/b6418c09/Utility_Exc.o: ../../ExchangeUtility/Utility_Exc.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/b6418c09
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b6418c09/Utility_Exc.o ../../ExchangeUtility/Utility_Exc.cpp

${OBJECTDIR}/CA/CABasicObjects.o: CA/CABasicObjects.cpp
	${MKDIR} -p ${OBJECTDIR}/CA
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CA/CABasicObjects.o CA/CABasicObjects.cpp

${OBJECTDIR}/EmbadedResource.o: EmbadedResource.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EmbadedResource.o EmbadedResource.cpp

${OBJECTDIR}/MarketDataMessage/TMDBaseMessage.o: MarketDataMessage/TMDBaseMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataMessage/TMDBaseMessage.o MarketDataMessage/TMDBaseMessage.cpp

${OBJECTDIR}/MarketDataMessage/TNewsMessage.o: MarketDataMessage/TNewsMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/MarketDataMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MarketDataMessage/TNewsMessage.o MarketDataMessage/TNewsMessage.cpp

${OBJECTDIR}/MessageDeliver/TMdListener.o: MessageDeliver/TMdListener.cpp
	${MKDIR} -p ${OBJECTDIR}/MessageDeliver
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MessageDeliver/TMdListener.o MessageDeliver/TMdListener.cpp

${OBJECTDIR}/MessageDeliver/TMdMessage.o: MessageDeliver/TMdMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/MessageDeliver
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MessageDeliver/TMdMessage.o MessageDeliver/TMdMessage.cpp

${OBJECTDIR}/MessageDeliver/TMdTransport.o: MessageDeliver/TMdTransport.cpp
	${MKDIR} -p ${OBJECTDIR}/MessageDeliver
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MessageDeliver/TMdTransport.o MessageDeliver/TMdTransport.cpp

${OBJECTDIR}/MessageDeliver/TThreadApp.o: MessageDeliver/TThreadApp.cpp
	${MKDIR} -p ${OBJECTDIR}/MessageDeliver
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MessageDeliver/TThreadApp.o MessageDeliver/TThreadApp.cpp

${OBJECTDIR}/OrderConnection/FuturesSymbolUtility.o: OrderConnection/FuturesSymbolUtility.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/FuturesSymbolUtility.o OrderConnection/FuturesSymbolUtility.cpp

${OBJECTDIR}/OrderConnection/HandleExchangeConfirm.o: OrderConnection/HandleExchangeConfirm.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/HandleExchangeConfirm.o OrderConnection/HandleExchangeConfirm.cpp

${OBJECTDIR}/OrderConnection/HandleExchangeFilled.o: OrderConnection/HandleExchangeFilled.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/HandleExchangeFilled.o OrderConnection/HandleExchangeFilled.cpp

${OBJECTDIR}/OrderConnection/HandleForeignExchangeConfirm.o: OrderConnection/HandleForeignExchangeConfirm.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/HandleForeignExchangeConfirm.o OrderConnection/HandleForeignExchangeConfirm.cpp

${OBJECTDIR}/OrderConnection/HandleForeignExchangeFilled.o: OrderConnection/HandleForeignExchangeFilled.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/HandleForeignExchangeFilled.o OrderConnection/HandleForeignExchangeFilled.cpp

${OBJECTDIR}/OrderConnection/HandleTouchOrderResponse.o: OrderConnection/HandleTouchOrderResponse.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/HandleTouchOrderResponse.o OrderConnection/HandleTouchOrderResponse.cpp

${OBJECTDIR}/OrderConnection/MessageRender.o: OrderConnection/MessageRender.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/MessageRender.o OrderConnection/MessageRender.cpp

${OBJECTDIR}/OrderConnection/RecoverThread.o: OrderConnection/RecoverThread.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/RecoverThread.o OrderConnection/RecoverThread.cpp

${OBJECTDIR}/OrderConnection/SendMessageToExchange.o: OrderConnection/SendMessageToExchange.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/SendMessageToExchange.o OrderConnection/SendMessageToExchange.cpp

${OBJECTDIR}/OrderConnection/SendTouchOrderCmd.o: OrderConnection/SendTouchOrderCmd.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/SendTouchOrderCmd.o OrderConnection/SendTouchOrderCmd.cpp

${OBJECTDIR}/OrderConnection/TTaifexConnection.o: OrderConnection/TTaifexConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderConnection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderConnection/TTaifexConnection.o OrderConnection/TTaifexConnection.cpp

${OBJECTDIR}/OrderMessage/TBaseMessage.o: OrderMessage/TBaseMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TBaseMessage.o OrderMessage/TBaseMessage.cpp

${OBJECTDIR}/OrderMessage/TCancelOrderMessage.o: OrderMessage/TCancelOrderMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TCancelOrderMessage.o OrderMessage/TCancelOrderMessage.cpp

${OBJECTDIR}/OrderMessage/TExecutionReportMessage.o: OrderMessage/TExecutionReportMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TExecutionReportMessage.o OrderMessage/TExecutionReportMessage.cpp

${OBJECTDIR}/OrderMessage/TNetworkID.o: OrderMessage/TNetworkID.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TNetworkID.o OrderMessage/TNetworkID.cpp

${OBJECTDIR}/OrderMessage/TNewOrderMessage.o: OrderMessage/TNewOrderMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TNewOrderMessage.o OrderMessage/TNewOrderMessage.cpp

${OBJECTDIR}/OrderMessage/TOrderStatusRequest.o: OrderMessage/TOrderStatusRequest.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TOrderStatusRequest.o OrderMessage/TOrderStatusRequest.cpp

${OBJECTDIR}/OrderMessage/TQuoteCancelMessage.o: OrderMessage/TQuoteCancelMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TQuoteCancelMessage.o OrderMessage/TQuoteCancelMessage.cpp

${OBJECTDIR}/OrderMessage/TQuoteMessage.o: OrderMessage/TQuoteMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TQuoteMessage.o OrderMessage/TQuoteMessage.cpp

${OBJECTDIR}/OrderMessage/TQuoteRequestMessage.o: OrderMessage/TQuoteRequestMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TQuoteRequestMessage.o OrderMessage/TQuoteRequestMessage.cpp

${OBJECTDIR}/OrderMessage/TReplaceOrderMessage.o: OrderMessage/TReplaceOrderMessage.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TReplaceOrderMessage.o OrderMessage/TReplaceOrderMessage.cpp

${OBJECTDIR}/OrderMessage/TTouchOrderCommand.o: OrderMessage/TTouchOrderCommand.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TTouchOrderCommand.o OrderMessage/TTouchOrderCommand.cpp

${OBJECTDIR}/OrderMessage/TTriggeringCondition.o: OrderMessage/TTriggeringCondition.cpp
	${MKDIR} -p ${OBJECTDIR}/OrderMessage
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D__AIX -I../../Migo -I../../UFC -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OrderMessage/TTriggeringCondition.o OrderMessage/TTriggeringCondition.cpp

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
