###########################################################################
default:
	@echo
	@echo "                           UFC Static Release" 
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UFC dynamic Release"
	@echo "==============================================================="
	cd UFCD && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           DataStore Static Release"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           DataStore Dynamic Release"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_Dynamic_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAPI Static Release"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAPI Dynamic Release"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Release_dynamic_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LUA Static Release"
	@echo "==============================================================="
	cd Lua && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TWEXLib Static Release"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TWEXLib Dynamic Release"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_dynamic_64
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
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make  CONF=LINUX_Release_64
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
	@echo "                   KGI Exec Connection"
	@echo "==============================================================="
	cd COM/KGIExecConnection && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTW Pro"
	@echo "==============================================================="
	cd SpeedyTWPro && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SMP Recover"
	@echo "==============================================================="
	cd SMPRecover && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
all: default

###########################################################################
clean:
	@echo
	@echo "                           Clean UFC Static Release"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk  CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean UFC Dynamic Release"
	@echo "==============================================================="
	cd UFCD && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean DataStore Static Release"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean DataStore Dynamic Release"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_Dynamic_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyAPI Static Release"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyAPI Dynamic Release"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Release_dynamic_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean LUA Static Release"
	@echo "==============================================================="
	cd Lua && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean TWEXLib Static Release"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean TWEXLib Dynamic Release"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_dynamic_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean MBus"
	@echo "==============================================================="
	cd MBus && make -f Makefile_MBus.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           Clean LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   Clean KGI Exec Connection"
	@echo "==============================================================="
	cd COM/KGIExecConnection && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyTW Pro"
	@echo "==============================================================="
	cd SpeedyTWPro && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SMP Recover"
	@echo "==============================================================="
	cd SMPRecover && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
