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
	@echo "                           SpiderNetEngine Static Release"
	@echo "==============================================================="
	cd SpiderNetEngine && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpiderNetEngineD Dynamic Release"
	@echo "==============================================================="
	cd SpiderNetEngineD && make CONF=LINUX_Release_64
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
	@echo "                           SMP Client"
	@echo "==============================================================="
	cd SMPClient && make
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
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpiderNet"
	@echo "==============================================================="
	cd SpiderNet && make -f SpiderNet-Makefile.mk CONF=LINUX_Release_64 
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
	@echo "                           Barclays TSE Mapper D2"
	@echo "==============================================================="
	cd BarclaysTSEMapperD2 && make CONF=Linux_Release_64 
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
#
	@echo
	@echo "                           KGI TWSE Mapper"
	@echo "==============================================================="
	cd KGITWSEMapper && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Macquarie Mapper"
	@echo "==============================================================="
	cd KGIMacquarieMapper && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Citi Mapper"
	@echo "==============================================================="
	cd KGICitiMapper && make CONF=LINUX_Release_64 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Deutshe Bank Mapper"
	@echo "==============================================================="
	cd KGIDeutsheBankMapper && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Barclays Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperBarclays && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Deutshe Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperDeutshe && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Citi Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperCiti && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Macquarie Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperMacquarie && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI Ovata Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperOvata && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           KGI TWSE RiskManagerD"
	@echo "==============================================================="
	cd KGITWSERiskManagerD && make CONF=LINUX_Release_64
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
	@echo "                           Clean SpiderNetEngine Static Release"
	@echo "==============================================================="
	cd SpiderNetEngine && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpiderNetEngineD Dynamic Release"
	@echo "==============================================================="
	cd SpiderNetEngineD && make CONF=LINUX_Release_64 clean
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
	@echo "                          Clean Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          Clean Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && make CONF=LINUX_Release_64 clean
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
	@echo "                           Clean SMP Client"
	@echo "==============================================================="
	cd SMPClient && make clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SMP Recover"
	@echo "==============================================================="
	cd SMPRecover && make CONF=LINUX_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean SpiderNet"
	@echo "==============================================================="
	cd SpiderNet && make -f SpiderNet-Makefile.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Risk Manager 64"
	@echo "==============================================================="
	cd KGIDRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Stock Risk Manager 64"
	@echo "==============================================================="
	cd KGIStockRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Lite Stock Risk Manager 64"
	@echo "==============================================================="
	cd KGILiteStockRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean Barclays TSE Mapper"
	@echo "==============================================================="
	cd BarclaysTSEMapper && make CONF=Linux_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean Barclays TSE Mapper D2"
	@echo "==============================================================="
	cd BarclaysTSEMapperD2 && make CONF=Linux_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean Citi TSE Mapper"
	@echo "==============================================================="
	cd CitiTSEMapper && make CONF=Linux_Release_64 clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean Deutshe TSE Mapper"
	@echo "==============================================================="
	cd DeutsheBankMapper && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean Macquarie TSE Mapper"
	@echo "==============================================================="
	cd MacquarieTSEMapper && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean Ovata TSE Mapper"
	@echo "==============================================================="
	cd OvataTSEMapper && make CONF=Linux_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI DB Stock RiskManager"
	@echo "==============================================================="
	cd KGIDBStkRM && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Stock RiskManager"
	@echo "==============================================================="
	cd KGIStockRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI TWSE Mapper"
	@echo "==============================================================="
	cd KGITWSEMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Macquarie Mapper"
	@echo "==============================================================="
	cd KGIMacquarieMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Citi Mapper"
	@echo "==============================================================="
	cd KGICitiMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Deutshe Bank Mapper"
	@echo "==============================================================="
	cd KGIDeutsheBankMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Barclays Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperBarclays && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Deutshe Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperDeutshe && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Citi Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperCiti && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Macquarie Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperMacquarie && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI Ovata Bank MT Mapper"
	@echo "==============================================================="
	cd KGIMTMapperOvata && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Clean KGI TWSE RiskManagerD"
	@echo "==============================================================="
	cd KGITWSERiskManagerD && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
