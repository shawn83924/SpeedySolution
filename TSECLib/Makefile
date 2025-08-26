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
  BUILD_DIR := ../Build
  Debug Build := 1
endif
ifeq ($(CFG),Release Build)
  BUILD_DIR := ../Build
  Release Build := 1
endif
ifeq ($(CFG),AIX Release Build)
  BUILD_DIR := ../Build
  AIX Release Build := 1
endif
ifeq ($(CFG),AIX Debug Build)
  BUILD_DIR := ../Build
  AIX Debug Build := 1
endif
ifneq ($(CFG),Debug Build)
  ifneq ($(CFG),Release Build)
    ifneq ($(CFG),AIX Release Build)
      ifneq ($(CFG),AIX Debug Build)
        $(error An incorrect configuration was specified)
      endif
    endif
  endif
endif
# End of configurations

ifeq ($(CFG),Debug Build)
  # Default build target if none specified:
default: builddir ../Build/TSECLib.a

all: default

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads LMessages.cpp

../Build/TMessages.o: TMessages.cpp
	g++ -c -o $(BUILD_DIR)/TMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads TMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads FMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads Utility.cpp

../Build/ExchangeConst.o: ExchangeConst.cpp
	g++ -c -o $(BUILD_DIR)/ExchangeConst.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads ExchangeConst.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads IBMX25Connection.cpp

../Build/SocketConnection.o: SocketConnection.cpp
	g++ -c -o $(BUILD_DIR)/SocketConnection.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads SocketConnection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -Wall -g2 -O0 -D _SOLARIS -MD -pthreads LinkSubSystem.cpp

../Build/TSECLib.a: ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o \
        
	ar q $(BUILD_DIR)/TSECLib.a ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/TMessages.o
	-rm ../Build/TMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/ExchangeConst.o
	-rm ../Build/ExchangeConst.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/SocketConnection.o
	-rm ../Build/SocketConnection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TSECLib.a

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../Build/TSECLib.a

all: default

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -D _SOLARIS -MD -pthreads LMessages.cpp

../Build/TMessages.o: TMessages.cpp
	g++ -c -o $(BUILD_DIR)/TMessages.o -D _SOLARIS -MD -pthreads TMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -D _SOLARIS -MD -pthreads RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -D _SOLARIS -MD -pthreads FMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -D _SOLARIS -MD -pthreads MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -D _SOLARIS -MD -pthreads Utility.cpp

../Build/ExchangeConst.o: ExchangeConst.cpp
	g++ -c -o $(BUILD_DIR)/ExchangeConst.o -D _SOLARIS -MD -pthreads ExchangeConst.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -D _SOLARIS -MD -pthreads SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -D _SOLARIS -MD -pthreads IBMX25Connection.cpp

../Build/SocketConnection.o: SocketConnection.cpp
	g++ -c -o $(BUILD_DIR)/SocketConnection.o -D _SOLARIS -MD -pthreads SocketConnection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -D _SOLARIS -MD -pthreads LinkSubSystem.cpp

../Build/TSECLib.a: ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o \
        
	ar q $(BUILD_DIR)/TSECLib.a ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/TMessages.o
	-rm ../Build/TMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/ExchangeConst.o
	-rm ../Build/ExchangeConst.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/SocketConnection.o
	-rm ../Build/SocketConnection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TSECLib.a

endif
ifeq ($(CFG),AIX Release Build)
  # Default build target if none specified:
default: builddir ../Build/TSECLib.a

all: default

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -D __AIX -MD -pthread LMessages.cpp

../Build/TMessages.o: TMessages.cpp
	g++ -c -o $(BUILD_DIR)/TMessages.o -D __AIX -MD -pthread TMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -D __AIX -MD -pthread RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -D __AIX -MD -pthread FMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -D __AIX -MD -pthread MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -D __AIX -MD -pthread Utility.cpp

../Build/ExchangeConst.o: ExchangeConst.cpp
	g++ -c -o $(BUILD_DIR)/ExchangeConst.o -D __AIX -MD -pthread ExchangeConst.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -D __AIX -MD -pthread SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -D __AIX -MD -pthread IBMX25Connection.cpp

../Build/SocketConnection.o: SocketConnection.cpp
	g++ -c -o $(BUILD_DIR)/SocketConnection.o -D __AIX -MD -pthread SocketConnection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -D __AIX -MD -pthread LinkSubSystem.cpp

../Build/TSECLib.a: ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o \
        
	ar -r $(BUILD_DIR)/TSECLib.a ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/TMessages.o
	-rm ../Build/TMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/ExchangeConst.o
	-rm ../Build/ExchangeConst.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/SocketConnection.o
	-rm ../Build/SocketConnection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TSECLib.a

endif
ifeq ($(CFG),AIX Debug Build)
  # Default build target if none specified:
default: builddir ../Build/TSECLib.a

all: default

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -g2 -O0 -D __AIX -MD -pthread LMessages.cpp

../Build/TMessages.o: TMessages.cpp
	g++ -c -o $(BUILD_DIR)/TMessages.o -g2 -O0 -D __AIX -MD -pthread TMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -g2 -O0 -D __AIX -MD -pthread RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -g2 -O0 -D __AIX -MD -pthread FMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -g2 -O0 -D __AIX -MD -pthread MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -g2 -O0 -D __AIX -MD -pthread Utility.cpp

../Build/ExchangeConst.o: ExchangeConst.cpp
	g++ -c -o $(BUILD_DIR)/ExchangeConst.o -g2 -O0 -D __AIX -MD -pthread ExchangeConst.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -g2 -O0 -D __AIX -MD -pthread SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -g2 -O0 -D __AIX -MD -pthread IBMX25Connection.cpp

../Build/SocketConnection.o: SocketConnection.cpp
	g++ -c -o $(BUILD_DIR)/SocketConnection.o -g2 -O0 -D __AIX -MD -pthread SocketConnection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -g2 -O0 -D __AIX -MD -pthread LinkSubSystem.cpp

../Build/TSECLib.a: ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o \
        
	ar -r $(BUILD_DIR)/TSECLib.a ../Build/LMessages.o ../Build/TMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/ExchangeConst.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/SocketConnection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/TMessages.o
	-rm ../Build/TMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/ExchangeConst.o
	-rm ../Build/ExchangeConst.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/SocketConnection.o
	-rm ../Build/SocketConnection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TSECLib.a

endif
