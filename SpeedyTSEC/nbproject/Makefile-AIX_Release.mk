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
include Makefile_SpeedyTSEC.mk

# Object Directory
OBJECTDIR=build/AIX_Release/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/SpeedyOrderClient.o \
	${OBJECTDIR}/OrderInfoStore.o \
	${OBJECTDIR}/SpeedyOrderConnectionObject.o \
	${OBJECTDIR}/QueryPVC.o \
	${OBJECTDIR}/Speedy.o \
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
LDLIBSOPTIONS=/MBus/lib/TSECLib.a /MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/SpeedyTSEC

/Speedy/bin/SpeedyTSEC: ${BUILD_SUBPROJECTS}

/Speedy/bin/SpeedyTSEC: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -o /Speedy/bin/SpeedyTSEC -s ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SpeedyOrderClient.o: SpeedyOrderClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyOrderClient.o SpeedyOrderClient.cpp

${OBJECTDIR}/OrderInfoStore.o: OrderInfoStore.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/OrderInfoStore.o OrderInfoStore.cpp

${OBJECTDIR}/SpeedyOrderConnectionObject.o: SpeedyOrderConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyOrderConnectionObject.o SpeedyOrderConnectionObject.cpp

${OBJECTDIR}/QueryPVC.o: QueryPVC.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/QueryPVC.o QueryPVC.cpp

${OBJECTDIR}/Speedy.o: Speedy.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/Speedy.o Speedy.cpp

${OBJECTDIR}/ProcessMonitor.o: ProcessMonitor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/ProcessMonitor.o ProcessMonitor.cpp

${OBJECTDIR}/SpeedyConfirmConnectionObject.o: SpeedyConfirmConnectionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyConfirmConnectionObject.o SpeedyConfirmConnectionObject.cpp

${OBJECTDIR}/SpeedyObjectsManager.o: SpeedyObjectsManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -D__AIX -I../UFC -I../Migo -o ${OBJECTDIR}/SpeedyObjectsManager.o SpeedyObjectsManager.cpp

# Subprojects
.build-subprojects:
	cd ../TSECLib && ${MAKE} -f Makefile_TSECLib.mk CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release
	${RM} /Speedy/bin/SpeedyTSEC

# Subprojects
.clean-subprojects:
	cd ../TSECLib && ${MAKE} -f Makefile_TSECLib.mk CONF=AIX_Release clean
