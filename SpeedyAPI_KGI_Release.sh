#! /usr/bin/bash


Today=`date +%Y%m%d`
ReleaseFile="SpeedyAPIKGISolarisRelease$Today.tar"
ReleaseGzFile="SpeedyAPIKGISolarisRelease$Today.tar.gz"
echo "Remove file $ReleaseFile"
rm $ReleaseFile
rm $ReleaseGzFile
cd COM/Lib
make clean
make
cd ..
cd KGIExecConnection
make clean
make
cd ../../
tar -cvf $ReleaseFile UFC/*.h Migo/*.h COM/KGIExecConnection COM/OrderConnection lib
gzip  $ReleaseFile
