###########################################################################
default:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           MBus"
	@echo "==============================================================="
	cd MBus && make -f Makefile_MBus.mk  CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk  CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && make -f Makefile_SpeedyManager.mk  CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyTSEC"
	@echo "==============================================================="
	cd SpeedyTSEC && make -f Makefile_SpeedyTSEC.mk  CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyFTTSEC"
	@echo "==============================================================="
	cd SpeedyFTTSEC && make  CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo

all: default

###########################################################################
clean:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           MBus"
	@echo "==============================================================="
	cd MBus && make -f Makefile_MBus.mk CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && make -f Makefile_SpeedyManager.mk  CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyTSEC"
	@echo "==============================================================="
	cd SpeedyTSEC && make -f Makefile_SpeedyTSEC.mk  CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyFTTSEC"
	@echo "==============================================================="
	cd SpeedyFTTSEC && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo

