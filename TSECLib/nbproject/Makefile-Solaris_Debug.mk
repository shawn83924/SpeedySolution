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
FC=g77

# Include project Makefile
include Makefile_TSECLib.mk

# Object Directory
OBJECTDIR=build/Solaris_Debug/GNU-Solaris-Sparc

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/MessageHeader.o \
	${OBJECTDIR}/SocketConnection.o \
	${OBJECTDIR}/IBMX25Connection.o \
	${OBJECTDIR}/EMessages.o \
	${OBJECTDIR}/TMessages.o \
	${OBJECTDIR}/FMessages.o \
	${OBJECTDIR}/_ext/space/src/SpeedySolution/Simulator/TSECLib/../ExchangeUtility/Utility.o \
	${OBJECTDIR}/SUNX25Connection.o \
	${OBJECTDIR}/LMessages.o \
	${OBJECTDIR}/VMessages.o \
	${OBJECTDIR}/ExchangeConst.o \
	${OBJECTDIR}/AMessages.o \
	${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/TSECLib/ExMessages.o \
	${OBJECTDIR}/LinkSubSystem.o \
	${OBJECTDIR}/SMessages.o \
	${OBJECTDIR}/PMessages.o \
	${OBJECTDIR}/RMessages.o \
	${OBJECTDIR}/OMessages.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthreads
CXXFLAGS=-pthreads

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} ../Build/TSECLib.a

../Build/TSECLib.a: ${OBJECTFILES}
	${MKDIR} -p ../Build
	${RM} ../Build/TSECLib.a
	${AR} rv ../Build/TSECLib.a ${OBJECTFILES} 
	$(RANLIB) ../Build/TSECLib.a

${OBJECTDIR}/MessageHeader.o: MessageHeader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/MessageHeader.o MessageHeader.cpp

${OBJECTDIR}/SocketConnection.o: SocketConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/SocketConnection.o SocketConnection.cpp

${OBJECTDIR}/IBMX25Connection.o: IBMX25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/IBMX25Connection.o IBMX25Connection.cpp

${OBJECTDIR}/EMessages.o: EMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/EMessages.o EMessages.cpp

${OBJECTDIR}/TMessages.o: TMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/TMessages.o TMessages.cpp

${OBJECTDIR}/FMessages.o: FMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/FMessages.o FMessages.cpp

${OBJECTDIR}/_ext/space/src/SpeedySolution/Simulator/TSECLib/../ExchangeUtility/Utility.o: ../ExchangeUtility/Utility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/space/src/SpeedySolution/Simulator/TSECLib/../ExchangeUtility
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/_ext/space/src/SpeedySolution/Simulator/TSECLib/../ExchangeUtility/Utility.o ../ExchangeUtility/Utility.cpp

${OBJECTDIR}/SUNX25Connection.o: SUNX25Connection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/SUNX25Connection.o SUNX25Connection.cpp

${OBJECTDIR}/LMessages.o: LMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/LMessages.o LMessages.cpp

${OBJECTDIR}/VMessages.o: VMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/VMessages.o VMessages.cpp

${OBJECTDIR}/ExchangeConst.o: ExchangeConst.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/ExchangeConst.o ExchangeConst.cpp

${OBJECTDIR}/AMessages.o: AMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/AMessages.o AMessages.cpp

${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/TSECLib/ExMessages.o: /home/yuan/src/SpeedySolution/Simulator/TSECLib/ExMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/TSECLib
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/_ext/home/yuan/src/SpeedySolution/Simulator/TSECLib/ExMessages.o /home/yuan/src/SpeedySolution/Simulator/TSECLib/ExMessages.cpp

${OBJECTDIR}/LinkSubSystem.o: LinkSubSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/LinkSubSystem.o LinkSubSystem.cpp

${OBJECTDIR}/SMessages.o: SMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/SMessages.o SMessages.cpp

${OBJECTDIR}/PMessages.o: PMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/PMessages.o PMessages.cpp

${OBJECTDIR}/RMessages.o: RMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/RMessages.o RMessages.cpp

${OBJECTDIR}/OMessages.o: OMessages.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g3 -gdwarf-2 -Wall -D__SOLARIS -o ${OBJECTDIR}/OMessages.o OMessages.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Solaris_Debug
	${RM} ../Build/TSECLib.a

# Subprojects
.clean-subprojects:
