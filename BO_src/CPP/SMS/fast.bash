#!/bin/bash
PWD=`pwd`
source `pwd`/../../../bash/build_functions.bash
CC=g++
CFLAGS="-O3"
LIBRARIES=`pwd`/../../../libraries
OPEN_SOURCE=`pwd`/../../../open_source

build "protobuffer"
build "libuv"

cd ${PWD}

${LIBRARIES}/bin/protoc --cpp_out=. request.proto
${LIBRARIES}/bin/protoc --cpp_out=. response.proto

${CC} ${CFLAGS} -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c response.pb.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c worker.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c logger.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -o worker.exe worker.o logger.o request.pb.o response.pb.o ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -pthread

rm *.o
