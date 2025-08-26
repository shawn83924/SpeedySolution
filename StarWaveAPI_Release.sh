#! /bin/bash


Today=`date +%Y%m%d`
ReleaseFile="StarWaveAPI_Linux_i386_Release_$Today.tgz"

echo "Remove file $ReleaseFile"
rm $ReleaseFile

make -f StarWaveAPI.mk clean
make -f StarWaveAPI.mk

cd Release
mkdir StarWaveAPI
cd StarWaveAPI

mkdir lib
cp /MBus/lib/libUFC.a lib
cp ../StarWaveAPI.a lib

mkdir include
mkdir include/UFC
mkdir include/Migo
mkdir include/StarWaveAPILib
mkdir include/StarWaveAPILib/MarketDataConnection
mkdir include/StarWaveAPILib/MarketDataMessage
mkdir include/StarWaveAPILib/ChartServerConnection

cp ../../UFC/*.h include/UFC
cp ../../Migo/*.h include/Migo
cp ../../StarWaveAPILib/MarketDataConnection/*.h include/StarWaveAPILib/MarketDataConnection
cp ../../StarWaveAPILib/MarketDataMessage/*.h include/StarWaveAPILib/MarketDataMessage
cp ../../StarWaveAPILib/ChartServerConnection/*.h include/StarWaveAPILib/ChartServerConnection
cd ..

tar -zcvf $ReleaseFile StarWaveAPI
rm -rf StarWaveAPI
mv $ReleaseFile /Speedy/bin
