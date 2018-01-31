#!/bin/sh
rm server.exe
rm server_jit.exe
rm -rf libraries
rm -rf BO
rm *.log
rm src/*.pb.h
rm src/*.pb.cc

cd BO_src/JAVA/USSD
./clean.ksh
cd ../../..

cd BO_src/CPP/SMS
./clean.ksh
cd ../../..

cd BO_src/CPP/BENCH
./clean.ksh
cd ../../..
