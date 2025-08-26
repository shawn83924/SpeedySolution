#! /bin/bash


Today=`date +%Y%m%d`
ReleaseFile="SpeedyAPIAIXRelease$Today.tar"
ZIPFile="SpeedyAPIAIXRelease$Today.tar.gz"
echo "Remove file $ZIPFile"
rm $ZIPFile
#------Make libUFC.a -------
cd UFC
gmake -f Makefile_UFC.mk clean
gmake -f Makefile_UFC.mk 
cd ../
cp /MBus/lib/UFC.a  COM/Lib/.

#------Make libSpeedyAPI.a -------
cd COM/Lib
gmake clean
gmake 
cd ../../

#------Make sample code -------
cd COM/SpeedyOrderConnection
gmake clean
gmake 
cd ../../

#------Make Release package -------
tar -cvf $ReleaseFile UFC/*.h Migo/*.h COM/OrderConnection COM/Lib ExchangeUtility COM/SpeedyOrderConnection
gzip  $ReleaseFile
