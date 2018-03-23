#!/bin/sh
CC=g++
LIBRARIES=`pwd`/libraries

export PREFIX=${LIBRARIES}

if [ -e "${LIBRARIES}/lib/libuv.a" ]
then
   echo "libuv exists..."
else
   echo "building libuv!"
   git clone https://github.com/libuv/libuv.git
   mkdir ${LIBRARIES}
   cd libuv
   sh autogen.sh
   ./configure --prefix=${LIBRARIES}
   make
   make html
   #uncomment next line to make long check on libuv...
   #make check
   make install
   cd ..
   #rm -rf libuv-1.x
fi

${CC} -c main.cpp -I. -I${LIBRARIES}/include
${CC} -o worker.exe main.o ${LIBRARIES}/lib/libuv.a -pthread
rm *.o

#export LD_LIBRARY_PATH=`pwd`
./worker.exe
