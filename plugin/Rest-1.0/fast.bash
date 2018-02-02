#!/bin/bash
PWD=`pwd`
source `pwd`/../../bash/build_functions.bash


CC=g++
LIBRARIES=`pwd`/../../libraries
OPEN_SOURCE=`pwd`/../../open_source

build "re2c"
build "protobuffer"
build "libuv"

cd ${PWD}

${LIBRARIES}/bin/re2c --input custom libM4UI-Rest-1.0.re > libM4UI-Rest-1.0.cc
${LIBRARIES}/bin/protoc --cpp_out=. request.proto


${CC} -fPIC -shared -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} -fPIC -shared -c libM4UI-Rest-1.0.cc -I. -I${LIBRARIES}/include

#-Wl,-rpath ${LIBRARIES}/lib/
${CC} -fPIC -shared libM4UI-Rest-1.0.o request.pb.o -Wl,-rpath,./libraries/lib/ -L${LIBRARIES}/lib/ -lprotobuf -o libM4UI-Rest-1.0.so

#${CC} -c main.cc -I. -I${LIBRARIES}/include
#-Wl,-rpath, sets the LD_LIBRARY_PATH inside the binary!
#se a lib nao estiver no path do rpath, tem que estar no path do sistema operacional:
#/usr/local/lib
#/usr/lib
#${CC} -o client.exe main.o request.pb.o -Wl,-rpath,`pwd` -Wl,-rpath,${LIBRARIES}/lib ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -ldl -pthread

#ln -s libM4UI-Rest-1.0.so libM4UI-Rest.so


rm *.o
#rm request.pb.*
#export LD_LIBRARY_PATH=`pwd`
#./client.exe request.book

#check if rpath is ok!
#objdump -x client.exe |grep RPATH
#objdump -x libM4UI-Rest-1.0.so |grep RPATH
