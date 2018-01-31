#!/bin/sh
CC=g++
LIBRARIES=`pwd`/libraries

export PREFIX=${LIBRARIES}

#re2c-0.14.3

if [ -e "${LIBRARIES}/bin/re2c" ]
then
   echo "re2c exists..."
else
   echo "building re2c!"
   mkdir ${LIBRARIES}
   tar -xzf re2c-0.14.3.tar.gz

   cd re2c-0.14.3
   ./configure --prefix=${LIBRARIES}
   make
   make install
   cd ..
   rm -rf re2c-0.14.3
fi
if [ -e "${LIBRARIES}/bin/protoc" ]
then
   echo "protoc exists..."
else
   echo "building protobuffer!"
   mkdir ${LIBRARIES}
   tar -xjf protobuf-master.tar.bz2
   cd protobuf-master
   ./autogen.sh
   ./configure --prefix=${LIBRARIES}
   make
   make install
   cd ..
   rm -rf protobuf-master
fi


${LIBRARIES}/bin/protoc --cpp_out=. request.proto

lua generate.lua > main.re

${LIBRARIES}/bin/re2c --input custom main.re > main.cc

${CC} -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} -c main.cc -I. -I${LIBRARIES}/include
${CC} -o client.exe main.o request.pb.o ${LIBRARIES}/lib/libprotobuf.a


rm *.o

./client.exe



