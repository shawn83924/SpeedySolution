# C++BuilderX Version: 1.0.1.103
# Generated GNU Makefile
# Do not modify, as your changes will be lost on re-export

# User Defined Variables:
# End User Defined Variables

# Start of configurations
# If the user specified no configuration on the command line, set a default:
ifndef CFG
  CFG := Debug Build
endif
ifeq ($(CFG),Debug Build)
  BUILD_DIR := ../Build
  Debug Build := 1
endif
ifeq ($(CFG),Release Build)
  BUILD_DIR := solaris/Release_Build
  Release Build := 1
endif
ifneq ($(CFG),Debug Build)
  ifneq ($(CFG),Release Build)
    $(error An incorrect configuration was specified)
  endif
endif
# End of configurations

ifeq ($(CFG),Debug Build)
  # Default build target if none specified:
default: builddir ../Build/SigoPublisher

all: default

../Build/SigoPublisher.o: SigoPublisher.cpp
	g++ -c -o $(BUILD_DIR)/SigoPublisher.o -g2 -O0 -MD -I/usr/include -I../Sigo -I../Migo -I../UFC  SigoPublisher.cpp

../Build/SigoPublisher: ../Build/SigoPublisher.o
	g++ -o $(BUILD_DIR)/SigoPublisher  ../Build/SigoPublisher.o ../Build/Sigo.a ../Build/Migo.a ../Build/UFC.a -lm -lpthread

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/SigoPublisher.o
	-rm ../Build/SigoPublisher.d
	-rm ../Build/SigoPublisher

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir solaris/Release_Build/SigoPublisher

all: default

solaris/Release_Build/SigoPublisher.o: SigoPublisher.cpp
	g++ -c -o $(BUILD_DIR)/SigoPublisher.o -MD -I/usr/include -I/usr/include/g++-3  SigoPublisher.cpp

solaris/Release_Build/SigoPublisher: solaris/Release_Build/SigoPublisher.o
	g++ -o $(BUILD_DIR)/SigoPublisher  solaris/Release_Build/SigoPublisher.o ../Build/Sigo.a ../Build/Migo.a ../Build/UFC.a

builddir:
	-mkdir -p solaris
	-mkdir -p solaris/Release_Build

clean:
	-rm solaris/Release_Build/SigoPublisher.o
	-rm solaris/Release_Build/SigoPublisher.d
	-rm solaris/Release_Build/SigoPublisher

endif
