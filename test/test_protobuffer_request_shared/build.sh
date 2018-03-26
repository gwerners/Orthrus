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
#   tar -xzf protobuf-2.6.1.tar.gz
   cd protobuf-master
#   cd protobuf-2.6.1
   ./autogen.sh
   ./configure --prefix=${LIBRARIES}
   make
   make install
   cd ..
   rm -rf protobuf-master
fi
if [ -e "${LIBRARIES}/lib/libuv.a" ]
then
   echo "libuv exists..."
else
   echo "building libuv!"
   mkdir ${LIBRARIES}
   tar -xjf libuv-1.x.tar.bz2

   cd libuv-1.x
   sh autogen.sh
   ./configure --prefix=${LIBRARIES}
   make
   #uncomment next line to make long check on libuv...
   #make check
   make install
   cd ..
   rm -rf libuv-1.x
fi
${LIBRARIES}/bin/re2c --input custom libRest-1.0.re > libRest-1.0.cc
${LIBRARIES}/bin/protoc --cpp_out=. request.proto


${CC} -fPIC -shared -c request.pb.cc -I. -I${LIBRARIES}/include
${CC} -fPIC -shared -c libRest-1.0.cc -I. -I${LIBRARIES}/include
#-Wl,-rpath ${LIBRARIES}/lib/
${CC} -fPIC -shared libRest-1.0.o request.pb.o  -Wl,-rpath,./libraries/lib/ -L${LIBRARIES}/lib/ -lprotobuf -o libRest-1.0.so

${CC} -c main.cc -I. -I${LIBRARIES}/include
#-Wl,-rpath, sets the LD_LIBRARY_PATH inside the binary!
#se a lib nao estiver no path do rpath, tem que estar no path do sistema operacional:
#/usr/local/lib
#/usr/lib
${CC} -o client.exe main.o request.pb.o -Wl,-rpath,`pwd` -Wl,-rpath,${LIBRARIES}/lib ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -ldl -pthread

#ln -s libRest-1.0.so libRest.so


rm *.o
#rm request.pb.*
#export LD_LIBRARY_PATH=`pwd`
./client.exe request.book

#check if rpath is ok!
#objdump -x client.exe |grep RPATH
#objdump -x libRest-1.0.so |grep RPATH
