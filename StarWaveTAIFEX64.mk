###########################################################################
default:
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
	@echo "                           TSSAgent"
	@echo "==============================================================="
	cd TSSAgent && make  CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "				  StarWave"
	@echo "==============================================================="
	cd StarWaveMarketDataServer && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "                           ChartServer"
	@echo "==============================================================="
	cd ChartServer && make CONF=LINUX_Release_64
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
	@echo "                           TAIFEXMDSource"
	@echo "==============================================================="
	cd TAIFEXMDSource && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#	
	@echo "                           TAIFEXSecurityDowbload"
	@echo "==============================================================="
	cd TAIFEXSecurityDownload && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TAIFEXMySQLAdapter"
	@echo "==============================================================="
	cd TAIFEXMySQLAdapter && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#

all: default

###########################################################################
clean:
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
	@echo "                           TSSAgent"
	@echo "==============================================================="
	cd TSSAgent && make  CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo	
#
	@echo
	@echo "				  StarWave"
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
	@echo "                           LogCleaner"
	@echo "==============================================================="
	cd LogCleaner && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TAIFEXMDSource"
	@echo "==============================================================="
	cd TAIFEXMDSource && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#	
	@echo "                           TAIFEXSecurityDowbload"
	@echo "==============================================================="
	cd TAIFEXSecurityDownload && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TAIFEXMySQLAdapter"
	@echo "==============================================================="
	cd TAIFEXMySQLAdapter && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
