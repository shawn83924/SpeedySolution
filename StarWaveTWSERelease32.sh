#! /bin/bash


Today=`date +%Y%m%d`
ReleaseFile="StarWaveTWSERelease_32_$Today.tgz"
echo "Remove file $ZIPFile"
rm $ZIPFile

make -f StarWaveTWSE_32.mk clean
make -f StarWaveTWSE_32.mk

#------Make Release package -------
cd /Speedy/bin
tar -zcvf $ReleaseFile MBus SpeedyAgent SpeedyManager SpeedyTop TSSAgent StarWave ChartServer  TWSEMDSource TWSEMySQLAdapter
