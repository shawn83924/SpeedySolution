#! /bin/bash

Today=`date +%Y%m%d`
ReleaseFile="SpeedyAPILinux_32_64_Release$Today.tar"
ZIPFile="SpeedyAPILinux_32_64_Release$Today.tar.gz"
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
tar -cvf $ReleaseFile UFC/*.h Migo/*.h COM/SpeedyOrderConnection/*.h  COM/OrderConnection/*.h COM/Lib ExchangeUtility/*.h
gzip  $ReleaseFile
