###########################################################################
default:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           MBus"
	@echo "==============================================================="
	cd MBus && make -f Makefile_MBus.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyTSEC"
	@echo "==============================================================="
	cd SpeedyTSEC && make -f Makefile_SpeedyTSEC.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyFTTSEC"
	@echo "==============================================================="
	cd SpeedyFTTSEC && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                      FubonOrderConnection"
	@echo "==============================================================="
	cd COM/FubonOrderConnection && make
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                      FubonFTConnection"
	@echo "==============================================================="
	cd COM/FubonFTConnection && make
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                      FubonExecution"
	@echo "==============================================================="
	cd COM/FubonFTConnection && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
all: default

###########################################################################
clean:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           MBus"
	@echo "==============================================================="
	cd MBus && make -f Makefile_MBus.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyTSEC"
	@echo "==============================================================="
	cd SpeedyTSEC && make -f Makefile_SpeedyTSEC.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyFTTSEC"
	@echo "==============================================================="
	cd SpeedyFTTSEC && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                      FubonOrderConnection"
	@echo "==============================================================="
	cd COM/FubonOrderConnection && make clean
	@echo "==============================================================="
	@echo

#
	@echo
	@echo "                      FubonFTConnection"
	@echo "==============================================================="
	cd COM/FubonFTConnection && make clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                      FubonExecution"
	@echo "==============================================================="
	cd COM/FubonFTConnection && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
