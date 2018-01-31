#!/bin/sh
CC=g++
LIBRARIES=`pwd`/libraries

export PREFIX=${LIBRARIES}

#re2c-0.14.3

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

${CC} -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} -c main.cc -I. -I${LIBRARIES}/include
${CC} -o client.exe main.o request.pb.o ${LIBRARIES}/lib/libprotobuf.a




rm *.o
#rm request.pb.*
#export LD_LIBRARY_PATH=`pwd`
./client.exe request.book
