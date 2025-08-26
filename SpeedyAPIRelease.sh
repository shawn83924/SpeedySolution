#! /bin/bash


Today=`date +%Y%m%d`
ReleaseFile="SpeedyAPILinuxRelease$Today.tar.gz"
echo "Remove file $ReleaseFile"
rm $ReleaseFile
cd COM/Lib
make CONF=LINUX_Release clean
make CONF=LINUX_Release 

make CONF=LINUX_Release_64 clean
make CONF=LINUX_Release_64
 
cd ../../
cp /MBus/lib/libUFC.a  COM/Lib/.
cp /MBus/lib/libUFC_64.a  COM/Lib/.

tar zcvf $ReleaseFile UFC/*.h Migo/*.h COM/SpeedyOrderConnection COM/OrderConnection COM/Lib/MarketDataConnection/*.h COM/Lib/MarketDataMessage/*.h COM/Lib/MessageDeliver/*.h COM/Lib/OrderConnection/*.h COM/Lib/OrderMessage/*.h COM/Lib/libUFC.a COM/Lib/libUFC_64.a COM/Lib/libSpeedyAPI.a COM/Lib/libSpeedyAPI_64.a COM/Lib/*.h ExchangeUtility

