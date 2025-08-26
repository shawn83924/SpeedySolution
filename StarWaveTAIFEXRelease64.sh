#! /bin/bash

Today=`date +%Y%m%d`
ReleaseFile="StarWaveTAIFEXRelease_64_$Today.tgz"
echo "Remove file $ZIPFile"
rm $ZIPFile

make -f StarWaveTAIFEX64.mk clean
make -f StarWaveTAIFEX64.mk

#------Make Release package -------
cd /Speedy/bin
tar -zcvf $ReleaseFile MBus SpeedyAgent SpeedyManager SpeedyTop TSSAgent StarWave ChartServer TAIFEXSecurityDownload TAIFEXMDSource TAIFEXMySQLAdapter TickMySQLConnector LogCleaner
