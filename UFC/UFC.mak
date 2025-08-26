# C++BuilderX Version: 1.0.1.103
# Generated GNU Makefile
# Do not modify, as your changes will be lost on re-export

# User Defined Variables:
# End User Defined Variables

# Start of configurations
# If the user specified no configuration on the command line, set a default:
ifndef CFG
  CFG := AIX Debug Build
endif
ifeq ($(CFG),Debug Build)
  BUILD_DIR := ../../../../../../MBus/lib
  Debug Build := 1
endif
ifeq ($(CFG),Release Build)
  BUILD_DIR := ../../../../../../MBus/lib
  Release Build := 1
endif
ifeq ($(CFG),AIX Debug Build)
  BUILD_DIR := ../../../../../../MBus/lib
  AIX Debug Build := 1
endif
ifneq ($(CFG),Debug Build)
  ifneq ($(CFG),Release Build)
    ifneq ($(CFG),AIX Debug Build)
      $(error An incorrect configuration was specified)
    endif
  endif
endif
# End of configurations

ifeq ($(CFG),Debug Build)
  # Default build target if none specified:
default: builddir ../../../../../../MBus/lib/UFC.a

all: default

../../../../../../MBus/lib/PServerSocket.o: PServerSocket.cpp
	g++ -c -o $(BUILD_DIR)/PServerSocket.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PServerSocket.cpp

../../../../../../MBus/lib/PClientSocket.o: PClientSocket.cpp
	g++ -c -o $(BUILD_DIR)/PClientSocket.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PClientSocket.cpp

../../../../../../MBus/lib/NetUtility.o: NetUtility.cpp
	g++ -c -o $(BUILD_DIR)/NetUtility.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads NetUtility.cpp

../../../../../../MBus/lib/MultiplexingServer.o: MultiplexingServer.cpp
	g++ -c -o $(BUILD_DIR)/MultiplexingServer.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads MultiplexingServer.cpp

../../../../../../MBus/lib/PSocket.o: PSocket.cpp
	g++ -c -o $(BUILD_DIR)/PSocket.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PSocket.cpp

../../../../../../MBus/lib/PUDPClient.o: PUDPClient.cpp
	g++ -c -o $(BUILD_DIR)/PUDPClient.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PUDPClient.cpp

../../../../../../MBus/lib/SocketHeartBeat.o: SocketHeartBeat.cpp
	g++ -c -o $(BUILD_DIR)/SocketHeartBeat.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads SocketHeartBeat.cpp

../../../../../../MBus/lib/StringBuffer.o: StringBuffer.cpp
	g++ -c -o $(BUILD_DIR)/StringBuffer.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads StringBuffer.cpp

../../../../../../MBus/lib/AnsiString.o: AnsiString.cpp
	g++ -c -o $(BUILD_DIR)/AnsiString.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads AnsiString.cpp

../../../../../../MBus/lib/FileStream.o: FileStream.cpp
	g++ -c -o $(BUILD_DIR)/FileStream.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads FileStream.cpp

../../../../../../MBus/lib/MemoryStream.o: MemoryStream.cpp
	g++ -c -o $(BUILD_DIR)/MemoryStream.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads MemoryStream.cpp

../../../../../../MBus/lib/PThread.o: PThread.cpp
	g++ -c -o $(BUILD_DIR)/PThread.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PThread.cpp

../../../../../../MBus/lib/List.o: List.cpp
	g++ -c -o $(BUILD_DIR)/List.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads List.cpp

../../../../../../MBus/lib/Exception.o: Exception.cpp
	g++ -c -o $(BUILD_DIR)/Exception.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads Exception.cpp

../../../../../../MBus/lib/ThreadPool.o: ThreadPool.cpp
	g++ -c -o $(BUILD_DIR)/ThreadPool.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads ThreadPool.cpp

../../../../../../MBus/lib/iniFile.o: iniFile.cpp
	g++ -c -o $(BUILD_DIR)/iniFile.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads iniFile.cpp

../../../../../../MBus/lib/MD5.o: MD5.cpp
	g++ -c -o $(BUILD_DIR)/MD5.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads MD5.cpp

../../../../../../MBus/lib/PInt32.o: PInt32.cpp
	g++ -c -o $(BUILD_DIR)/PInt32.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PInt32.cpp

../../../../../../MBus/lib/PQueue.o: PQueue.cpp
	g++ -c -o $(BUILD_DIR)/PQueue.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads PQueue.cpp

../../../../../../MBus/lib/TField.o: TField.cpp
	g++ -c -o $(BUILD_DIR)/TField.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TField.cpp

../../../../../../MBus/lib/TRecord.o: TRecord.cpp
	g++ -c -o $(BUILD_DIR)/TRecord.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TRecord.cpp

../../../../../../MBus/lib/TDataset.o: TDataset.cpp
	g++ -c -o $(BUILD_DIR)/TDataset.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TDataset.cpp

../../../../../../MBus/lib/TDataFormat.o: TDataFormat.cpp
	g++ -c -o $(BUILD_DIR)/TDataFormat.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TDataFormat.cpp

../../../../../../MBus/lib/TFixedFormat.o: TFixedFormat.cpp
	g++ -c -o $(BUILD_DIR)/TFixedFormat.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TFixedFormat.cpp

../../../../../../MBus/lib/TParseData.o: TParseData.cpp
	g++ -c -o $(BUILD_DIR)/TParseData.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TParseData.cpp

../../../../../../MBus/lib/TRenderData.o: TRenderData.cpp
	g++ -c -o $(BUILD_DIR)/TRenderData.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads TRenderData.cpp

../../../../../../MBus/lib/UDateTime.o: UDateTime.cpp
	g++ -c -o $(BUILD_DIR)/UDateTime.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads UDateTime.cpp

../../../../../../MBus/lib/UException.o: UException.cpp
	g++ -c -o $(BUILD_DIR)/UException.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads UException.cpp

../../../../../../MBus/lib/UIPC.o: UIPC.cpp
	g++ -c -o $(BUILD_DIR)/UIPC.o -Wall -Wno-long-long -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads UIPC.cpp

../../../../../../MBus/lib/UFC.a: ../../../../../../MBus/lib/PServerSocket.o ../../../../../../MBus/lib/PClientSocket.o \
         ../../../../../../MBus/lib/NetUtility.o ../../../../../../MBus/lib/MultiplexingServer.o \
         ../../../../../../MBus/lib/PSocket.o ../../../../../../MBus/lib/PUDPClient.o \
         ../../../../../../MBus/lib/SocketHeartBeat.o ../../../../../../MBus/lib/StringBuffer.o \
         ../../../../../../MBus/lib/AnsiString.o ../../../../../../MBus/lib/FileStream.o \
         ../../../../../../MBus/lib/MemoryStream.o ../../../../../../MBus/lib/PThread.o \
         ../../../../../../MBus/lib/List.o ../../../../../../MBus/lib/Exception.o \
         ../../../../../../MBus/lib/ThreadPool.o ../../../../../../MBus/lib/iniFile.o \
         ../../../../../../MBus/lib/MD5.o ../../../../../../MBus/lib/PInt32.o ../../../../../../MBus/lib/PQueue.o \
         ../../../../../../MBus/lib/TField.o ../../../../../../MBus/lib/TRecord.o \
         ../../../../../../MBus/lib/TDataset.o ../../../../../../MBus/lib/TDataFormat.o \
         ../../../../../../MBus/lib/TFixedFormat.o ../../../../../../MBus/lib/TParseData.o \
         ../../../../../../MBus/lib/TRenderData.o ../../../../../../MBus/lib/UDateTime.o \
         ../../../../../../MBus/lib/UException.o ../../../../../../MBus/lib/UIPC.o \
        
	ar q $(BUILD_DIR)/UFC.a ../../../../../../MBus/lib/PServerSocket.o ../../../../../../MBus/lib/PClientSocket.o ../../../../../../MBus/lib/NetUtility.o ../../../../../../MBus/lib/MultiplexingServer.o ../../../../../../MBus/lib/PSocket.o ../../../../../../MBus/lib/PUDPClient.o ../../../../../../MBus/lib/SocketHeartBeat.o ../../../../../../MBus/lib/StringBuffer.o ../../../../../../MBus/lib/AnsiString.o ../../../../../../MBus/lib/FileStream.o ../../../../../../MBus/lib/MemoryStream.o ../../../../../../MBus/lib/PThread.o ../../../../../../MBus/lib/List.o ../../../../../../MBus/lib/Exception.o ../../../../../../MBus/lib/ThreadPool.o ../../../../../../MBus/lib/iniFile.o ../../../../../../MBus/lib/MD5.o ../../../../../../MBus/lib/PInt32.o ../../../../../../MBus/lib/PQueue.o ../../../../../../MBus/lib/TField.o ../../../../../../MBus/lib/TRecord.o ../../../../../../MBus/lib/TDataset.o ../../../../../../MBus/lib/TDataFormat.o ../../../../../../MBus/lib/TFixedFormat.o ../../../../../../MBus/lib/TParseData.o ../../../../../../MBus/lib/TRenderData.o ../../../../../../MBus/lib/UDateTime.o ../../../../../../MBus/lib/UException.o ../../../../../../MBus/lib/UIPC.o

builddir:
	-mkdir -p ..
	-mkdir -p ../MBus
	-mkdir -p ../MBus/lib

clean:
	-rm ../../../../../../MBus/lib/PServerSocket.o
	-rm ../../../../../../MBus/lib/PServerSocket.d
	-rm ../../../../../../MBus/lib/PClientSocket.o
	-rm ../../../../../../MBus/lib/PClientSocket.d
	-rm ../../../../../../MBus/lib/NetUtility.o
	-rm ../../../../../../MBus/lib/NetUtility.d
	-rm ../../../../../../MBus/lib/MultiplexingServer.o
	-rm ../../../../../../MBus/lib/MultiplexingServer.d
	-rm ../../../../../../MBus/lib/PSocket.o
	-rm ../../../../../../MBus/lib/PSocket.d
	-rm ../../../../../../MBus/lib/PUDPClient.o
	-rm ../../../../../../MBus/lib/PUDPClient.d
	-rm ../../../../../../MBus/lib/SocketHeartBeat.o
	-rm ../../../../../../MBus/lib/SocketHeartBeat.d
	-rm ../../../../../../MBus/lib/StringBuffer.o
	-rm ../../../../../../MBus/lib/StringBuffer.d
	-rm ../../../../../../MBus/lib/AnsiString.o
	-rm ../../../../../../MBus/lib/AnsiString.d
	-rm ../../../../../../MBus/lib/FileStream.o
	-rm ../../../../../../MBus/lib/FileStream.d
	-rm ../../../../../../MBus/lib/MemoryStream.o
	-rm ../../../../../../MBus/lib/MemoryStream.d
	-rm ../../../../../../MBus/lib/PThread.o
	-rm ../../../../../../MBus/lib/PThread.d
	-rm ../../../../../../MBus/lib/List.o
	-rm ../../../../../../MBus/lib/List.d
	-rm ../../../../../../MBus/lib/Exception.o
	-rm ../../../../../../MBus/lib/Exception.d
	-rm ../../../../../../MBus/lib/ThreadPool.o
	-rm ../../../../../../MBus/lib/ThreadPool.d
	-rm ../../../../../../MBus/lib/iniFile.o
	-rm ../../../../../../MBus/lib/iniFile.d
	-rm ../../../../../../MBus/lib/MD5.o
	-rm ../../../../../../MBus/lib/MD5.d
	-rm ../../../../../../MBus/lib/PInt32.o
	-rm ../../../../../../MBus/lib/PInt32.d
	-rm ../../../../../../MBus/lib/PQueue.o
	-rm ../../../../../../MBus/lib/PQueue.d
	-rm ../../../../../../MBus/lib/TField.o
	-rm ../../../../../../MBus/lib/TField.d
	-rm ../../../../../../MBus/lib/TRecord.o
	-rm ../../../../../../MBus/lib/TRecord.d
	-rm ../../../../../../MBus/lib/TDataset.o
	-rm ../../../../../../MBus/lib/TDataset.d
	-rm ../../../../../../MBus/lib/TDataFormat.o
	-rm ../../../../../../MBus/lib/TDataFormat.d
	-rm ../../../../../../MBus/lib/TFixedFormat.o
	-rm ../../../../../../MBus/lib/TFixedFormat.d
	-rm ../../../../../../MBus/lib/TParseData.o
	-rm ../../../../../../MBus/lib/TParseData.d
	-rm ../../../../../../MBus/lib/TRenderData.o
	-rm ../../../../../../MBus/lib/TRenderData.d
	-rm ../../../../../../MBus/lib/UDateTime.o
	-rm ../../../../../../MBus/lib/UDateTime.d
	-rm ../../../../../../MBus/lib/UException.o
	-rm ../../../../../../MBus/lib/UException.d
	-rm ../../../../../../MBus/lib/UIPC.o
	-rm ../../../../../../MBus/lib/UIPC.d
	-rm ../../../../../../MBus/lib/UFC.a

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../MBus/lib/UFC.a

all: default

../../../../../../MBus/lib/PServerSocket.o: PServerSocket.cpp
	g++ -c -o $(BUILD_DIR)/PServerSocket.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PServerSocket.cpp

../../../../../../MBus/lib/PClientSocket.o: PClientSocket.cpp
	g++ -c -o $(BUILD_DIR)/PClientSocket.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PClientSocket.cpp

../../../../../../MBus/lib/NetUtility.o: NetUtility.cpp
	g++ -c -o $(BUILD_DIR)/NetUtility.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads NetUtility.cpp

../../../../../../MBus/lib/MultiplexingServer.o: MultiplexingServer.cpp
	g++ -c -o $(BUILD_DIR)/MultiplexingServer.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads MultiplexingServer.cpp

../../../../../../MBus/lib/PSocket.o: PSocket.cpp
	g++ -c -o $(BUILD_DIR)/PSocket.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PSocket.cpp

../../../../../../MBus/lib/PUDPClient.o: PUDPClient.cpp
	g++ -c -o $(BUILD_DIR)/PUDPClient.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PUDPClient.cpp

../../../../../../MBus/lib/SocketHeartBeat.o: SocketHeartBeat.cpp
	g++ -c -o $(BUILD_DIR)/SocketHeartBeat.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads SocketHeartBeat.cpp

../../../../../../MBus/lib/StringBuffer.o: StringBuffer.cpp
	g++ -c -o $(BUILD_DIR)/StringBuffer.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads StringBuffer.cpp

../../../../../../MBus/lib/AnsiString.o: AnsiString.cpp
	g++ -c -o $(BUILD_DIR)/AnsiString.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads AnsiString.cpp

../../../../../../MBus/lib/FileStream.o: FileStream.cpp
	g++ -c -o $(BUILD_DIR)/FileStream.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads FileStream.cpp

../../../../../../MBus/lib/MemoryStream.o: MemoryStream.cpp
	g++ -c -o $(BUILD_DIR)/MemoryStream.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads MemoryStream.cpp

../../../../../../MBus/lib/PThread.o: PThread.cpp
	g++ -c -o $(BUILD_DIR)/PThread.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PThread.cpp

../../../../../../MBus/lib/List.o: List.cpp
	g++ -c -o $(BUILD_DIR)/List.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads List.cpp

../../../../../../MBus/lib/Exception.o: Exception.cpp
	g++ -c -o $(BUILD_DIR)/Exception.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads Exception.cpp

../../../../../../MBus/lib/ThreadPool.o: ThreadPool.cpp
	g++ -c -o $(BUILD_DIR)/ThreadPool.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads ThreadPool.cpp

../../../../../../MBus/lib/iniFile.o: iniFile.cpp
	g++ -c -o $(BUILD_DIR)/iniFile.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads iniFile.cpp

../../../../../../MBus/lib/MD5.o: MD5.cpp
	g++ -c -o $(BUILD_DIR)/MD5.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads MD5.cpp

../../../../../../MBus/lib/PInt32.o: PInt32.cpp
	g++ -c -o $(BUILD_DIR)/PInt32.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PInt32.cpp

../../../../../../MBus/lib/PQueue.o: PQueue.cpp
	g++ -c -o $(BUILD_DIR)/PQueue.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads PQueue.cpp

../../../../../../MBus/lib/TField.o: TField.cpp
	g++ -c -o $(BUILD_DIR)/TField.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TField.cpp

../../../../../../MBus/lib/TRecord.o: TRecord.cpp
	g++ -c -o $(BUILD_DIR)/TRecord.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TRecord.cpp

../../../../../../MBus/lib/TDataset.o: TDataset.cpp
	g++ -c -o $(BUILD_DIR)/TDataset.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TDataset.cpp

../../../../../../MBus/lib/TDataFormat.o: TDataFormat.cpp
	g++ -c -o $(BUILD_DIR)/TDataFormat.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TDataFormat.cpp

../../../../../../MBus/lib/TFixedFormat.o: TFixedFormat.cpp
	g++ -c -o $(BUILD_DIR)/TFixedFormat.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TFixedFormat.cpp

../../../../../../MBus/lib/TParseData.o: TParseData.cpp
	g++ -c -o $(BUILD_DIR)/TParseData.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TParseData.cpp

../../../../../../MBus/lib/TRenderData.o: TRenderData.cpp
	g++ -c -o $(BUILD_DIR)/TRenderData.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads TRenderData.cpp

../../../../../../MBus/lib/UDateTime.o: UDateTime.cpp
	g++ -c -o $(BUILD_DIR)/UDateTime.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads UDateTime.cpp

../../../../../../MBus/lib/UException.o: UException.cpp
	g++ -c -o $(BUILD_DIR)/UException.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads UException.cpp

../../../../../../MBus/lib/UIPC.o: UIPC.cpp
	g++ -c -o $(BUILD_DIR)/UIPC.o -Wall -D _SOLARIS -MD -I/usr/include -pthreads UIPC.cpp

../../../../../../MBus/lib/UFC.a: ../../../../../../MBus/lib/PServerSocket.o ../../../../../../MBus/lib/PClientSocket.o \
         ../../../../../../MBus/lib/NetUtility.o ../../../../../../MBus/lib/MultiplexingServer.o \
         ../../../../../../MBus/lib/PSocket.o ../../../../../../MBus/lib/PUDPClient.o \
         ../../../../../../MBus/lib/SocketHeartBeat.o ../../../../../../MBus/lib/StringBuffer.o \
         ../../../../../../MBus/lib/AnsiString.o ../../../../../../MBus/lib/FileStream.o \
         ../../../../../../MBus/lib/MemoryStream.o ../../../../../../MBus/lib/PThread.o \
         ../../../../../../MBus/lib/List.o ../../../../../../MBus/lib/Exception.o \
         ../../../../../../MBus/lib/ThreadPool.o ../../../../../../MBus/lib/iniFile.o \
         ../../../../../../MBus/lib/MD5.o ../../../../../../MBus/lib/PInt32.o ../../../../../../MBus/lib/PQueue.o \
         ../../../../../../MBus/lib/TField.o ../../../../../../MBus/lib/TRecord.o \
         ../../../../../../MBus/lib/TDataset.o ../../../../../../MBus/lib/TDataFormat.o \
         ../../../../../../MBus/lib/TFixedFormat.o ../../../../../../MBus/lib/TParseData.o \
         ../../../../../../MBus/lib/TRenderData.o ../../../../../../MBus/lib/UDateTime.o \
         ../../../../../../MBus/lib/UException.o ../../../../../../MBus/lib/UIPC.o \
        
	ar q $(BUILD_DIR)/UFC.a ../../../../../../MBus/lib/PServerSocket.o ../../../../../../MBus/lib/PClientSocket.o ../../../../../../MBus/lib/NetUtility.o ../../../../../../MBus/lib/MultiplexingServer.o ../../../../../../MBus/lib/PSocket.o ../../../../../../MBus/lib/PUDPClient.o ../../../../../../MBus/lib/SocketHeartBeat.o ../../../../../../MBus/lib/StringBuffer.o ../../../../../../MBus/lib/AnsiString.o ../../../../../../MBus/lib/FileStream.o ../../../../../../MBus/lib/MemoryStream.o ../../../../../../MBus/lib/PThread.o ../../../../../../MBus/lib/List.o ../../../../../../MBus/lib/Exception.o ../../../../../../MBus/lib/ThreadPool.o ../../../../../../MBus/lib/iniFile.o ../../../../../../MBus/lib/MD5.o ../../../../../../MBus/lib/PInt32.o ../../../../../../MBus/lib/PQueue.o ../../../../../../MBus/lib/TField.o ../../../../../../MBus/lib/TRecord.o ../../../../../../MBus/lib/TDataset.o ../../../../../../MBus/lib/TDataFormat.o ../../../../../../MBus/lib/TFixedFormat.o ../../../../../../MBus/lib/TParseData.o ../../../../../../MBus/lib/TRenderData.o ../../../../../../MBus/lib/UDateTime.o ../../../../../../MBus/lib/UException.o ../../../../../../MBus/lib/UIPC.o

builddir:
	-mkdir -p ..
	-mkdir -p ../MBus
	-mkdir -p ../MBus/lib

clean:
	-rm ../../../../../../MBus/lib/PServerSocket.o
	-rm ../../../../../../MBus/lib/PServerSocket.d
	-rm ../../../../../../MBus/lib/PClientSocket.o
	-rm ../../../../../../MBus/lib/PClientSocket.d
	-rm ../../../../../../MBus/lib/NetUtility.o
	-rm ../../../../../../MBus/lib/NetUtility.d
	-rm ../../../../../../MBus/lib/MultiplexingServer.o
	-rm ../../../../../../MBus/lib/MultiplexingServer.d
	-rm ../../../../../../MBus/lib/PSocket.o
	-rm ../../../../../../MBus/lib/PSocket.d
	-rm ../../../../../../MBus/lib/PUDPClient.o
	-rm ../../../../../../MBus/lib/PUDPClient.d
	-rm ../../../../../../MBus/lib/SocketHeartBeat.o
	-rm ../../../../../../MBus/lib/SocketHeartBeat.d
	-rm ../../../../../../MBus/lib/StringBuffer.o
	-rm ../../../../../../MBus/lib/StringBuffer.d
	-rm ../../../../../../MBus/lib/AnsiString.o
	-rm ../../../../../../MBus/lib/AnsiString.d
	-rm ../../../../../../MBus/lib/FileStream.o
	-rm ../../../../../../MBus/lib/FileStream.d
	-rm ../../../../../../MBus/lib/MemoryStream.o
	-rm ../../../../../../MBus/lib/MemoryStream.d
	-rm ../../../../../../MBus/lib/PThread.o
	-rm ../../../../../../MBus/lib/PThread.d
	-rm ../../../../../../MBus/lib/List.o
	-rm ../../../../../../MBus/lib/List.d
	-rm ../../../../../../MBus/lib/Exception.o
	-rm ../../../../../../MBus/lib/Exception.d
	-rm ../../../../../../MBus/lib/ThreadPool.o
	-rm ../../../../../../MBus/lib/ThreadPool.d
	-rm ../../../../../../MBus/lib/iniFile.o
	-rm ../../../../../../MBus/lib/iniFile.d
	-rm ../../../../../../MBus/lib/MD5.o
	-rm ../../../../../../MBus/lib/MD5.d
	-rm ../../../../../../MBus/lib/PInt32.o
	-rm ../../../../../../MBus/lib/PInt32.d
	-rm ../../../../../../MBus/lib/PQueue.o
	-rm ../../../../../../MBus/lib/PQueue.d
	-rm ../../../../../../MBus/lib/TField.o
	-rm ../../../../../../MBus/lib/TField.d
	-rm ../../../../../../MBus/lib/TRecord.o
	-rm ../../../../../../MBus/lib/TRecord.d
	-rm ../../../../../../MBus/lib/TDataset.o
	-rm ../../../../../../MBus/lib/TDataset.d
	-rm ../../../../../../MBus/lib/TDataFormat.o
	-rm ../../../../../../MBus/lib/TDataFormat.d
	-rm ../../../../../../MBus/lib/TFixedFormat.o
	-rm ../../../../../../MBus/lib/TFixedFormat.d
	-rm ../../../../../../MBus/lib/TParseData.o
	-rm ../../../../../../MBus/lib/TParseData.d
	-rm ../../../../../../MBus/lib/TRenderData.o
	-rm ../../../../../../MBus/lib/TRenderData.d
	-rm ../../../../../../MBus/lib/UDateTime.o
	-rm ../../../../../../MBus/lib/UDateTime.d
	-rm ../../../../../../MBus/lib/UException.o
	-rm ../../../../../../MBus/lib/UException.d
	-rm ../../../../../../MBus/lib/UIPC.o
	-rm ../../../../../../MBus/lib/UIPC.d
	-rm ../../../../../../MBus/lib/UFC.a

endif
ifeq ($(CFG),AIX Debug Build)
  # Default build target if none specified:
default: builddir ../../../../../../MBus/lib/UFC.a

all: default

../../../../../../MBus/lib/PServerSocket.o: PServerSocket.cpp
	g++ -c -o $(BUILD_DIR)/PServerSocket.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PServerSocket.cpp

../../../../../../MBus/lib/PClientSocket.o: PClientSocket.cpp
	g++ -c -o $(BUILD_DIR)/PClientSocket.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PClientSocket.cpp

../../../../../../MBus/lib/NetUtility.o: NetUtility.cpp
	g++ -c -o $(BUILD_DIR)/NetUtility.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread NetUtility.cpp

../../../../../../MBus/lib/MultiplexingServer.o: MultiplexingServer.cpp
	g++ -c -o $(BUILD_DIR)/MultiplexingServer.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread MultiplexingServer.cpp

../../../../../../MBus/lib/PSocket.o: PSocket.cpp
	g++ -c -o $(BUILD_DIR)/PSocket.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PSocket.cpp

../../../../../../MBus/lib/PUDPClient.o: PUDPClient.cpp
	g++ -c -o $(BUILD_DIR)/PUDPClient.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PUDPClient.cpp

../../../../../../MBus/lib/SocketHeartBeat.o: SocketHeartBeat.cpp
	g++ -c -o $(BUILD_DIR)/SocketHeartBeat.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread SocketHeartBeat.cpp

../../../../../../MBus/lib/StringBuffer.o: StringBuffer.cpp
	g++ -c -o $(BUILD_DIR)/StringBuffer.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread StringBuffer.cpp

../../../../../../MBus/lib/AnsiString.o: AnsiString.cpp
	g++ -c -o $(BUILD_DIR)/AnsiString.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread AnsiString.cpp

../../../../../../MBus/lib/FileStream.o: FileStream.cpp
	g++ -c -o $(BUILD_DIR)/FileStream.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread FileStream.cpp

../../../../../../MBus/lib/MemoryStream.o: MemoryStream.cpp
	g++ -c -o $(BUILD_DIR)/MemoryStream.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread MemoryStream.cpp

../../../../../../MBus/lib/PThread.o: PThread.cpp
	g++ -c -o $(BUILD_DIR)/PThread.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PThread.cpp

../../../../../../MBus/lib/List.o: List.cpp
	g++ -c -o $(BUILD_DIR)/List.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread List.cpp

../../../../../../MBus/lib/Exception.o: Exception.cpp
	g++ -c -o $(BUILD_DIR)/Exception.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread Exception.cpp

../../../../../../MBus/lib/ThreadPool.o: ThreadPool.cpp
	g++ -c -o $(BUILD_DIR)/ThreadPool.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread ThreadPool.cpp

../../../../../../MBus/lib/iniFile.o: iniFile.cpp
	g++ -c -o $(BUILD_DIR)/iniFile.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread iniFile.cpp

../../../../../../MBus/lib/MD5.o: MD5.cpp
	g++ -c -o $(BUILD_DIR)/MD5.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread MD5.cpp

../../../../../../MBus/lib/PInt32.o: PInt32.cpp
	g++ -c -o $(BUILD_DIR)/PInt32.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PInt32.cpp

../../../../../../MBus/lib/PQueue.o: PQueue.cpp
	g++ -c -o $(BUILD_DIR)/PQueue.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread PQueue.cpp

../../../../../../MBus/lib/TField.o: TField.cpp
	g++ -c -o $(BUILD_DIR)/TField.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TField.cpp

../../../../../../MBus/lib/TRecord.o: TRecord.cpp
	g++ -c -o $(BUILD_DIR)/TRecord.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TRecord.cpp

../../../../../../MBus/lib/TDataset.o: TDataset.cpp
	g++ -c -o $(BUILD_DIR)/TDataset.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TDataset.cpp

../../../../../../MBus/lib/TDataFormat.o: TDataFormat.cpp
	g++ -c -o $(BUILD_DIR)/TDataFormat.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TDataFormat.cpp

../../../../../../MBus/lib/TFixedFormat.o: TFixedFormat.cpp
	g++ -c -o $(BUILD_DIR)/TFixedFormat.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TFixedFormat.cpp

../../../../../../MBus/lib/TParseData.o: TParseData.cpp
	g++ -c -o $(BUILD_DIR)/TParseData.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TParseData.cpp

../../../../../../MBus/lib/TRenderData.o: TRenderData.cpp
	g++ -c -o $(BUILD_DIR)/TRenderData.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread TRenderData.cpp

../../../../../../MBus/lib/UDateTime.o: UDateTime.cpp
	g++ -c -o $(BUILD_DIR)/UDateTime.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread UDateTime.cpp

../../../../../../MBus/lib/UException.o: UException.cpp
	g++ -c -o $(BUILD_DIR)/UException.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread UException.cpp

../../../../../../MBus/lib/UIPC.o: UIPC.cpp
	g++ -c -o $(BUILD_DIR)/UIPC.o -g2 -O0 -D __AIX -MD -L/usr/lib/threads -pthread UIPC.cpp

../../../../../../MBus/lib/UFC.a: ../../../../../../MBus/lib/PServerSocket.o ../../../../../../MBus/lib/PClientSocket.o \
         ../../../../../../MBus/lib/NetUtility.o ../../../../../../MBus/lib/MultiplexingServer.o \
         ../../../../../../MBus/lib/PSocket.o ../../../../../../MBus/lib/PUDPClient.o \
         ../../../../../../MBus/lib/SocketHeartBeat.o ../../../../../../MBus/lib/StringBuffer.o \
         ../../../../../../MBus/lib/AnsiString.o ../../../../../../MBus/lib/FileStream.o \
         ../../../../../../MBus/lib/MemoryStream.o ../../../../../../MBus/lib/PThread.o \
         ../../../../../../MBus/lib/List.o ../../../../../../MBus/lib/Exception.o \
         ../../../../../../MBus/lib/ThreadPool.o ../../../../../../MBus/lib/iniFile.o \
         ../../../../../../MBus/lib/MD5.o ../../../../../../MBus/lib/PInt32.o ../../../../../../MBus/lib/PQueue.o \
         ../../../../../../MBus/lib/TField.o ../../../../../../MBus/lib/TRecord.o \
         ../../../../../../MBus/lib/TDataset.o ../../../../../../MBus/lib/TDataFormat.o \
         ../../../../../../MBus/lib/TFixedFormat.o ../../../../../../MBus/lib/TParseData.o \
         ../../../../../../MBus/lib/TRenderData.o ../../../../../../MBus/lib/UDateTime.o \
         ../../../../../../MBus/lib/UException.o ../../../../../../MBus/lib/UIPC.o \
        
	ar q $(BUILD_DIR)/UFC.a ../../../../../../MBus/lib/PServerSocket.o ../../../../../../MBus/lib/PClientSocket.o ../../../../../../MBus/lib/NetUtility.o ../../../../../../MBus/lib/MultiplexingServer.o ../../../../../../MBus/lib/PSocket.o ../../../../../../MBus/lib/PUDPClient.o ../../../../../../MBus/lib/SocketHeartBeat.o ../../../../../../MBus/lib/StringBuffer.o ../../../../../../MBus/lib/AnsiString.o ../../../../../../MBus/lib/FileStream.o ../../../../../../MBus/lib/MemoryStream.o ../../../../../../MBus/lib/PThread.o ../../../../../../MBus/lib/List.o ../../../../../../MBus/lib/Exception.o ../../../../../../MBus/lib/ThreadPool.o ../../../../../../MBus/lib/iniFile.o ../../../../../../MBus/lib/MD5.o ../../../../../../MBus/lib/PInt32.o ../../../../../../MBus/lib/PQueue.o ../../../../../../MBus/lib/TField.o ../../../../../../MBus/lib/TRecord.o ../../../../../../MBus/lib/TDataset.o ../../../../../../MBus/lib/TDataFormat.o ../../../../../../MBus/lib/TFixedFormat.o ../../../../../../MBus/lib/TParseData.o ../../../../../../MBus/lib/TRenderData.o ../../../../../../MBus/lib/UDateTime.o ../../../../../../MBus/lib/UException.o ../../../../../../MBus/lib/UIPC.o

builddir:
	-mkdir -p ..
	-mkdir -p ../MBus
	-mkdir -p ../MBus/lib

clean:
	-rm ../../../../../../MBus/lib/PServerSocket.o
	-rm ../../../../../../MBus/lib/PServerSocket.d
	-rm ../../../../../../MBus/lib/PClientSocket.o
	-rm ../../../../../../MBus/lib/PClientSocket.d
	-rm ../../../../../../MBus/lib/NetUtility.o
	-rm ../../../../../../MBus/lib/NetUtility.d
	-rm ../../../../../../MBus/lib/MultiplexingServer.o
	-rm ../../../../../../MBus/lib/MultiplexingServer.d
	-rm ../../../../../../MBus/lib/PSocket.o
	-rm ../../../../../../MBus/lib/PSocket.d
	-rm ../../../../../../MBus/lib/PUDPClient.o
	-rm ../../../../../../MBus/lib/PUDPClient.d
	-rm ../../../../../../MBus/lib/SocketHeartBeat.o
	-rm ../../../../../../MBus/lib/SocketHeartBeat.d
	-rm ../../../../../../MBus/lib/StringBuffer.o
	-rm ../../../../../../MBus/lib/StringBuffer.d
	-rm ../../../../../../MBus/lib/AnsiString.o
	-rm ../../../../../../MBus/lib/AnsiString.d
	-rm ../../../../../../MBus/lib/FileStream.o
	-rm ../../../../../../MBus/lib/FileStream.d
	-rm ../../../../../../MBus/lib/MemoryStream.o
	-rm ../../../../../../MBus/lib/MemoryStream.d
	-rm ../../../../../../MBus/lib/PThread.o
	-rm ../../../../../../MBus/lib/PThread.d
	-rm ../../../../../../MBus/lib/List.o
	-rm ../../../../../../MBus/lib/List.d
	-rm ../../../../../../MBus/lib/Exception.o
	-rm ../../../../../../MBus/lib/Exception.d
	-rm ../../../../../../MBus/lib/ThreadPool.o
	-rm ../../../../../../MBus/lib/ThreadPool.d
	-rm ../../../../../../MBus/lib/iniFile.o
	-rm ../../../../../../MBus/lib/iniFile.d
	-rm ../../../../../../MBus/lib/MD5.o
	-rm ../../../../../../MBus/lib/MD5.d
	-rm ../../../../../../MBus/lib/PInt32.o
	-rm ../../../../../../MBus/lib/PInt32.d
	-rm ../../../../../../MBus/lib/PQueue.o
	-rm ../../../../../../MBus/lib/PQueue.d
	-rm ../../../../../../MBus/lib/TField.o
	-rm ../../../../../../MBus/lib/TField.d
	-rm ../../../../../../MBus/lib/TRecord.o
	-rm ../../../../../../MBus/lib/TRecord.d
	-rm ../../../../../../MBus/lib/TDataset.o
	-rm ../../../../../../MBus/lib/TDataset.d
	-rm ../../../../../../MBus/lib/TDataFormat.o
	-rm ../../../../../../MBus/lib/TDataFormat.d
	-rm ../../../../../../MBus/lib/TFixedFormat.o
	-rm ../../../../../../MBus/lib/TFixedFormat.d
	-rm ../../../../../../MBus/lib/TParseData.o
	-rm ../../../../../../MBus/lib/TParseData.d
	-rm ../../../../../../MBus/lib/TRenderData.o
	-rm ../../../../../../MBus/lib/TRenderData.d
	-rm ../../../../../../MBus/lib/UDateTime.o
	-rm ../../../../../../MBus/lib/UDateTime.d
	-rm ../../../../../../MBus/lib/UException.o
	-rm ../../../../../../MBus/lib/UException.d
	-rm ../../../../../../MBus/lib/UIPC.o
	-rm ../../../../../../MBus/lib/UIPC.d
	-rm ../../../../../../MBus/lib/UFC.a

endif
