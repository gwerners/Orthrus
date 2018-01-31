#!/bin/sh
CC=g++
BOOST_PATH=`pwd`/boost_1_59_0
gcc -O3 -c keccak.c
${CC} -O3 -c main.cxx -I. -I${BOOST_PATH}
${CC} -O3 -o sample.exe main.o keccak.o


rm *.o

