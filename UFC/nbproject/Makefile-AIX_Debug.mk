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
FC=g77
AS=

# Macros
CND_PLATFORM=GNU-Solaris-Sparc
CND_CONF=AIX_Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile_UFC.mk

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/UIPC.o \
	${OBJECTDIR}/PThread.o \
	${OBJECTDIR}/Profiler.o \
	${OBJECTDIR}/NameValueMessage.o \
	${OBJECTDIR}/_ext/760415558/inftrees.o \
	${OBJECTDIR}/TDataFormat.o \
	${OBJECTDIR}/MD5.o \
	${OBJECTDIR}/_ext/760415558/trees.o \
	${OBJECTDIR}/MDS/MRowSet.o \
	${OBJECTDIR}/MDS/MDSCommObj.o \
	${OBJECTDIR}/MDS/MColumn.o \
	${OBJECTDIR}/StringBuffer.o \
	${OBJECTDIR}/TField.o \
	${OBJECTDIR}/_ext/761759085/SubjectDB.o \
	${OBJECTDIR}/_ext/760415558/inflate.o \
	${OBJECTDIR}/List.o \
	${OBJECTDIR}/_ext/760415558/compress.o \
	${OBJECTDIR}/MDS/MIndex.o \
	${OBJECTDIR}/_ext/760415558/gzwrite.o \
	${OBJECTDIR}/zlibutility.o \
	${OBJECTDIR}/MemoryStream.o \
	${OBJECTDIR}/MDS/MRow.o \
	${OBJECTDIR}/_ext/760415558/infback.o \
	${OBJECTDIR}/_ext/760415558/zutil.o \
	${OBJECTDIR}/TRecord.o \
	${OBJECTDIR}/_ext/761759085/MigoHeader.o \
	${OBJECTDIR}/_ext/761759085/MNode.o \
	${OBJECTDIR}/_ext/760415558/crc32.o \
	${OBJECTDIR}/_ext/760415558/minigzip.o \
	${OBJECTDIR}/PInt32.o \
	${OBJECTDIR}/_ext/760415558/deflate.o \
	${OBJECTDIR}/TParseData.o \
	${OBJECTDIR}/_ext/760415558/uncompr.o \
	${OBJECTDIR}/PUDPClient.o \
	${OBJECTDIR}/NetUtility.o \
	${OBJECTDIR}/_ext/760415558/inffast.o \
	${OBJECTDIR}/TRenderData.o \
	${OBJECTDIR}/_ext/761759085/Sigo.o \
	${OBJECTDIR}/_ext/760415558/gzlib.o \
	${OBJECTDIR}/Utility.o \
	${OBJECTDIR}/TLicenseKey.o \
	${OBJECTDIR}/UDateTime.o \
	${OBJECTDIR}/SocketHeartBeat.o \
	${OBJECTDIR}/_ext/761759085/MApp.o \
	${OBJECTDIR}/iniFile.o \
	${OBJECTDIR}/_ext/760415558/adler32.o \
	${OBJECTDIR}/_ext/760415558/gzread.o \
	${OBJECTDIR}/PQueue.o \
	${OBJECTDIR}/_ext/761759085/MComponent.o \
	${OBJECTDIR}/MDS/MDSException.o \
	${OBJECTDIR}/AnsiString.o \
	${OBJECTDIR}/ThreadPool.o \
	${OBJECTDIR}/_ext/761759085/MTree.o \
	${OBJECTDIR}/TDataset.o \
	${OBJECTDIR}/Exception.o \
	${OBJECTDIR}/MDS/MSchema.o \
	${OBJECTDIR}/PSocket.o \
	${OBJECTDIR}/PClientSocket.o \
	${OBJECTDIR}/_ext/760415558/gzclose.o \
	${OBJECTDIR}/_ext/761759085/MSubscriber.o \
	${OBJECTDIR}/PServerSocket.o \
	${OBJECTDIR}/MDS/MTransactionStore.o \
	${OBJECTDIR}/MultiplexingServer.o \
	${OBJECTDIR}/TFixedFormat.o \
	${OBJECTDIR}/FileStream.o \
	${OBJECTDIR}/MDS/MTable.o \
	${OBJECTDIR}/BufferedLog.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-AIX_Debug.mk ../bin/UFC.a

../bin/UFC.a: ${OBJECTFILES}
	${MKDIR} -p ../bin
	${RM} ../bin/UFC.a
	${AR} -rv ../bin/UFC.a ${OBJECTFILES} 

${OBJECTDIR}/UIPC.o: UIPC.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/UIPC.o UIPC.cpp

${OBJECTDIR}/PThread.o: PThread.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PThread.o PThread.cpp

${OBJECTDIR}/Profiler.o: Profiler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/Profiler.o Profiler.cpp

${OBJECTDIR}/NameValueMessage.o: NameValueMessage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/NameValueMessage.o NameValueMessage.cpp

${OBJECTDIR}/_ext/760415558/inftrees.o: ../zlib/inftrees.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/inftrees.o ../zlib/inftrees.c

${OBJECTDIR}/TDataFormat.o: TDataFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TDataFormat.o TDataFormat.cpp

${OBJECTDIR}/MD5.o: MD5.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MD5.o MD5.cpp

${OBJECTDIR}/_ext/760415558/trees.o: ../zlib/trees.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/trees.o ../zlib/trees.c

${OBJECTDIR}/MDS/MRowSet.o: MDS/MRowSet.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MRowSet.o MDS/MRowSet.cpp

${OBJECTDIR}/MDS/MDSCommObj.o: MDS/MDSCommObj.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MDSCommObj.o MDS/MDSCommObj.cpp

${OBJECTDIR}/MDS/MColumn.o: MDS/MColumn.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MColumn.o MDS/MColumn.cpp

${OBJECTDIR}/StringBuffer.o: StringBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/StringBuffer.o StringBuffer.cpp

${OBJECTDIR}/TField.o: TField.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TField.o TField.cpp

${OBJECTDIR}/_ext/761759085/SubjectDB.o: ../Migo/SubjectDB.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/SubjectDB.o ../Migo/SubjectDB.cpp

${OBJECTDIR}/_ext/760415558/inflate.o: ../zlib/inflate.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/inflate.o ../zlib/inflate.c

${OBJECTDIR}/List.o: List.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/List.o List.cpp

${OBJECTDIR}/_ext/760415558/compress.o: ../zlib/compress.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/compress.o ../zlib/compress.c

${OBJECTDIR}/MDS/MIndex.o: MDS/MIndex.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MIndex.o MDS/MIndex.cpp

${OBJECTDIR}/_ext/760415558/gzwrite.o: ../zlib/gzwrite.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/gzwrite.o ../zlib/gzwrite.c

${OBJECTDIR}/zlibutility.o: zlibutility.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/zlibutility.o zlibutility.cpp

${OBJECTDIR}/MemoryStream.o: MemoryStream.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MemoryStream.o MemoryStream.cpp

${OBJECTDIR}/MDS/MRow.o: MDS/MRow.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MRow.o MDS/MRow.cpp

${OBJECTDIR}/_ext/760415558/infback.o: ../zlib/infback.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/infback.o ../zlib/infback.c

${OBJECTDIR}/_ext/760415558/zutil.o: ../zlib/zutil.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/zutil.o ../zlib/zutil.c

${OBJECTDIR}/TRecord.o: TRecord.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TRecord.o TRecord.cpp

${OBJECTDIR}/_ext/761759085/MigoHeader.o: ../Migo/MigoHeader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/MigoHeader.o ../Migo/MigoHeader.cpp

${OBJECTDIR}/_ext/761759085/MNode.o: ../Migo/MNode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/MNode.o ../Migo/MNode.cpp

${OBJECTDIR}/_ext/760415558/crc32.o: ../zlib/crc32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/crc32.o ../zlib/crc32.c

${OBJECTDIR}/_ext/760415558/minigzip.o: ../zlib/minigzip.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/minigzip.o ../zlib/minigzip.c

${OBJECTDIR}/PInt32.o: PInt32.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PInt32.o PInt32.cpp

${OBJECTDIR}/_ext/760415558/deflate.o: ../zlib/deflate.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/deflate.o ../zlib/deflate.c

${OBJECTDIR}/TParseData.o: TParseData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TParseData.o TParseData.cpp

${OBJECTDIR}/_ext/760415558/uncompr.o: ../zlib/uncompr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/uncompr.o ../zlib/uncompr.c

${OBJECTDIR}/PUDPClient.o: PUDPClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PUDPClient.o PUDPClient.cpp

${OBJECTDIR}/NetUtility.o: NetUtility.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/NetUtility.o NetUtility.cpp

${OBJECTDIR}/_ext/760415558/inffast.o: ../zlib/inffast.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/inffast.o ../zlib/inffast.c

${OBJECTDIR}/TRenderData.o: TRenderData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TRenderData.o TRenderData.cpp

${OBJECTDIR}/_ext/761759085/Sigo.o: ../Migo/Sigo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/Sigo.o ../Migo/Sigo.cpp

${OBJECTDIR}/_ext/760415558/gzlib.o: ../zlib/gzlib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/gzlib.o ../zlib/gzlib.c

${OBJECTDIR}/Utility.o: Utility.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/Utility.o Utility.cpp

${OBJECTDIR}/TLicenseKey.o: TLicenseKey.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TLicenseKey.o TLicenseKey.cpp

${OBJECTDIR}/UDateTime.o: UDateTime.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/UDateTime.o UDateTime.cpp

${OBJECTDIR}/SocketHeartBeat.o: SocketHeartBeat.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/SocketHeartBeat.o SocketHeartBeat.cpp

${OBJECTDIR}/_ext/761759085/MApp.o: ../Migo/MApp.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/MApp.o ../Migo/MApp.cpp

${OBJECTDIR}/iniFile.o: iniFile.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/iniFile.o iniFile.cpp

${OBJECTDIR}/_ext/760415558/adler32.o: ../zlib/adler32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/adler32.o ../zlib/adler32.c

${OBJECTDIR}/_ext/760415558/gzread.o: ../zlib/gzread.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/gzread.o ../zlib/gzread.c

${OBJECTDIR}/PQueue.o: PQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PQueue.o PQueue.cpp

${OBJECTDIR}/_ext/761759085/MComponent.o: ../Migo/MComponent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/MComponent.o ../Migo/MComponent.cpp

${OBJECTDIR}/MDS/MDSException.o: MDS/MDSException.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MDSException.o MDS/MDSException.cpp

${OBJECTDIR}/AnsiString.o: AnsiString.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/AnsiString.o AnsiString.cpp

${OBJECTDIR}/ThreadPool.o: ThreadPool.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/ThreadPool.o ThreadPool.cpp

${OBJECTDIR}/_ext/761759085/MTree.o: ../Migo/MTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/MTree.o ../Migo/MTree.cpp

${OBJECTDIR}/TDataset.o: TDataset.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TDataset.o TDataset.cpp

${OBJECTDIR}/Exception.o: Exception.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/Exception.o Exception.cpp

${OBJECTDIR}/MDS/MSchema.o: MDS/MSchema.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MSchema.o MDS/MSchema.cpp

${OBJECTDIR}/PSocket.o: PSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PSocket.o PSocket.cpp

${OBJECTDIR}/PClientSocket.o: PClientSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PClientSocket.o PClientSocket.cpp

${OBJECTDIR}/_ext/760415558/gzclose.o: ../zlib/gzclose.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760415558
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760415558/gzclose.o ../zlib/gzclose.c

${OBJECTDIR}/_ext/761759085/MSubscriber.o: ../Migo/MSubscriber.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/761759085
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/761759085/MSubscriber.o ../Migo/MSubscriber.cpp

${OBJECTDIR}/PServerSocket.o: PServerSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/PServerSocket.o PServerSocket.cpp

${OBJECTDIR}/MDS/MTransactionStore.o: MDS/MTransactionStore.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MTransactionStore.o MDS/MTransactionStore.cpp

${OBJECTDIR}/MultiplexingServer.o: MultiplexingServer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MultiplexingServer.o MultiplexingServer.cpp

${OBJECTDIR}/TFixedFormat.o: TFixedFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/TFixedFormat.o TFixedFormat.cpp

${OBJECTDIR}/FileStream.o: FileStream.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/FileStream.o FileStream.cpp

${OBJECTDIR}/MDS/MTable.o: MDS/MTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/MDS/MTable.o MDS/MTable.cpp

${OBJECTDIR}/BufferedLog.o: BufferedLog.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -D__AIX -MMD -MP -MF $@.d -o ${OBJECTDIR}/BufferedLog.o BufferedLog.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Debug
	${RM} ../bin/UFC.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
