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
OBJECTDIR=build/AIX_Release/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/MBus/lib/MessageDeliver_Lib.a /MBus/lib/UFC.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} /home/yuan/src/SpeedySolution/Simulator/COM/FubonDBMHSender/Run/FubonDBMHSender

/home/yuan/src/SpeedySolution/Simulator/COM/FubonDBMHSender/Run/FubonDBMHSender: ${BUILD_SUBPROJECTS}

/home/yuan/src/SpeedySolution/Simulator/COM/FubonDBMHSender/Run/FubonDBMHSender: ${OBJECTFILES}
	${MKDIR} -p /home/yuan/src/SpeedySolution/Simulator/COM/FubonDBMHSender/Run
	${LINK.cc} -pthread -o /home/yuan/src/SpeedySolution/Simulator/COM/FubonDBMHSender/Run/FubonDBMHSender ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -Wall -DR_6 -D__AIX -I../../UFC -I../../Migo -o ${OBJECTDIR}/Main.o Main.cpp

# Subprojects
.build-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AIX_Release
	${RM} /home/yuan/src/SpeedySolution/Simulator/COM/FubonDBMHSender/Run/FubonDBMHSender

# Subprojects
.clean-subprojects:
	cd ../Lib && ${MAKE}  -f Makefile CONF=AIX_Release clean
