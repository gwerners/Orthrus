#!/bin/sh
CC=g++



${CC} -g -c main.cc -I.
${CC} -g -o sample.exe main.o

rm *.o
./sample.exe
