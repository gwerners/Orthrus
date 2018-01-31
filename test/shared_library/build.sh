#!/bin/sh
CC=g++

CC=gcc
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

#OSX
#g++ -dynamiclib -flat_namespace library.cxx-o library.so
#linux
g++ -fPIC -shared library.cxx -o library.so

gcc -O3 -c client.c -I${LIBRARIES}/include
gcc -O3 -o client.exe client.o ${LIBRARIES}/lib/libuv.a -ldl -pthread
rm *.o

#export LD_LIBRARY_PATH=`pwd`
./client.exe
