# C++BuilderX Version: 1.0.1.103
# Generated GNU Makefile
# Do not modify, as your changes will be lost on re-export

# User Defined Variables:
# End User Defined Variables

# Start of configurations
# If the user specified no configuration on the command line, set a default:
ifndef CFG
  CFG := AIX Release Build
endif
ifeq ($(CFG),Debug Build)
  BUILD_DIR := ../../../../../../Speedy/bin
  Debug Build := 1
endif
ifeq ($(CFG),Release Build)
  BUILD_DIR := ../../../../../../Speedy/bin
  Release Build := 1
endif
ifeq ($(CFG),AIX Release Build)
  BUILD_DIR := ../../../../../../Speedy/bin
  AIX Release Build := 1
endif
ifneq ($(CFG),Debug Build)
  ifneq ($(CFG),Release Build)
    ifneq ($(CFG),AIX Release Build)
      $(error An incorrect configuration was specified)
    endif
  endif
endif
# End of configurations

ifeq ($(CFG),Debug Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyManager

all: default

../../../../../../Speedy/bin/SpeedyManager.o: SpeedyManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyManager.o -g2 -O0 -MD -I/usr/include -I/home/yuan/src/SimTAIFEX/Simulator/Sigo -I/home/yuan/src/SimTAIFEX/Simulator/Migo -I/home/yuan/src/SimTAIFEX/Simulator/UFC  SpeedyManager.cpp

../../../../../../Speedy/bin/UserDB.o: UserDB.cpp
	g++ -c -o $(BUILD_DIR)/UserDB.o -g2 -O0 -MD -I/usr/include -I/home/yuan/src/SimTAIFEX/Simulator/Sigo -I/home/yuan/src/SimTAIFEX/Simulator/Migo -I/home/yuan/src/SimTAIFEX/Simulator/UFC  UserDB.cpp

../../../../../../Speedy/bin/SpeedyManager: ../../../../../../Speedy/bin/SpeedyManager.o ../../../../../../Speedy/bin/UserDB.o \
        
	g++ -o $(BUILD_DIR)/SpeedyManager  ../../../../../../Speedy/bin/SpeedyManager.o ../../../../../../Speedy/bin/UserDB.o /MBus/lib/Migo.a /MBus/lib/UFC.a -lrt -lsocket -lnsl -lpthread

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyManager.o
	-rm ../../../../../../Speedy/bin/SpeedyManager.d
	-rm ../../../../../../Speedy/bin/UserDB.o
	-rm ../../../../../../Speedy/bin/UserDB.d
	-rm ../../../../../../Speedy/bin/SpeedyManager

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyManager

all: default

../../../../../../Speedy/bin/SpeedyManager.o: SpeedyManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyManager.o -MD -I/usr/local/include -pthreads SpeedyManager.cpp

../../../../../../Speedy/bin/UserDB.o: UserDB.cpp
	g++ -c -o $(BUILD_DIR)/UserDB.o -MD -I/usr/local/include -pthreads UserDB.cpp

../../../../../../Speedy/bin/SpeedyManager: ../../../../../../Speedy/bin/SpeedyManager.o ../../../../../../Speedy/bin/UserDB.o \
        
	g++ -o $(BUILD_DIR)/SpeedyManager -pthreads ../../../../../../Speedy/bin/SpeedyManager.o ../../../../../../Speedy/bin/UserDB.o /MBus/lib/Migo.a /MBus/lib/UFC.a -lrt -lsocket -lnsl -lpthread

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyManager.o
	-rm ../../../../../../Speedy/bin/SpeedyManager.d
	-rm ../../../../../../Speedy/bin/UserDB.o
	-rm ../../../../../../Speedy/bin/UserDB.d
	-rm ../../../../../../Speedy/bin/SpeedyManager

endif
ifeq ($(CFG),AIX Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyManager

all: default

../../../../../../Speedy/bin/SpeedyManager.o: SpeedyManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyManager.o -MD -pthread SpeedyManager.cpp

../../../../../../Speedy/bin/UserDB.o: UserDB.cpp
	g++ -c -o $(BUILD_DIR)/UserDB.o -MD -pthread UserDB.cpp

../../../../../../Speedy/bin/SpeedyManager: ../../../../../../Speedy/bin/SpeedyManager.o ../../../../../../Speedy/bin/UserDB.o \
        
	g++ -o $(BUILD_DIR)/SpeedyManager -pthread ../../../../../../Speedy/bin/SpeedyManager.o ../../../../../../Speedy/bin/UserDB.o /usr/MBus/lib/UFC.a /usr/MBus/lib/Migo.a

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyManager.o
	-rm ../../../../../../Speedy/bin/SpeedyManager.d
	-rm ../../../../../../Speedy/bin/UserDB.o
	-rm ../../../../../../Speedy/bin/UserDB.d
	-rm ../../../../../../Speedy/bin/SpeedyManager

endif
