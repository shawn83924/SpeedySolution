#! /bin/bash


Today=`date +%Y%m%d`
ReleaseFile="MBusAPI_Linux_i386_Release_$Today.tgz"

echo "Remove file $ReleaseFile"
rm $ReleaseFile

make -f MBusAPI.mk clean
make -f MBusAPI.mk

cd Release
mkdir MBusAPI
cd MBusAPI

mkdir lib
cp /MBus/lib/libUFC.a lib

mkdir include
mkdir include/UFC
mkdir include/Migo

cp ../../UFC/*.h include/UFC
cp ../../Migo/*.h include/Migo
cd ..

tar -zcvf $ReleaseFile MBusAPI
rm -rf MBusAPI
mv $ReleaseFile /Speedy/bin
