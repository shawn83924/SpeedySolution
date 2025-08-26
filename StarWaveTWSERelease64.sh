#! /bin/bash


Today=`date +%Y%m%d`
ReleaseFile="StarWaveTWSERelease_64_$Today.tgz"
echo "Remove file $ZIPFile"
rm $ZIPFile

make -f StarWaveTWSE64.mk clean
make -f StarWaveTWSE64.mk

#------Make Release package -------
cd /Speedy/bin
tar -zcvf $ReleaseFile MBus SpeedyAgent SpeedyManager SpeedyTop TSSAgent StarWave ChartServer  TWSEMDSource TWSEMySQLAdapter
