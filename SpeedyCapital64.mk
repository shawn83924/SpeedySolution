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
	@echo "                           TWEXLib"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   Capital Exec ConnectionTCP"
	@echo "==============================================================="
	cd COM/CapitalExecConnectionTCP && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                         SpeedyTW Pro"
	@echo "==============================================================="
	cd SpeedyTWPro && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                 Capital Stock Risk Manager 64"
	@echo "==============================================================="
	cd JSunStockRiskManager && make CONF=LINUX_Capital_Release_64
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
	@echo "                           TWEXLib"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                   Capital Exec Connection"
	@echo "==============================================================="
	cd COM/CapitalExecConnectionTCP && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                        SpeedyTW Pro"
	@echo "==============================================================="
	cd SpeedyTWPro && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                 Capital Stock Risk Manager 64"
	@echo "==============================================================="
	cd JSunStockRiskManager && make CONF=LINUX_Capital_Release_64 clean
	@echo "==============================================================="
	@echo
