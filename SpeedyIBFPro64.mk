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
	@echo "                           UFC dynamic"
	@echo "==============================================================="
	cd UFCD && make CONF=LINUX_Release_64
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
	@echo "                          Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TWEXLib"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   Exec Rpt Connection"
	@echo "==============================================================="
	cd COM/ExecRptConnection && make CONF=LINUX_Release_64 
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
	@echo "                           IBF Risk Manager 64"
	@echo "==============================================================="
	cd IBFDRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
all: default

###########################################################################
clean:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk  CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UFC dynamic 64"
	@echo "==============================================================="
	cd UFCD && make CONF=LINUX_Release_64 clean
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
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make CONF=LINUX_Release_64 clean
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
	@echo "                          Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TWEXLib"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                    Exec Rpt Connection"
	@echo "==============================================================="
	cd COM/ExecRptConnection && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTW Pro"
	@echo "==============================================================="
	cd SpeedyTWPro && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           IBF Risk Manager 64"
	@echo "==============================================================="
	cd IBFDRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo

