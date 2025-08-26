#! /usr/bin/bash


Today=`date +%Y%m%d`
ReleaseFile="SpeedyAPISolarisRelease$Today.tar"
ZIPFile="SpeedyAPISolarisRelease$Today.tar.gz"
echo "Remove file $ZIPFile"
rm $ZIPFile
#------Make libUFC.a -------
cd UFC
make -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release clean
make -f Makefile_UFC.mk CONF=Solaris_GCC4.3.2_Release
cd ../
cp /MBus/lib/libUFC.a  COM/Lib/.

#------Make libSpeedyAPI.a -------
cd COM/Lib
make CONF=Solaris_GCC4.3.2_Release clean
make CONF=Solaris_GCC4.3.2_Release
cd ../../

#------Make sample code -------
cd COM/SpeedyOrderConnection
make CONF=Solaris_GCC4.3.2_Release clean
make CONF=Solaris_GCC4.3.2_Release
cd ../../

#------Make Release package -------

tar -cvf $ReleaseFile UFC/*.h Migo/*.h COM/SpeedyOrderConnection COM/OrderConnection COM/Lib/MarketDataConnection/*.h COM/Lib/MarketDataMessage/*.h COM/Lib/MessageDeliver/*.h COM/Lib/OrderConnection/*.h COM/Lib/OrderMessage/*.h COM/Lib/libUFC.a COM/Lib/libSpeedyAPI.a COM/Lib/*.h ExchangeUtility

gzip  $ReleaseFile
