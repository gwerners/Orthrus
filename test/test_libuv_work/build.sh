#!/bin/sh
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

${CC} -c main.c -I. -I${LIBRARIES}/include

${CC} -o client.exe main.o ${LIBRARIES}/lib/libuv.a -pthread



rm *.o

#export LD_LIBRARY_PATH=`pwd`
./client.exe
