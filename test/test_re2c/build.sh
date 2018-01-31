#!/bin/sh
CC=gcc
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

lua generate.lua > main.re

${LIBRARIES}/bin/re2c --input custom main.re > main.c
#${LIBRARIES}/bin/re2c main.re > main.c

${CC} -c main.c -I. -I${LIBRARIES}/include

${CC} -o client.exe main.o 




rm *.o

#export LD_LIBRARY_PATH=`pwd`
./client.exe
