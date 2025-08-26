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
	@echo "                           TWSEMDSource"
	@echo "==============================================================="
	cd TWSEMDSource && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TWSEMySQLAdapter"
	@echo "==============================================================="
	cd TWSEMySQLAdapter && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TAIFEXMDSource"
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
	@echo
	@echo "                          TickMySQLConnector"
	@echo "==============================================================="
	cd TickMySQLConnector && make CONF=LINUX_Release_64
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
#
	@echo
	@echo "                           ESEMDLib"
	@echo "==============================================================="
	cd ESEMDLib && make CONF=Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           ESMDSource"
	@echo "==============================================================="
	cd ESMDSource && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          ESMySQLAdapter"
	@echo "==============================================================="
	cd ESMySQLAdapter && make CONF=LINUX_Release_64
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
	@echo "                           TWSEMDSource"
	@echo "==============================================================="
	cd TWSEMDSource && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                          TWSEMySQLAdapter"
	@echo "==============================================================="
	cd TWSEMySQLAdapter && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
	@echo
#
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
	@echo
	@echo "                          TickMySQLConnector"
	@echo "==============================================================="
	cd TickMySQLConnector && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                         CTFSecurityDownload"
	@echo "==============================================================="
	cd CTFSecurityDownload && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                             CTFMDSource"
	@echo "==============================================================="
	cd CTFMDSource && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           ESEMDLib"
	@echo "==============================================================="
	cd ESEMDLib && make CONF=Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           ESMDSource"
	@echo "==============================================================="
	cd ESMDSource && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
# 
	@echo
	@echo "                          ESMySQLAdapter"
	@echo "==============================================================="
	cd ESMySQLAdapter && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#

