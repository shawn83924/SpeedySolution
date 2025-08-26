#!/bin/bash


Today=`date +%Y%m%d`
ZIPFile="SpeedyAPILinux68_32_64_Release$Today.tgz"
echo "Remove file $ZIPFile"
rm $ZIPFile
#------Make libUFC.a -------
cd UFC
make -f Makefile_UFC.mk CONF=LINUX_Release_64 clean
make -f Makefile_UFC.mk CONF=LINUX_Release_64

make -f Makefile_UFC.mk CONF=LINUX_Release clean
make -f Makefile_UFC.mk CONF=LINUX_Release
cd ../
cp /MBus/lib/libUFC_64.a  COM/Lib/.
cp /MBus/lib/libUFC.a  COM/Lib/.

#------Make libSpeedyAPI.a -------
cd COM/Lib
make CONF=LINUX_Release_64 clean
make CONF=LINUX_Release_64
make CONF=LINUX_Release clean
make CONF=LINUX_Release
cd ../../

cp /MBus/lib/libSpeedyAPI_64.a COM/Lib/.
cp /MBus/lib/libSpeedyAPI.a  COM/Lib/.

#------Make sample code -------
cd COM/SpeedyOrderConnection
make clean
make 
cd ../../

#------Make Release package -------
tar zcvf $ZIPFile UFC/*.h Migo/*.h UFC/include COM/SpeedyOrderConnection COM/OrderConnection COM/Lib/MarketDataConnection/*.h COM/Lib/MarketDataMessage/*.h COM/Lib/MessageDeliver/*.h COM/Lib/OrderConnection/*.h COM/Lib/OrderMessage/*.h COM/Lib/libUFC.a COM/Lib/libUFC_64.a COM/Lib/libSpeedyAPI.a COM/Lib/libSpeedyAPI_64.a COM/Lib/*.h COM/Lib/CA/*.h ExchangeUtility COM/SpeedyAPIXE7/*.h COM/SpeedyAPIXE7/*.ini
