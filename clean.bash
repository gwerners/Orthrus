#!/bin/bash
rm server.exe
rm server_jit.exe
rm -rf libraries
rm -rf BO
rm *.log
rm src/*.pb.h
rm src/*.pb.cc
rm ._LuaJIT*

cd BO_src/JAVA/USSD
./clean.bash
cd ../../..

cd BO_src/CPP/SMS
./clean.bash
cd ../../..

cd BO_src/CPP/BENCH
./clean.bash
cd ../../..
