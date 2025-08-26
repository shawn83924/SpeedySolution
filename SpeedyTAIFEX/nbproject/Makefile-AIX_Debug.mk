#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add custumized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/AIX_Debug/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/OrderInfoStore.o \
	${OBJECTDIR}/SpeedyFTPRecvObject.o \
	${OBJECTDIR}/SpeedyMarketDataConnectionObject.o \
	${OBJECTDIR}/SpeedyTMPOrderConnectionObject.o \
	${OBJECTDIR}/Speedy.o \
	${OBJECTDIR}/SpeedyFTPConnectionObject.o \
	${OBJECTDIR}/SpeedyOrderClient.o \
	${OBJECTDIR}/SpeedyTMPOrderClient.o \
	${OBJECTDIR}/SpeedyOrderConnectionObject.o \
	${OBJECTDIR}/SpeedyFTPSendObject.o \
	${OBJECTDIR}/QueryPVC.o \
	${OBJECTDIR}/ProcessMonitor.o \
	${OBJECTDIR}/SpeedyConfirmConnectionObject.o \
	${OBJECTDIR}/SpeedyObjectsManager.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../TAIFEXLib/../Build/TAIFEXLib.a /MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/Speedy

/Speedy/bin/Speedy: ${BUILD_SUBPROJECTS}

/Speedy/bin/Speedy: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/Speedy ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/OrderInfoStore.o: OrderInfoStore.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/OrderInfoStore.o OrderInfoStore.cpp

${OBJECTDIR}/SpeedyFTPRecvObject.o: SpeedyFTPRecvObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyFTPRecvObject.o SpeedyFTPRecvObject.cpp

${OBJECTDIR}/SpeedyMarketDataConnectionObject.o: SpeedyMarketDataConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyMarketDataConnectionObject.o SpeedyMarketDataConnectionObject.cpp

${OBJECTDIR}/SpeedyTMPOrderConnectionObject.o: SpeedyTMPOrderConnectionObject.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyTMPOrderConnectionObject.o SpeedyTMPOrderConnectionObject.cc

${OBJECTDIR}/Speedy.o: Speedy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/Speedy.o Speedy.cpp

${OBJECTDIR}/SpeedyFTPConnectionObject.o: SpeedyFTPConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyFTPConnectionObject.o SpeedyFTPConnectionObject.cpp

${OBJECTDIR}/SpeedyOrderClient.o: SpeedyOrderClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyOrderClient.o SpeedyOrderClient.cpp

${OBJECTDIR}/SpeedyTMPOrderClient.o: SpeedyTMPOrderClient.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyTMPOrderClient.o SpeedyTMPOrderClient.cc

${OBJECTDIR}/SpeedyOrderConnectionObject.o: SpeedyOrderConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyOrderConnectionObject.o SpeedyOrderConnectionObject.cpp

${OBJECTDIR}/SpeedyFTPSendObject.o: SpeedyFTPSendObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyFTPSendObject.o SpeedyFTPSendObject.cpp

${OBJECTDIR}/QueryPVC.o: QueryPVC.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/QueryPVC.o QueryPVC.cpp

${OBJECTDIR}/ProcessMonitor.o: ProcessMonitor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/ProcessMonitor.o ProcessMonitor.cpp

${OBJECTDIR}/SpeedyConfirmConnectionObject.o: SpeedyConfirmConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyConfirmConnectionObject.o SpeedyConfirmConnectionObject.cpp

${OBJECTDIR}/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyObjectsManager.o SpeedyObjectsManager.cpp

# Subprojects
.build-subprojects:
	cd ../TAIFEXLib && ${MAKE} -f Makefile CONF=AIX_Debug
	cd ../UFC && ${MAKE} -f Makefile_UFC.mk CONF=AIX_Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Debug
	${RM} /Speedy/bin/Speedy

# Subprojects
.clean-subprojects:
	cd ../TAIFEXLib && ${MAKE} -f Makefile CONF=AIX_Debug clean
	cd ../UFC && ${MAKE} -f Makefile_UFC.mk CONF=AIX_Debug clean
