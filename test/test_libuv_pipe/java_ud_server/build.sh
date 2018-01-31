#!/bin/sh
CC=gcc


#build tcp server
gcc -c server.c -I.
gcc -o server.exe server.o

gcc -c client.c -I.
gcc -o client.exe client.o

#build unix domain server
gcc -c udserver.c -I.
gcc -o udserver.exe udserver.o

#inicia o udserver primeiro!!!
#nohup udserver.exe
#server conecta no udserver... e espera clientes para repassar para o udserver responder!
./server.exe

#agora chama o client.exe e digita na tela...
