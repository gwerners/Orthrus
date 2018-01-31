#!/bin/sh

gcc -O3 -c keccak.c -I.
gcc -O3 -c main.c -I.
gcc -O3 -o sample.exe main.o keccak.o

rm *.o

