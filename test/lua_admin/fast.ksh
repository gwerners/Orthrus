#!/bin/ksh
PWD=`pwd`
LIBRARIES=../../libraries
OPEN_SOURCE=../../open_source
source ../../ksh/build_functions.ksh ${LIBRARIES} ${OPEN_SOURCE}
CC=g++
CFLAGS="-O3"


build "luajit"

cd ${PWD}


${CC} ${CFLAGS} -c lua_bench.cc -I. -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0
${CC} ${CFLAGS} -o bench.exe lua_bench.o ${LIBRARIES}/lib/libluajit-5.1.a -ldl -pthread

rm *.o

./bench.exe
