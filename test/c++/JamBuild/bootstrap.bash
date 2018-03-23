#!/bin/bash
unzip jam-2.6.zip
cd jam-2.6
make
cp bin.linuxx86_64/jam ..
cp bin.linuxx86_64/mkjambase ..
cp Jam.html ..
cp Jambase.html ..
cp Jamfile.html ..
cd ..
rm -rf jam-2.6
