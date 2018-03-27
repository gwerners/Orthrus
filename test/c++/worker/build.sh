#!/bin/sh
CC=gcc
CFLAGS=-O3
LIBEVENT=libevent-2.1.8-stable
LIBRARIES=`pwd`/libraries
rm *.exe
rm *.o

export PREFIX=${LIBRARIES}
if [ ! -e "${LIBRARIES}/lib/libuv.a" ]; then
  tar -xzf ${LIBEVENT}.tar.gz
  cd ${LIBEVENT}
  ./configure --prefix=$LIBRARIES
  make
  make install
  cd ..
  rm -rf $LIBEVENT
fi
${CC} $CFLAGS -c main.c -I. -I${LIBRARIES}/include
${CC} $CFLAGS -o worker.exe main.o ${LIBRARIES}/lib/libevent.a -pthread
rm *.o

##export LD_LIBRARY_PATH=`pwd`
./worker.exe www
