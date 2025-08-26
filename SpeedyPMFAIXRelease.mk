###########################################################################
default:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && gmake -f Makefile_UFC.mk
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           MBus"
	@echo "==============================================================="
	cd MBus && gmake -f Makefile_MBus.mk
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && gmake -f Makefile_SpeedyAgent.mk
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && gmake -f Makefile_SpeedyManager.mk
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && gmake 
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           libMessageDeliver_jni"
	@echo "==============================================================="
	cd MessageDeliver_jni && gmake 
	@echo "==============================================================="
	@echo		
#
	@echo
	@echo "                           LUA lib"
	@echo "==============================================================="
	cd Lua && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && gmake 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TAIFEXLib"
	@echo "==============================================================="
	cd TAIFEXLib && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && gmake 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && gmake 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTMP"
	@echo "==============================================================="
	cd SpeedyTCPTAIFEX && gmake 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                  PolarisMF MBus I/O(For HoMai)"
	@echo "==============================================================="
	cd PMFMBusFileIO && gmake 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "              PolarisMF Order Connection(For HoMai)"
	@echo "==============================================================="
	cd COM/SpeedyFIFOOrderConnection && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "              PolarisMF Executions to file(For HoMai)"
	@echo "==============================================================="
	cd COM/PMFExecToFile && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                 PolarisMF Risk Manager(For HoMai)"
	@echo "==============================================================="
	cd COM/SpeedyFIFORiskManager && gmake
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "              PolarisMF Executions to backend(Tandem)"
	@echo "==============================================================="
	cd COM/PMFExecConnection && gmake
	@echo "==============================================================="
	@echo

all: default

###########################################################################
clean:
	@echo
	@echo "                           UFC"
	@echo "==============================================================="
	cd UFC && gmake -f Makefile_UFC.mk clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           MBus"
	@echo "==============================================================="
	cd MBus && gmake -f Makefile_MBus.mk clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTop"
	@echo "==============================================================="
	cd SpeedyTop && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && gmake -f Makefile_SpeedyAgent.mk clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && gmake -f Makefile_SpeedyManager.mk clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && gmake clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           libMessageDeliver_jni"
	@echo "==============================================================="
	cd MessageDeliver_jni && gmake clean
	@echo "==============================================================="
	@echo		
#
	@echo
	@echo "                           LUA lib"
	@echo "==============================================================="
	cd Lua && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TAIFEXLib"
	@echo "==============================================================="
	cd TAIFEXLib && gmake clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyTMP"
	@echo "==============================================================="
	cd SpeedyTCPTAIFEX && gmake clean 
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                  PolarisMF MBus I/O(For HoMai)"
	@echo "==============================================================="
	cd PMFMBusFileIO && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "              PolarisMF Order Connection(For HoMai)"
	@echo "==============================================================="
	cd COM/SpeedyFIFOOrderConnection && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "              PolarisMF Executions to file(For HoMai)"
	@echo "==============================================================="
	cd COM/PMFExecToFile && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                 PolarisMF Risk Manager(For HoMai)"
	@echo "==============================================================="
	cd COM/SpeedyFIFORiskManager && gmake clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "              PolarisMF Executions to backend(Tandem)"
	@echo "==============================================================="
	cd COM/PMFExecConnection && gmake clean
	@echo "==============================================================="
	@echo
