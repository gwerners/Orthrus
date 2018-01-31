#!/bin/sh
CC=gcc


${CC} -c cli.c -I.
${CC} -c srv.c -I.
${CC} -o server.exe srv.o
${CC} -o client.exe cli.o 

rm *.o

#export LD_LIBRARY_PATH=`pwd`
./server.exe
