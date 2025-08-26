###########################################################################
default:
	@echo
	@echo "                           UFC Static"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UFC Dynamic"
	@echo "==============================================================="
	cd UFCD && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           DataStore Static"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           DataStore Static Debug"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Debug_64
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
	cd SpeedyProfile && make CONF=LINUX_Release_64
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
	@echo "                           SpeedyAPI Static"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAPI Static Debug"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Debug_64
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
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && make CONF=UNICA_LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTMP"
	@echo "==============================================================="
	cd SpeedyTCPTAIFEX && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UNI Exec Connection"
	@echo "==============================================================="
	cd COM/UniExecConnection && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Uni TW RiskManager"
	@echo "==============================================================="
	cd UniTWRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniRiskManager"
	@echo "==============================================================="
	cd COM/UniRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                            UNI AS400Backend"
	@echo "==============================================================="
	cd UniAS400Backend && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
all: default

###########################################################################
clean:
	@echo
	@echo "                           UFC Static"
	@echo "==============================================================="
	cd UFC && make -f Makefile_UFC.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UFC Dynamic"
	@echo "==============================================================="
	cd UFCD && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           DataStore Static"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           DataStore Static Debug"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Debug_64 clean
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
	@echo "                           SpeedyAPI Static"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAPI Static Debug"
	@echo "==============================================================="
	cd COM/Lib && make CONF=LINUX_Debug_64 clean
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
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && make CONF=UNICA_LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTMP"
	@echo "==============================================================="
	cd SpeedyTCPTAIFEX && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UNI Exec Connection"
	@echo "==============================================================="
	cd COM/UniExecConnection && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Uni TW RiskManager"
	@echo "==============================================================="
	cd UniTWRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniRiskManager"
	@echo "==============================================================="
	cd COM/UniRiskManager CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                            UNI AS400Backend"
	@echo "==============================================================="
	cd UniAS400Backend && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
