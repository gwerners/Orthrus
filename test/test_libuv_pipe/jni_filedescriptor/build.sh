#!/bin/sh
CC=gcc
LIBRARIES=`pwd`/libraries
export LD_LIBRARY_PATH=`pwd`
export JAVA_HOME=/usr/lib/jvm/java-8-oracle;
export PATH=$PATH:$JAVA_HOME/bin
export PREFIX=${LIBRARIES}
export CLASSPATH=`pwd`
#/com/example/tutorial
#path/usr/lib/jvm/java-8-oracle/include/jni.h

rm *.o
rm *.so
rm example.class

swig -java example.i

gcc -fpic -c example.c example_wrap.c -I/usr/local/include -I/usr/lib/jvm/java-8-oracle/include -I/usr/lib/jvm/java-8-oracle/include/linux
gcc -shared example.o example_wrap.o -o libexample.so

javac -classpath .:${CLASSPATH} Base.java
java -classpath .:${CLASSPATH} Base

rm *.o

#export LD_LIBRARY_PATH=`pwd`
#javac -classpath .:${CLASSPATH} Example.java
#java -classpath .:${CLASSPATH} Example
