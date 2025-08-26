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
ifeq ($(CFG),AIX Debug Build)
  BUILD_DIR := ../../../../../../Speedy/bin
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
default: builddir ../../../../../../Speedy/bin/SpeedyTSEC

all: default

../../../../../../Speedy/bin/SpeedyOrderClient.o: SpeedyOrderClient.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderClient.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyOrderClient.cpp

../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o: \
         SpeedyOrderConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderConnectionObject.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyOrderConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o: \
         SpeedyConfirmConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyConfirmConnectionObject.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyConfirmConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o: \
         SpeedyFTPConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPConnectionObject.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyFTPConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPRecvObject.o: \
         SpeedyFTPRecvObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPRecvObject.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyFTPRecvObject.cpp

../../../../../../Speedy/bin/SpeedyFTPSendObject.o: SpeedyFTPSendObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPSendObject.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyFTPSendObject.cpp

../../../../../../Speedy/bin/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyObjectsManager.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyObjectsManager.cpp

../../../../../../Speedy/bin/QueryPVC.o: QueryPVC.cpp
	g++ -c -o $(BUILD_DIR)/QueryPVC.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads QueryPVC.cpp

../../../../../../Speedy/bin/Speedy.o: Speedy.cpp
	g++ -c -o $(BUILD_DIR)/Speedy.o -g2 -O0 -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads Speedy.cpp

../../../../../../Speedy/bin/SpeedyTSEC: ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o \
         ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o \
         ../../../../../../Speedy/bin/Speedy.o
	g++ -Bdynamic -L/opt/SUNWconn/lib -o $(BUILD_DIR)/SpeedyTSEC -R/opt/SUNWconn/lib -pthreads ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o ../../../../../../Speedy/bin/Speedy.o /MBus/lib/Migo.a /MBus/lib/UFC.a ../Build/TSECLib.a ../../../../../../MBus/lib/Migo.a ../../../../../../MBus/lib/UFC.a -lrt -lsocket -lnsl -lsx25

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.d
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.d
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.o
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.d
	-rm ../../../../../../Speedy/bin/QueryPVC.o
	-rm ../../../../../../Speedy/bin/QueryPVC.d
	-rm ../../../../../../Speedy/bin/Speedy.o
	-rm ../../../../../../Speedy/bin/Speedy.d
	-rm ../../../../../../Speedy/bin/SpeedyTSEC

endif
ifeq ($(CFG),Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyTSEC

all: default

../../../../../../Speedy/bin/SpeedyOrderClient.o: SpeedyOrderClient.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderClient.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyOrderClient.cpp

../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o: \
         SpeedyOrderConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderConnectionObject.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyOrderConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o: \
         SpeedyConfirmConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyConfirmConnectionObject.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyConfirmConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o: \
         SpeedyFTPConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPConnectionObject.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyFTPConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPRecvObject.o: \
         SpeedyFTPRecvObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPRecvObject.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyFTPRecvObject.cpp

../../../../../../Speedy/bin/SpeedyFTPSendObject.o: SpeedyFTPSendObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPSendObject.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyFTPSendObject.cpp

../../../../../../Speedy/bin/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyObjectsManager.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads SpeedyObjectsManager.cpp

../../../../../../Speedy/bin/QueryPVC.o: QueryPVC.cpp
	g++ -c -o $(BUILD_DIR)/QueryPVC.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads QueryPVC.cpp

../../../../../../Speedy/bin/Speedy.o: Speedy.cpp
	g++ -c -o $(BUILD_DIR)/Speedy.o -D _SOLARIS -MD -I/usr/include -I../Migo -I../UFC -R/opt/SUNWconn/lib -pthreads Speedy.cpp

../../../../../../Speedy/bin/SpeedyTSEC: ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o \
         ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o \
         ../../../../../../Speedy/bin/Speedy.o
	g++ -L/opt/SUNWconn/lib -o $(BUILD_DIR)/SpeedyTSEC -R/opt/SUNWconn/lib -pthreads ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o ../../../../../../Speedy/bin/Speedy.o ../Build/TSECLib.a ../../../../../../MBus/lib/Migo.a ../../../../../../MBus/lib/UFC.a -lrt -lsocket -lnsl -lsx25

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.d
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.d
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.o
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.d
	-rm ../../../../../../Speedy/bin/QueryPVC.o
	-rm ../../../../../../Speedy/bin/QueryPVC.d
	-rm ../../../../../../Speedy/bin/Speedy.o
	-rm ../../../../../../Speedy/bin/Speedy.d
	-rm ../../../../../../Speedy/bin/SpeedyTSEC

endif
ifeq ($(CFG),AIX Release Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyTSEC

all: default

../../../../../../Speedy/bin/SpeedyOrderClient.o: SpeedyOrderClient.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderClient.o -D __AIX -MD -pthread SpeedyOrderClient.cpp

../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o: \
         SpeedyOrderConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderConnectionObject.o -D __AIX -MD -pthread SpeedyOrderConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o: \
         SpeedyConfirmConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyConfirmConnectionObject.o -D __AIX -MD -pthread SpeedyConfirmConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o: \
         SpeedyFTPConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPConnectionObject.o -D __AIX -MD -pthread SpeedyFTPConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPRecvObject.o: \
         SpeedyFTPRecvObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPRecvObject.o -D __AIX -MD -pthread SpeedyFTPRecvObject.cpp

../../../../../../Speedy/bin/SpeedyFTPSendObject.o: SpeedyFTPSendObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPSendObject.o -D __AIX -MD -pthread SpeedyFTPSendObject.cpp

../../../../../../Speedy/bin/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyObjectsManager.o -D __AIX -MD -pthread SpeedyObjectsManager.cpp

../../../../../../Speedy/bin/QueryPVC.o: QueryPVC.cpp
	g++ -c -o $(BUILD_DIR)/QueryPVC.o -D __AIX -MD -pthread QueryPVC.cpp

../../../../../../Speedy/bin/Speedy.o: Speedy.cpp
	g++ -c -o $(BUILD_DIR)/Speedy.o -D __AIX -MD -pthread Speedy.cpp

../../../../../../Speedy/bin/SpeedyTSEC: ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o \
         ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o \
         ../../../../../../Speedy/bin/Speedy.o
	g++ -o $(BUILD_DIR)/SpeedyTSEC -pthread ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o ../../../../../../Speedy/bin/Speedy.o ../Build/TSECLib.a ../../../../../../MBus/lib/Migo.a ../../../../../../MBus/lib/UFC.a

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.d
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.d
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.o
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.d
	-rm ../../../../../../Speedy/bin/QueryPVC.o
	-rm ../../../../../../Speedy/bin/QueryPVC.d
	-rm ../../../../../../Speedy/bin/Speedy.o
	-rm ../../../../../../Speedy/bin/Speedy.d
	-rm ../../../../../../Speedy/bin/SpeedyTSEC

endif
ifeq ($(CFG),AIX Debug Build)
  # Default build target if none specified:
default: builddir ../../../../../../Speedy/bin/SpeedyTSEC

all: default

../../../../../../Speedy/bin/SpeedyOrderClient.o: SpeedyOrderClient.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderClient.o -g2 -O0 -D __AIX -MD -pthread SpeedyOrderClient.cpp

../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o: \
         SpeedyOrderConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyOrderConnectionObject.o -g2 -O0 -D __AIX -MD -pthread SpeedyOrderConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o: \
         SpeedyConfirmConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyConfirmConnectionObject.o -g2 -O0 -D __AIX -MD -pthread SpeedyConfirmConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o: \
         SpeedyFTPConnectionObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPConnectionObject.o -g2 -O0 -D __AIX -MD -pthread SpeedyFTPConnectionObject.cpp

../../../../../../Speedy/bin/SpeedyFTPRecvObject.o: \
         SpeedyFTPRecvObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPRecvObject.o -g2 -O0 -D __AIX -MD -pthread SpeedyFTPRecvObject.cpp

../../../../../../Speedy/bin/SpeedyFTPSendObject.o: SpeedyFTPSendObject.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyFTPSendObject.o -g2 -O0 -D __AIX -MD -pthread SpeedyFTPSendObject.cpp

../../../../../../Speedy/bin/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp
	g++ -c -o $(BUILD_DIR)/SpeedyObjectsManager.o -g2 -O0 -D __AIX -MD -pthread SpeedyObjectsManager.cpp

../../../../../../Speedy/bin/QueryPVC.o: QueryPVC.cpp
	g++ -c -o $(BUILD_DIR)/QueryPVC.o -g2 -O0 -D __AIX -MD -pthread QueryPVC.cpp

../../../../../../Speedy/bin/Speedy.o: Speedy.cpp
	g++ -c -o $(BUILD_DIR)/Speedy.o -g2 -O0 -D __AIX -MD -pthread Speedy.cpp

../../../../../../Speedy/bin/SpeedyTSEC: ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o \
         ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o \
         ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o \
         ../../../../../../Speedy/bin/Speedy.o
	g++ -o $(BUILD_DIR)/SpeedyTSEC -pthread ../../../../../../Speedy/bin/SpeedyOrderClient.o ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o ../../../../../../Speedy/bin/SpeedyFTPSendObject.o ../../../../../../Speedy/bin/SpeedyObjectsManager.o ../../../../../../Speedy/bin/QueryPVC.o ../../../../../../Speedy/bin/Speedy.o ../Build/TSECLib.a ../../../../../../MBus/lib/Migo.a ../../../../../../MBus/lib/UFC.a

builddir:
	-mkdir -p ..
	-mkdir -p ../Speedy
	-mkdir -p ../Speedy/bin

clean:
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderClient.d
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyOrderConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyConfirmConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPConnectionObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPRecvObject.d
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.o
	-rm ../../../../../../Speedy/bin/SpeedyFTPSendObject.d
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.o
	-rm ../../../../../../Speedy/bin/SpeedyObjectsManager.d
	-rm ../../../../../../Speedy/bin/QueryPVC.o
	-rm ../../../../../../Speedy/bin/QueryPVC.d
	-rm ../../../../../../Speedy/bin/Speedy.o
	-rm ../../../../../../Speedy/bin/Speedy.d
	-rm ../../../../../../Speedy/bin/SpeedyTSEC

endif
