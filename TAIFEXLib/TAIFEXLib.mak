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
ifeq ($(CFG),AIX Debug Build)
  BUILD_DIR := ../Build
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
default: builddir ../Build/TAIFEXLib.a

all: default

../Build/CMessages.o: CMessages.cpp
	g++ -c -o $(BUILD_DIR)/CMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads CMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads FMessages.cpp

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads LMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads Utility.cpp

../Build/XBIODefine.o: XBIODefine.cpp
	g++ -c -o $(BUILD_DIR)/XBIODefine.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads XBIODefine.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads IBMX25Connection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -Wall -g2 -O0 -D _SOLARIS -MD -I/usr/include -pthreads LinkSubSystem.cpp

../Build/TAIFEXLib.a: ../Build/CMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/LMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/XBIODefine.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/LinkSubSystem.o
	ar q $(BUILD_DIR)/TAIFEXLib.a ../Build/CMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/LMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/XBIODefine.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/CMessages.o
	-rm ../Build/CMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/XBIODefine.o
	-rm ../Build/XBIODefine.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TAIFEXLib.a

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../Build/TAIFEXLib.a

all: default

../Build/CMessages.o: CMessages.cpp
	g++ -c -o $(BUILD_DIR)/CMessages.o -MD -I/usr/local/include -pthreads CMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -MD -I/usr/local/include -pthreads RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -MD -I/usr/local/include -pthreads FMessages.cpp

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -MD -I/usr/local/include -pthreads LMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -MD -I/usr/local/include -pthreads MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -MD -I/usr/local/include -pthreads Utility.cpp

../Build/XBIODefine.o: XBIODefine.cpp
	g++ -c -o $(BUILD_DIR)/XBIODefine.o -MD -I/usr/local/include -pthreads XBIODefine.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -MD -I/usr/local/include -pthreads SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -MD -I/usr/local/include -pthreads IBMX25Connection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -MD -I/usr/local/include -pthreads LinkSubSystem.cpp

../Build/TAIFEXLib.a: ../Build/CMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/LMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/XBIODefine.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/LinkSubSystem.o
	ar q $(BUILD_DIR)/TAIFEXLib.a ../Build/CMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/LMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/XBIODefine.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/CMessages.o
	-rm ../Build/CMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/XBIODefine.o
	-rm ../Build/XBIODefine.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TAIFEXLib.a

endif
ifeq ($(CFG),AIX Debug Build)
  # Default build target if none specified:
default: builddir ../Build/TAIFEXLib.a

all: default

../Build/CMessages.o: CMessages.cpp
	g++ -c -o $(BUILD_DIR)/CMessages.o -g2 -O0 -D __AIX -MD -pthread CMessages.cpp

../Build/RMessages.o: RMessages.cpp
	g++ -c -o $(BUILD_DIR)/RMessages.o -g2 -O0 -D __AIX -MD -pthread RMessages.cpp

../Build/FMessages.o: FMessages.cpp
	g++ -c -o $(BUILD_DIR)/FMessages.o -g2 -O0 -D __AIX -MD -pthread FMessages.cpp

../Build/LMessages.o: LMessages.cpp
	g++ -c -o $(BUILD_DIR)/LMessages.o -g2 -O0 -D __AIX -MD -pthread LMessages.cpp

../Build/MessageHeader.o: MessageHeader.cpp
	g++ -c -o $(BUILD_DIR)/MessageHeader.o -g2 -O0 -D __AIX -MD -pthread MessageHeader.cpp

../Build/Utility.o: Utility.cpp
	g++ -c -o $(BUILD_DIR)/Utility.o -g2 -O0 -D __AIX -MD -pthread Utility.cpp

../Build/XBIODefine.o: XBIODefine.cpp
	g++ -c -o $(BUILD_DIR)/XBIODefine.o -g2 -O0 -D __AIX -MD -pthread XBIODefine.cpp

../Build/SUNX25Connection.o: SUNX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/SUNX25Connection.o -g2 -O0 -D __AIX -MD -pthread SUNX25Connection.cpp

../Build/IBMX25Connection.o: IBMX25Connection.cpp
	g++ -c -o $(BUILD_DIR)/IBMX25Connection.o -g2 -O0 -D __AIX -MD -pthread IBMX25Connection.cpp

../Build/LinkSubSystem.o: LinkSubSystem.cpp
	g++ -c -o $(BUILD_DIR)/LinkSubSystem.o -g2 -O0 -D __AIX -MD -pthread LinkSubSystem.cpp

../Build/TAIFEXLib.a: ../Build/CMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/LMessages.o \
         ../Build/MessageHeader.o ../Build/Utility.o ../Build/XBIODefine.o ../Build/SUNX25Connection.o \
         ../Build/IBMX25Connection.o ../Build/LinkSubSystem.o
	ar q $(BUILD_DIR)/TAIFEXLib.a ../Build/CMessages.o ../Build/RMessages.o ../Build/FMessages.o ../Build/LMessages.o ../Build/MessageHeader.o ../Build/Utility.o ../Build/XBIODefine.o ../Build/SUNX25Connection.o ../Build/IBMX25Connection.o ../Build/LinkSubSystem.o

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/CMessages.o
	-rm ../Build/CMessages.d
	-rm ../Build/RMessages.o
	-rm ../Build/RMessages.d
	-rm ../Build/FMessages.o
	-rm ../Build/FMessages.d
	-rm ../Build/LMessages.o
	-rm ../Build/LMessages.d
	-rm ../Build/MessageHeader.o
	-rm ../Build/MessageHeader.d
	-rm ../Build/Utility.o
	-rm ../Build/Utility.d
	-rm ../Build/XBIODefine.o
	-rm ../Build/XBIODefine.d
	-rm ../Build/SUNX25Connection.o
	-rm ../Build/SUNX25Connection.d
	-rm ../Build/IBMX25Connection.o
	-rm ../Build/IBMX25Connection.d
	-rm ../Build/LinkSubSystem.o
	-rm ../Build/LinkSubSystem.d
	-rm ../Build/TAIFEXLib.a

endif
