
#! /bin/bash

Today=`date +%Y%m%d`
ReleaseFile="StarWaveRelease_x64_$Today.tgz"
cd /home/yuan/src/SpeedySolution/Simulator

make -f StarWave_TWSE_TFX_x64.mk clean
make -f StarWave_TWSE_TFX_x64.mk

#------Make Release package -------
cd /Speedy/

echo "Remove file $ZIPFile"
rm $ZIPFile

tar -zcvf $ReleaseFile bin/MBus bin/SpeedyAgent bin/SpeedyManager bin/SpeedyTop bin/TSSAgent bin/StarWave bin/ChartServer bin/TAIFEXSecurityDownload bin/TAIFEXMDSource bin/TAIFEXMySQLAdapter bin/TickMySQLConnector bin/TWSEMDSource bin/TWSEMySQLAdapter bin/LogCleaner bin/CTFMDSource bin/CTFSecurityDownload cfg/ExchangeInfo.ini lib lib64
echo "Release file $ZIPFile"

