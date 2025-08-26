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
CND_CONF=LINUX_Release_for_so
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile_UFC.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/d2987a93/MApp.o \
	${OBJECTDIR}/_ext/d2987a93/MComponent.o \
	${OBJECTDIR}/_ext/d2987a93/MNode.o \
	${OBJECTDIR}/_ext/d2987a93/MSubscriber.o \
	${OBJECTDIR}/_ext/d2987a93/MTree.o \
	${OBJECTDIR}/_ext/d2987a93/MigoHeader.o \
	${OBJECTDIR}/_ext/d2987a93/Sigo.o \
	${OBJECTDIR}/_ext/d2987a93/SubjectDB.o \
	${OBJECTDIR}/_ext/d2acfaba/adler32.o \
	${OBJECTDIR}/_ext/d2acfaba/compress.o \
	${OBJECTDIR}/_ext/d2acfaba/crc32.o \
	${OBJECTDIR}/_ext/d2acfaba/deflate.o \
	${OBJECTDIR}/_ext/d2acfaba/gzio.o \
	${OBJECTDIR}/_ext/d2acfaba/infback.o \
	${OBJECTDIR}/_ext/d2acfaba/inffast.o \
	${OBJECTDIR}/_ext/d2acfaba/inflate.o \
	${OBJECTDIR}/_ext/d2acfaba/inftrees.o \
	${OBJECTDIR}/_ext/d2acfaba/trees.o \
	${OBJECTDIR}/_ext/d2acfaba/uncompr.o \
	${OBJECTDIR}/_ext/d2acfaba/zutil.o \
	${OBJECTDIR}/AnsiString.o \
	${OBJECTDIR}/BufferedLog.o \
	${OBJECTDIR}/Exception.o \
	${OBJECTDIR}/FIFO.o \
	${OBJECTDIR}/FileStream.o \
	${OBJECTDIR}/List.o \
	${OBJECTDIR}/MD5.o \
	${OBJECTDIR}/MDS/MColumn.o \
	${OBJECTDIR}/MDS/MDSCommObj.o \
	${OBJECTDIR}/MDS/MDSException.o \
	${OBJECTDIR}/MDS/MIndex.o \
	${OBJECTDIR}/MDS/MRow.o \
	${OBJECTDIR}/MDS/MRowSet.o \
	${OBJECTDIR}/MDS/MSchema.o \
	${OBJECTDIR}/MDS/MTable.o \
	${OBJECTDIR}/MDS/MTransactionStore.o \
	${OBJECTDIR}/MemoryStream.o \
	${OBJECTDIR}/MultiplexingServer.o \
	${OBJECTDIR}/NameValueMessage.o \
	${OBJECTDIR}/NetUtility.o \
	${OBJECTDIR}/PClientSocket.o \
	${OBJECTDIR}/PInt32.o \
	${OBJECTDIR}/PQueue.o \
	${OBJECTDIR}/PServerSocket.o \
	${OBJECTDIR}/PSocket.o \
	${OBJECTDIR}/PThread.o \
	${OBJECTDIR}/PUDPClient.o \
	${OBJECTDIR}/Profiler.o \
	${OBJECTDIR}/SocketHeartBeat.o \
	${OBJECTDIR}/StringBuffer.o \
	${OBJECTDIR}/TDataFormat.o \
	${OBJECTDIR}/TDataset.o \
	${OBJECTDIR}/TField.o \
	${OBJECTDIR}/TFixedFormat.o \
	${OBJECTDIR}/TLicenseKey.o \
	${OBJECTDIR}/TParseData.o \
	${OBJECTDIR}/TRecord.o \
	${OBJECTDIR}/TRenderData.o \
	${OBJECTDIR}/ThreadPool.o \
	${OBJECTDIR}/UDateTime.o \
	${OBJECTDIR}/UIPC.o \
	${OBJECTDIR}/Utility.o \
	${OBJECTDIR}/iniFile.o \
	${OBJECTDIR}/zlibutility.o


# C Compiler Flags
CFLAGS=-m32 -pthread -fPIC

# CC Compiler Flags
CCFLAGS=-m32 -pthread -fPIC -Wno-deprecated
CXXFLAGS=-m32 -pthread -fPIC -Wno-deprecated

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=--32

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /MBus/lib/libUFC_dl.a

/MBus/lib/libUFC_dl.a: ${OBJECTFILES}
	${MKDIR} -p /MBus/lib
	${RM} /MBus/lib/libUFC_dl.a
	${AR} -rv /MBus/lib/libUFC_dl.a ${OBJECTFILES} 
	$(RANLIB) /MBus/lib/libUFC_dl.a

${OBJECTDIR}/_ext/d2987a93/MApp.o: ../Migo/MApp.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/MApp.o ../Migo/MApp.cpp

${OBJECTDIR}/_ext/d2987a93/MComponent.o: ../Migo/MComponent.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/MComponent.o ../Migo/MComponent.cpp

${OBJECTDIR}/_ext/d2987a93/MNode.o: ../Migo/MNode.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/MNode.o ../Migo/MNode.cpp

${OBJECTDIR}/_ext/d2987a93/MSubscriber.o: ../Migo/MSubscriber.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/MSubscriber.o ../Migo/MSubscriber.cpp

${OBJECTDIR}/_ext/d2987a93/MTree.o: ../Migo/MTree.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/MTree.o ../Migo/MTree.cpp

${OBJECTDIR}/_ext/d2987a93/MigoHeader.o: ../Migo/MigoHeader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/MigoHeader.o ../Migo/MigoHeader.cpp

${OBJECTDIR}/_ext/d2987a93/Sigo.o: ../Migo/Sigo.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/Sigo.o ../Migo/Sigo.cpp

${OBJECTDIR}/_ext/d2987a93/SubjectDB.o: ../Migo/SubjectDB.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d2987a93
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2987a93/SubjectDB.o ../Migo/SubjectDB.cpp

${OBJECTDIR}/_ext/d2acfaba/adler32.o: ../zlib/adler32.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/adler32.o ../zlib/adler32.c

${OBJECTDIR}/_ext/d2acfaba/compress.o: ../zlib/compress.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/compress.o ../zlib/compress.c

${OBJECTDIR}/_ext/d2acfaba/crc32.o: ../zlib/crc32.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/crc32.o ../zlib/crc32.c

${OBJECTDIR}/_ext/d2acfaba/deflate.o: ../zlib/deflate.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/deflate.o ../zlib/deflate.c

${OBJECTDIR}/_ext/d2acfaba/gzio.o: ../zlib/gzio.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/gzio.o ../zlib/gzio.c

${OBJECTDIR}/_ext/d2acfaba/infback.o: ../zlib/infback.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/infback.o ../zlib/infback.c

${OBJECTDIR}/_ext/d2acfaba/inffast.o: ../zlib/inffast.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/inffast.o ../zlib/inffast.c

${OBJECTDIR}/_ext/d2acfaba/inflate.o: ../zlib/inflate.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/inflate.o ../zlib/inflate.c

${OBJECTDIR}/_ext/d2acfaba/inftrees.o: ../zlib/inftrees.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/inftrees.o ../zlib/inftrees.c

${OBJECTDIR}/_ext/d2acfaba/trees.o: ../zlib/trees.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/trees.o ../zlib/trees.c

${OBJECTDIR}/_ext/d2acfaba/uncompr.o: ../zlib/uncompr.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/uncompr.o ../zlib/uncompr.c

${OBJECTDIR}/_ext/d2acfaba/zutil.o: ../zlib/zutil.c
	${MKDIR} -p ${OBJECTDIR}/_ext/d2acfaba
	${RM} "$@.d"
	$(COMPILE.c) -O3 -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d2acfaba/zutil.o ../zlib/zutil.c

${OBJECTDIR}/AnsiString.o: AnsiString.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AnsiString.o AnsiString.cpp

${OBJECTDIR}/BufferedLog.o: BufferedLog.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BufferedLog.o BufferedLog.cpp

${OBJECTDIR}/Exception.o: Exception.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Exception.o Exception.cpp

${OBJECTDIR}/FIFO.o: FIFO.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FIFO.o FIFO.cpp

${OBJECTDIR}/FileStream.o: FileStream.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FileStream.o FileStream.cpp

${OBJECTDIR}/List.o: List.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/List.o List.cpp

${OBJECTDIR}/MD5.o: MD5.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MD5.o MD5.cpp

${OBJECTDIR}/MDS/MColumn.o: MDS/MColumn.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MColumn.o MDS/MColumn.cpp

${OBJECTDIR}/MDS/MDSCommObj.o: MDS/MDSCommObj.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MDSCommObj.o MDS/MDSCommObj.cpp

${OBJECTDIR}/MDS/MDSException.o: MDS/MDSException.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MDSException.o MDS/MDSException.cpp

${OBJECTDIR}/MDS/MIndex.o: MDS/MIndex.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MIndex.o MDS/MIndex.cpp

${OBJECTDIR}/MDS/MRow.o: MDS/MRow.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MRow.o MDS/MRow.cpp

${OBJECTDIR}/MDS/MRowSet.o: MDS/MRowSet.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MRowSet.o MDS/MRowSet.cpp

${OBJECTDIR}/MDS/MSchema.o: MDS/MSchema.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MSchema.o MDS/MSchema.cpp

${OBJECTDIR}/MDS/MTable.o: MDS/MTable.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MTable.o MDS/MTable.cpp

${OBJECTDIR}/MDS/MTransactionStore.o: MDS/MTransactionStore.cpp
	${MKDIR} -p ${OBJECTDIR}/MDS
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MDS/MTransactionStore.o MDS/MTransactionStore.cpp

${OBJECTDIR}/MemoryStream.o: MemoryStream.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MemoryStream.o MemoryStream.cpp

${OBJECTDIR}/MultiplexingServer.o: MultiplexingServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MultiplexingServer.o MultiplexingServer.cpp

${OBJECTDIR}/NameValueMessage.o: NameValueMessage.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NameValueMessage.o NameValueMessage.cpp

${OBJECTDIR}/NetUtility.o: NetUtility.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NetUtility.o NetUtility.cpp

${OBJECTDIR}/PClientSocket.o: PClientSocket.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PClientSocket.o PClientSocket.cpp

${OBJECTDIR}/PInt32.o: PInt32.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PInt32.o PInt32.cpp

${OBJECTDIR}/PQueue.o: PQueue.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PQueue.o PQueue.cpp

${OBJECTDIR}/PServerSocket.o: PServerSocket.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PServerSocket.o PServerSocket.cpp

${OBJECTDIR}/PSocket.o: PSocket.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PSocket.o PSocket.cpp

${OBJECTDIR}/PThread.o: PThread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PThread.o PThread.cpp

${OBJECTDIR}/PUDPClient.o: PUDPClient.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PUDPClient.o PUDPClient.cpp

${OBJECTDIR}/Profiler.o: Profiler.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Profiler.o Profiler.cpp

${OBJECTDIR}/SocketHeartBeat.o: SocketHeartBeat.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SocketHeartBeat.o SocketHeartBeat.cpp

${OBJECTDIR}/StringBuffer.o: StringBuffer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StringBuffer.o StringBuffer.cpp

${OBJECTDIR}/TDataFormat.o: TDataFormat.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TDataFormat.o TDataFormat.cpp

${OBJECTDIR}/TDataset.o: TDataset.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TDataset.o TDataset.cpp

${OBJECTDIR}/TField.o: TField.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TField.o TField.cpp

${OBJECTDIR}/TFixedFormat.o: TFixedFormat.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TFixedFormat.o TFixedFormat.cpp

${OBJECTDIR}/TLicenseKey.o: TLicenseKey.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TLicenseKey.o TLicenseKey.cpp

${OBJECTDIR}/TParseData.o: TParseData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TParseData.o TParseData.cpp

${OBJECTDIR}/TRecord.o: TRecord.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TRecord.o TRecord.cpp

${OBJECTDIR}/TRenderData.o: TRenderData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TRenderData.o TRenderData.cpp

${OBJECTDIR}/ThreadPool.o: ThreadPool.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ThreadPool.o ThreadPool.cpp

${OBJECTDIR}/UDateTime.o: UDateTime.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UDateTime.o UDateTime.cpp

${OBJECTDIR}/UIPC.o: UIPC.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UIPC.o UIPC.cpp

${OBJECTDIR}/Utility.o: Utility.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utility.o Utility.cpp

${OBJECTDIR}/iniFile.o: iniFile.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/iniFile.o iniFile.cpp

${OBJECTDIR}/zlibutility.o: zlibutility.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -D__LINUX -D__SO -I. -IMDS -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/zlibutility.o zlibutility.cpp

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
