#!/bin/sh
CC=g++

CFLAGS="-DHTTP_PARSER_STRICT=0 -Wall -Wextra -Werror"
gcc ${CFLAGS} -c http_parser.c -I.
${CC} ${CFLAGS} -c main.cc -I.
${CC}  ${CFLAGS} -o sample.exe main.o http_parser.o

rm *.o
./sample.exe
