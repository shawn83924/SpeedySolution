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
OBJECTDIR=build/Debug/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection/Sysque.o \
	${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection/SQMessage.o \
	${OBJECTDIR}/main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lsq2 /MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /Speedy/bin/FubonOrderConnection

/Speedy/bin/FubonOrderConnection: ${BUILD_SUBPROJECTS}

/Speedy/bin/FubonOrderConnection: ${OBJECTFILES}
	${MKDIR} -p /Speedy/bin
	${LINK.cc} -pthread -o /Speedy/bin/FubonOrderConnection ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection/Sysque.o: ../FubonOrderConnection/Sysque.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection
	$(COMPILE.cc) -g3 -gdwarf-2 -D__AIX -o ${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection/Sysque.o ../FubonOrderConnection/Sysque.cpp

${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection/SQMessage.o: ../FubonOrderConnection/SQMessage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection
	$(COMPILE.cc) -g3 -gdwarf-2 -D__AIX -o ${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/COM/FubonExecution/../FubonOrderConnection/SQMessage.o ../FubonOrderConnection/SQMessage.cpp

${OBJECTDIR}/main.o: main.cc 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -D__AIX -o ${OBJECTDIR}/main.o main.cc

# Subprojects
.build-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} /Speedy/bin/FubonOrderConnection

# Subprojects
.clean-subprojects:
	cd ../../UFC && ${MAKE}  -f Makefile_UFC.mk CONF=AIX_Debug clean
