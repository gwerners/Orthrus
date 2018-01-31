#!/bin/ksh
PWD=`pwd`
source `pwd`/../../ksh/build_functions.ksh


CC=g++
CFLAGS="-O3"

LIBRARIES=`pwd`/../../libraries
OPEN_SOURCE=`pwd`/../../open_source

build "re2c"
build "protobuffer"
build "libuv"

cd ${PWD}

${LIBRARIES}/bin/re2c --input custom libM4UI-Rest-2.0.re > libM4UI-Rest-2.0.cc
${LIBRARIES}/bin/protoc --cpp_out=. request.proto


${CC} ${CFLAGS} -fPIC -shared -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -fPIC -shared -c logger.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -fPIC -shared -c libM4UI-Rest-2.0.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -fPIC -shared -c http_parser.c -I. -I${LIBRARIES}/include
#-Wl,-rpath ${LIBRARIES}/lib/
${CC} ${CFLAGS} -fPIC -shared http_parser.o libM4UI-Rest-2.0.o request.pb.o logger.o -Wl,-rpath,./libraries/lib/ -L${LIBRARIES}/lib/ -lprotobuf -o libM4UI-Rest-2.0.so

${CC} ${CFLAGS} -c main.cc -I. -I${LIBRARIES}/include
#-Wl,-rpath, sets the LD_LIBRARY_PATH inside the binary!
#se a lib nao estiver no path do rpath, tem que estar no path do sistema operacional:
#/usr/local/lib
#/usr/lib
${CC} ${CFLAGS} -o client.exe main.o request.pb.o logger.o -Wl,-rpath,`pwd` -Wl,-rpath,${LIBRARIES}/lib ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -ldl -pthread

#ln -s libM4UI-Rest-1.0.so libM4UI-Rest.so


rm *.o
#rm request.pb.*
#export LD_LIBRARY_PATH=`pwd`
./client.exe request.book

#check if rpath is ok!
#objdump -x client.exe |grep RPATH
#objdump -x libM4UI-Rest-1.0.so |grep RPATH
