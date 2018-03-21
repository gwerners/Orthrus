#!/bin/bash
PWD=`pwd`
LIBRARIES=../../../libraries
OPEN_SOURCE=../../../open_source
source ../../../bash/build_functions.bash ${LIBRARIES} ${OPEN_SOURCE}
CC=g++
CFLAGS="-O3"


build "protobuffer"
build "libuv"
build "luajit"

cd ${PWD}

${LIBRARIES}/bin/protoc --cpp_out=. request.proto
${LIBRARIES}/bin/protoc --cpp_out=. response.proto

${CC} ${CFLAGS} -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c response.pb.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c worker.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c logger.cc -I. -I${LIBRARIES}/include
${CC} ${CFLAGS} -c lua_utils.cc -I. -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0
${CC} ${CFLAGS} -o worker.exe worker.o logger.o lua_utils.o request.pb.o response.pb.o ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a ${LIBRARIES}/lib/libluajit-5.1.a -ldl -pthread

rm *.o

#./worker.exe
