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
	cd MBus && make -f Makefile_MBus.mk CONF=Solaris_GCC4.3.2_Release
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
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && make -f Makefile_SpeedyManager.mk CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           libMessageDeliver_jni"
	@echo "==============================================================="
	cd MessageDeliver_jni && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo		
#
	@echo
	@echo "                           LUA lib"
	@echo "==============================================================="
	cd Lua && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TAIFEXLib"
	@echo "==============================================================="
	cd TAIFEXLib && make CONF=Solaris_GCC4.3.2_Release_FCM
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                          SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && make CONF=Solaris_GCC4.3.2_Release
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                          SpeedyTMP"
	@echo "==============================================================="
	cd SpeedyTCPTAIFEX && make CONF=Solaris_GCC4.3.2_Release
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
	cd SpeedyManager && make -f Makefile_SpeedyManager.mk CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProfile"
	@echo "==============================================================="
	cd SpeedyProfile && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           libMessageDeliver_jni"
	@echo "==============================================================="
	cd MessageDeliver_jni && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo			
#
	@echo
	@echo "                           LUA lib"
	@echo "==============================================================="
	cd Lua && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           Set Seq Utility"
	@echo "==============================================================="
	cd SpeedySetSeqUtil && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Cancel All Utility"
	@echo "==============================================================="
	cd SpeedyCancelAll && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TAIFEXLib"
	@echo "==============================================================="
	cd TAIFEXLib && make CONF=Solaris_GCC4.3.2_Release_FCM clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                          SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                          SpeedyTMP"
	@echo "==============================================================="
	cd SpeedyTCPTAIFEX && make CONF=Solaris_GCC4.3.2_Release clean
	@echo "==============================================================="
	@echo
