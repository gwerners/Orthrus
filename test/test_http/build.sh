#!/bin/sh
CC=g++


gcc -g -c http_parser.c -I.
${CC} -g -c main.cc -I.
${CC} -g -o sample.exe main.o http_parser.o

rm *.o
./sample.exe
