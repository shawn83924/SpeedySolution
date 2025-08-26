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
	@echo "                           LUA lib"
	@echo "==============================================================="
	cd Lua && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64  
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make CONF=LINUX_Release_64  
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TSECLib"
	@echo "==============================================================="
	cd TSECLib && make -f Makefile_TSECLib.mk CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTSEC"
	@echo "==============================================================="
	cd SpeedyTSEC && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyFTTSEC"
	@echo "==============================================================="
	cd SpeedyFTTSEC && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
			
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
	@echo "                           LUA lib"
	@echo "==============================================================="
	cd Lua && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make CONF=LINUX_Release_64  clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TSECLib"
	@echo "==============================================================="
	cd TSECLib && make -f Makefile_TSECLib.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTSEC"
	@echo "==============================================================="
	cd SpeedyTSEC && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyFTTSEC"
	@echo "==============================================================="
	cd SpeedyFTTSEC && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
