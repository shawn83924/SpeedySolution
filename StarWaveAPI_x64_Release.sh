Today=`date +%Y%m%d`
ReleaseFile="StarWaveAPI_x64_Release$Today.tgz"

echo "Remove file $ReleaseFile"
rm $ReleaseFile

make -f StarWaveAPI_x64.mk clean
make -f StarWaveAPI_x64.mk

cd Release
mkdir StarWaveAPI_x64
cd StarWaveAPI_x64

mkdir lib
cp /MBus/lib/libUFC_64.a lib
cp ../StarWaveAPI_64.a lib

mkdir include
mkdir include/UFC
mkdir include/Migo
mkdir include/StarWaveAPILib
mkdir include/StarWaveAPILib/MarketDataConnection
mkdir include/StarWaveAPILib/MarketDataMessage
cp ../../UFC/*.h include/UFC
cp ../../Migo/*.h include/Migo
cp ../../StarWaveAPILib/MarketDataConnection/*.h include/StarWaveAPILib/MarketDataConnection
cp ../../StarWaveAPILib/MarketDataMessage/*.h include/StarWaveAPILib/MarketDataMessage

cd ..

tar -zcvf $ReleaseFile StarWaveAPI_x64
rm -rf StarWaveAPI_x64
mv $ReleaseFile /Speedy/bin
