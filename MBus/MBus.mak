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
  BUILD_DIR := ../../../../../../MBus/Bin
  Debug Build := 1
endif
ifeq ($(CFG),Release Build)
  BUILD_DIR := ../../../../../../MBus/Bin
  Release Build := 1
endif
ifeq ($(CFG),AIX Debug Build)
  BUILD_DIR := ../../../../../../MBus/Bin
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
default: builddir ../../../../../../MBus/Bin/MBus

all: default

../../../../../../MBus/Bin/MBusMain.o: MBusMain.cpp
	g++ -c -o $(BUILD_DIR)/MBusMain.o -g2 -O0 -MD -I/usr/include -I../UFC -I../Migo -pthreads MBusMain.cpp

../../../../../../MBus/Bin/MBusServer.o: MBusServer.cpp
	g++ -c -o $(BUILD_DIR)/MBusServer.o -g2 -O0 -MD -I/usr/include -I../UFC -I../Migo -pthreads MBusServer.cpp

../../../../../../MBus/Bin/MBus: ../../../../../../MBus/Bin/MBusMain.o ../../../../../../MBus/Bin/MBusServer.o
	g++ -static-libgcc -o $(BUILD_DIR)/MBus -pthreads ../../../../../../MBus/Bin/MBusMain.o ../../../../../../MBus/Bin/MBusServer.o /MBus/lib/Migo.a /MBus/lib/UFC.a -lrt -lsocket -lnsl

builddir: \
        
	-mkdir -p ..
	-mkdir -p ../MBus
	-mkdir -p ../MBus/Bin

clean:
	-rm ../../../../../../MBus/Bin/MBusMain.o
	-rm ../../../../../../MBus/Bin/MBusMain.d
	-rm ../../../../../../MBus/Bin/MBusServer.o
	-rm ../../../../../../MBus/Bin/MBusServer.d
	-rm ../../../../../../MBus/Bin/MBus

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../MBus/Bin/MBus

all: default

../../../../../../MBus/Bin/MBusMain.o: MBusMain.cpp
	g++ -c -o $(BUILD_DIR)/MBusMain.o -MD -I/usr/include -I../UFC -I../Migo -pthreads MBusMain.cpp

../../../../../../MBus/Bin/MBusServer.o: MBusServer.cpp
	g++ -c -o $(BUILD_DIR)/MBusServer.o -MD -I/usr/include -I../UFC -I../Migo -pthreads MBusServer.cpp

../../../../../../MBus/Bin/MBus: ../../../../../../MBus/Bin/MBusMain.o ../../../../../../MBus/Bin/MBusServer.o
	g++ -o $(BUILD_DIR)/MBus -pthreads ../../../../../../MBus/Bin/MBusMain.o ../../../../../../MBus/Bin/MBusServer.o /MBus/lib/Migo.a /MBus/lib/UFC.a -lrt -lsocket -lnsl

builddir: \
        
	-mkdir -p ..
	-mkdir -p ../MBus
	-mkdir -p ../MBus/Bin

clean:
	-rm ../../../../../../MBus/Bin/MBusMain.o
	-rm ../../../../../../MBus/Bin/MBusMain.d
	-rm ../../../../../../MBus/Bin/MBusServer.o
	-rm ../../../../../../MBus/Bin/MBusServer.d
	-rm ../../../../../../MBus/Bin/MBus

endif
ifeq ($(CFG),AIX Debug Build)
  # Default build target if none specified:
default: builddir ../../../../../../MBus/Bin/MBus

all: default

../../../../../../MBus/Bin/MBusMain.o: MBusMain.cpp
	g++ -c -o $(BUILD_DIR)/MBusMain.o -g2 -O0 -D __AIX -MD -I/usr/include -I../UFC -I../Migo -L/usr/lib/threads -pthread MBusMain.cpp

../../../../../../MBus/Bin/MBusServer.o: MBusServer.cpp
	g++ -c -o $(BUILD_DIR)/MBusServer.o -g2 -O0 -D __AIX -MD -I/usr/include -I../UFC -I../Migo -L/usr/lib/threads -pthread MBusServer.cpp

../../../../../../MBus/Bin/MBus: ../../../../../../MBus/Bin/MBusMain.o ../../../../../../MBus/Bin/MBusServer.o
	g++ -o $(BUILD_DIR)/MBus -pthread ../../../../../../MBus/Bin/MBusMain.o ../../../../../../MBus/Bin/MBusServer.o /MBus/lib/Migo.a /MBus/lib/UFC.a

builddir: \
        
	-mkdir -p ..
	-mkdir -p ../MBus
	-mkdir -p ../MBus/Bin

clean:
	-rm ../../../../../../MBus/Bin/MBusMain.o
	-rm ../../../../../../MBus/Bin/MBusMain.d
	-rm ../../../../../../MBus/Bin/MBusServer.o
	-rm ../../../../../../MBus/Bin/MBusServer.d
	-rm ../../../../../../MBus/Bin/MBus

endif
