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
	@echo "                           UFC dynamic"
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
	@echo "                           TAIFEXLib Static Release"
	@echo "==============================================================="
	cd TAIFEXLib && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TSECLib Static Release"
	@echo "==============================================================="
	cd TSECLib && make -f Makefile_TSECLib.mk CONF=LINUX_Release_64
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
	@echo "                   UNI Exec Connection"
	@echo "==============================================================="
	cd COM/UniExecConnection && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   UNI TW RiskManager"
	@echo "==============================================================="
	cd UniTWRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   UNI RiskManager"
	@echo "==============================================================="
	cd COM/UniRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   UNI AS400Backend"
	@echo "==============================================================="
	cd UniAS400Backend && make CONF=LINUX_Release_64
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
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64 
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
#
	@echo
	@echo "                           KGI Risk Manager 64"
	@echo "==============================================================="
	cd KGIDRiskManager && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Stock Risk Manager 64"
	@echo "==============================================================="
	cd KGIStockRiskManager && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Lite Stock Risk Manager 64"
	@echo "==============================================================="
	cd KGILiteStockRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Barclays TSE Mapper"
	@echo "==============================================================="
	cd BarclaysTSEMapper && make CONF=Linux_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Citi TSE Mapper"
	@echo "==============================================================="
	cd CitiTSEMapper && make CONF=Linux_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Deutshe TSE Mapper"
	@echo "==============================================================="
	cd DeutsheBankMapper && make CONF=Linux_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Macquarie TSE Mapper"
	@echo "==============================================================="
	cd MacquarieTSEMapper && make CONF=Linux_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Ovata TSE Mapper"
	@echo "==============================================================="
	cd OvataTSEMapper && make CONF=Linux_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI DB Stock RiskManager"
	@echo "==============================================================="
	cd KGIDBStkRM && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Stock RiskManager"
	@echo "==============================================================="
	cd KGIStockRiskManager && make CONF=LINUX_Release_64
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
	@echo "                           DataStore Static Release"
	@echo "==============================================================="
	cd UFC/DataStore && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
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
	@echo "                           TAIFEXLib Static Release"
	@echo "==============================================================="
	cd TAIFEXLib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   TWEXLib"
	@echo "==============================================================="
	cd TWEXLib && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                   UNI Exec Connection"
	@echo "==============================================================="
	cd COM/UniExecConnection && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   UNI TW RiskManager"
	@echo "==============================================================="
	cd UniTWRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   UNI RiskManager"
	@echo "==============================================================="
	cd COM/UniRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   UNI AS400Backend"
	@echo "==============================================================="
	cd UniAS400Backend && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                   KGI Exec Connection"
	@echo "==============================================================="
	cd COM/KGIExecConnection && make CONF=LINUX_Release_64 clean
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
	@echo "                           SpeedyTW Pro"
	@echo "==============================================================="
	cd SpeedyTWPro && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SMP Recover"
	@echo "==============================================================="
	cd SMPRecover && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Risk Manager 64"
	@echo "==============================================================="
	cd KGIDRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Stock Risk Manager 64"
	@echo "==============================================================="
	cd KGIStockRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Lite Stock Risk Manager 64"
	@echo "==============================================================="
	cd KGILiteStockRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Barclays TSE Mapper"
	@echo "==============================================================="
	cd BarclaysTSEMapper && make CONF=Linux_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Citi TSE Mapper"
	@echo "==============================================================="
	cd CitiTSEMapper && make CONF=Linux_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Deutshe TSE Mapper"
	@echo "==============================================================="
	cd DeutsheBankMapper && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Macquarie TSE Mapper"
	@echo "==============================================================="
	cd MacquarieTSEMapper && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Ovata TSE Mapper"
	@echo "==============================================================="
	cd OvataTSEMapper && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI DB Stock RiskManager"
	@echo "==============================================================="
	cd KGIDBStkRM && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Stock RiskManager"
	@echo "==============================================================="
	cd KGIStockRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
