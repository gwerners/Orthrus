#!/bin/bash
PWD=`pwd`
source `pwd`/../../../bash/build_functions.bash
CC=g++

LIBRARIES=`pwd`/../../../libraries
OPEN_SOURCE=`pwd`/../../../open_source

export JAVA_HOME=/usr/lib/jvm/default-java;
export PATH=$PATH:$JAVA_HOME/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/jvm/default-java/jre/lib/amd64/server
export CLASSPATH=`pwd`:`pwd`/com/interfaces:`pwd`/com/:`pwd`/com/interfaces/request:${LIBRARIES}/lib/protobuf-java-3.0.0-beta-1.jar

CFLAGS="-O3 -I$JAVA_HOME/include -I$JAVA_HOME/include/linux"
LDFLAGS="-L$JAVA_HOME/jre/lib/i386 -L$JAVA_HOME/jre/lib/i386/client/ -L/usr/lib/jvm/default-java/jre/lib/amd64/server -ljvm"


build "protobuffer"
build "libuv"

cd ${PWD}

${LIBRARIES}/bin/protoc --cpp_out=. request.proto
${LIBRARIES}/bin/protoc --cpp_out=. response.proto

${LIBRARIES}/bin/protoc --java_out=. request.proto
${LIBRARIES}/bin/protoc --java_out=. response.proto

javac -classpath .:${CLASSPATH} Worker.java

${CC} -c request.pb.cc -I. -I${LIBRARIES}/include ${CFLAGS}
${CC} -c response.pb.cc -I. -I${LIBRARIES}/include ${CFLAGS}
${CC} -c logger.cc -I. -I${LIBRARIES}/include ${CFLAGS}
${CC} -c worker.cc -I. -I${LIBRARIES}/include ${CFLAGS}
${CC} -o worker.exe worker.o logger.o request.pb.o response.pb.o ${LDFLAGS} ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -pthread

${CC} -c client.cc -I. -I${LIBRARIES}/include ${CFLAGS}
${CC} -o client.exe client.o logger.o request.pb.o response.pb.o ${LDFLAGS} ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -pthread

rm *.o
./client.exe "-Djava.class.path=.:${CLASSPATH}"

#echo ${CLASSPATH}
