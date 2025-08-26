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
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make  CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && make  CONF=WLSCA_LINUX_Release_64
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
	@echo "                           SpiderNet"
	@echo "==============================================================="
	cd SpiderNet && make -f SpiderNet-Makefile.mk CONF=LINUX_Release_64
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
	@echo "                           ESunnyMapper"
	@echo "==============================================================="
	cd ESunnyMapper && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           NTTMapper"
	@echo "==============================================================="
	cd NTTMapper && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           OverseasExecutionLogger"
	@echo "==============================================================="
	cd OverseasExecutionLogger && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           PATSFIXMapper"
	@echo "==============================================================="
	cd PATSFIXMapper && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           ProxyPasswordGateway"
	@echo "==============================================================="
	cd ProxyPasswordGateway && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TTMktDataBroadcaster"
	@echo "==============================================================="
	cd TTMktDataBroadcaster && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TTSecurityConverter"
	@echo "==============================================================="
	cd TTSecurityConverter && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniAS400Gateway"
	@echo "==============================================================="
	cd UniAS400Gateway && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniConvertPKI"
	@echo "==============================================================="
	cd UniConvertPKI && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniFVGateway"
	@echo "==============================================================="
	cd UniFVGateway && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniFVServerSimulator"
	@echo "==============================================================="
	cd UniFVServerSimulator && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniOpenGateway"
	@echo "==============================================================="
	cd UniOpenGateway && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniOSRiskManager"
	@echo "==============================================================="
	cd UniOSRiskManager && make CONF=LINUX_Release_64
	@echo "==============================================================="
	@echo
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
	@echo "                           SpeedyAgent"
	@echo "==============================================================="
	cd SpeedyAgent && make -f Makefile_SpeedyAgent.mk CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyGateway"
	@echo "==============================================================="
	cd SpeedyGateway && make  CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           SpeedyProxy"
	@echo "==============================================================="
	cd SpeedyProxy && make  CONF=WLSCA_LINUX_Release_64 clean
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
	@echo "                           SpiderNet"
	@echo "==============================================================="
	cd SpiderNet && make -f SpiderNet-Makefile.mk CONF=LINUX_Release_64 clean
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
	@echo "                           ESunnyMapper"
	@echo "==============================================================="
	cd ESunnyMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           NTTMapper"
	@echo "==============================================================="
	cd NTTMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           OverseasExecutionLogger"
	@echo "==============================================================="
	cd OverseasExecutionLogger && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           PATSFIXMapper"
	@echo "==============================================================="
	cd PATSFIXMapper && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           ProxyPasswordGateway"
	@echo "==============================================================="
	cd ProxyPasswordGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TTMktDataBroadcaster"
	@echo "==============================================================="
	cd TTMktDataBroadcaster && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           TTSecurityConverter"
	@echo "==============================================================="
	cd TTSecurityConverter && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniAS400Gateway"
	@echo "==============================================================="
	cd UniAS400Gateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniConvertPKI"
	@echo "==============================================================="
	cd UniConvertPKI && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniFVGateway"
	@echo "==============================================================="
	cd UniFVGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniFVServerSimulator"
	@echo "==============================================================="
	cd UniFVServerSimulator && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniOpenGateway"
	@echo "==============================================================="
	cd UniOpenGateway && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo
#
	@echo
	@echo "                           UniOSRiskManager"
	@echo "==============================================================="
	cd UniOSRiskManager && make CONF=LINUX_Release_64 clean
	@echo "==============================================================="
	@echo

