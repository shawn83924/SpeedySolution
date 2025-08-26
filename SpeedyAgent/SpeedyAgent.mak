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
default: builddir ../../../../../../Speedy/bin/SpeedyAgent

all: default

../../../../../../Speedy/bin/SpeedyAgent.o: SpeedyAgent.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyAgent.o -g2 -O0 -MD -I/usr/include -I../Migo -I../UFC  SpeedyAgent.cpp

../../../../../../Speedy/bin/SpeedyAgent: ../../../../../../Speedy/bin/SpeedyAgent.o
	g++ -o $(BUILD_DIR)/SpeedyAgent -pthreads ../../../../../../Speedy/bin/SpeedyAgent.o /MBus/lib/Migo.a /MBus/lib/UFC.a -lrt -lsocket -lnsl

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyAgent.o
	-rm ../../../../../../Speedy/bin/SpeedyAgent.d
	-rm ../../../../../../Speedy/bin/SpeedyAgent

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyAgent

all: default

../../../../../../Speedy/bin/SpeedyAgent.o: SpeedyAgent.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyAgent.o -MD -I/usr/local/include -pthreads SpeedyAgent.cpp

../../../../../../Speedy/bin/SpeedyAgent: ../../../../../../Speedy/bin/SpeedyAgent.o
	g++ -o $(BUILD_DIR)/SpeedyAgent -pthreads ../../../../../../Speedy/bin/SpeedyAgent.o /MBus/lib/Migo.a /MBus/lib/UFC.a -lrt -lsocket -lnsl

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyAgent.o
	-rm ../../../../../../Speedy/bin/SpeedyAgent.d
	-rm ../../../../../../Speedy/bin/SpeedyAgent

endif
ifeq ($(CFG),AIX Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyAgent

all: default

../../../../../../Speedy/bin/SpeedyAgent.o: SpeedyAgent.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyAgent.o -MD -pthread SpeedyAgent.cpp

../../../../../../Speedy/bin/SpeedyAgent: ../../../../../../Speedy/bin/SpeedyAgent.o
	g++ -o $(BUILD_DIR)/SpeedyAgent -pthread ../../../../../../Speedy/bin/SpeedyAgent.o /usr/MBus/lib/UFC.a /usr/MBus/lib/Migo.a

builddir:
	-mkdir -p ..
	-mkdir -p ../usr
	-mkdir -p ../usr/Speedy
	-mkdir -p ../usr/Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyAgent.o
	-rm ../../../../../../Speedy/bin/SpeedyAgent.d
	-rm ../../../../../../Speedy/bin/SpeedyAgent

endif
