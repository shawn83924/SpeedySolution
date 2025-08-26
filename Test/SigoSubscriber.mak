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
default: builddir ../Build/SigoSubscriber

all: default

../Build/SigoSubscriber.o: SigoSubscriber.cpp
	g++ -c -o $(BUILD_DIR)/SigoSubscriber.o -g2 -O0 -MD -I/usr/include -I../Sigo -I../Migo -I../UFC  SigoSubscriber.cpp

../Build/SigoSubscriber: ../Build/SigoSubscriber.o
	g++ -o $(BUILD_DIR)/SigoSubscriber  ../Build/SigoSubscriber.o ../Build/Sigo.a ../Build/Migo.a ../Build/UFC.a -lm -lnsl -lpthread

builddir:
	-mkdir -p ..
	-mkdir -p ../Build

clean:
	-rm ../Build/SigoSubscriber.o
	-rm ../Build/SigoSubscriber.d
	-rm ../Build/SigoSubscriber

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir solaris/Release_Build/SigoSubscriber

all: default

solaris/Release_Build/SigoSubscriber.o: SigoSubscriber.cpp
	g++ -c -o $(BUILD_DIR)/SigoSubscriber.o -MD -I/usr/include -I/usr/include/g++-3  SigoSubscriber.cpp

solaris/Release_Build/SigoSubscriber: solaris/Release_Build/SigoSubscriber.o
	g++ -o $(BUILD_DIR)/SigoSubscriber  solaris/Release_Build/SigoSubscriber.o ../Build/Sigo.a ../Build/Migo.a ../Build/UFC.a

builddir:
	-mkdir -p solaris
	-mkdir -p solaris/Release_Build

clean:
	-rm solaris/Release_Build/SigoSubscriber.o
	-rm solaris/Release_Build/SigoSubscriber.d
	-rm solaris/Release_Build/SigoSubscriber

endif
