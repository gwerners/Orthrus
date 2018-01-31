#!/bin/sh
CC=g++
LIBRARIES=`pwd`/libraries

export PREFIX=${LIBRARIES}

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
${CC} -o server.exe main.o request.pb.o ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -pthread
${CC} -c worker.cc -I. -I${LIBRARIES}/include
${CC} -o worker.exe worker.o request.pb.o ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -pthread



rm *.o

#export LD_LIBRARY_PATH=`pwd`
./server.exe


#http://stackoverflow.com/questions/9496101/protocol-buffer-over-socket-in-c
#http://www.cplusplus.com/forum/general/58677/
#https://groups.google.com/forum/#!topic/protobuf/78RgOtp2rqM
#http://docs.biicode.com/c++/examples/zmq_protobuf.html
#person.ParseFromString(msg_str);
#https://famellee.wordpress.com/2013/04/29/transmit-multiple-google-protocol-buffers-messages-between-java-and-c/






