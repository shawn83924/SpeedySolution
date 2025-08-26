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
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && make -f Makefile_SpeedyManager.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           Star Wave"
	@echo "==============================================================="
	cd StarWaveMarketDataServer && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          Chart Server"
	@echo "==============================================================="
	cd ChartServer && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                     Trading Session Stat Agent"
	@echo "==============================================================="
	cd TSSAgent && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       TickMySQLConnector"
	@echo "==============================================================="
	cd TickMySQLConnector && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                     TAIFEXSecurityDownload"
	@echo "==============================================================="
	cd TAIFEXSecurityDownload && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       TAIFEXMDSource"
	@echo "==============================================================="
	cd TAIFEXMDSource && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       TAIFEXMySQLAdapter"
	@echo "==============================================================="
	cd TAIFEXMySQLAdapter && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                     CTFSecurityDownload"
	@echo "==============================================================="
	cd CTFSecurityDownload && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       CTFMDSource"
	@echo "==============================================================="
	cd CTFMDSource && make CONF=LINUX_Release_64
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
	@echo "                           SpeedyManager"
	@echo "==============================================================="
	cd SpeedyManager && make -f Makefile_SpeedyManager.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           StarWave"
	@echo "==============================================================="
	cd StarWaveMarketDataServer && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           ChartServer"
	@echo "==============================================================="
	cd ChartServer && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TSSAgent"
	@echo "==============================================================="
	cd TSSAgent && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       TickMySQLConnector"
	@echo "==============================================================="
	cd TickMySQLConnector && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                     TAIFEXSecurityDownload"
	@echo "==============================================================="
	cd TAIFEXSecurityDownload && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       TAIFEXMDSource"
	@echo "==============================================================="
	cd TAIFEXMDSource && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       TAIFEXMySQLAdapter"
	@echo "==============================================================="
	cd TAIFEXMySQLAdapter && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                     CTFSecurityDownload"
	@echo "==============================================================="
	cd CTFSecurityDownload && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                       CTFMDSource"
	@echo "==============================================================="
	cd CTFMDSource && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
