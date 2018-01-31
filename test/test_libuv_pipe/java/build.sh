#!/bin/sh
CC=g++

LIBRARIES=`pwd`/libraries
export JAVA_HOME=/usr/lib/jvm/java-8-oracle;
export PATH=$PATH:$JAVA_HOME/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/jvm/java-8-oracle/jre/lib/amd64/server
export CLASSPATH=`pwd`/com/example/tutorial:${LIBRARIES}/lib/protobuf-java-3.0.0-beta-1.jar

CFLAGS="-I$JAVA_HOME/include -I$JAVA_HOME/include/linux"
LDFLAGS="-L$JAVA_HOME/jre/lib/i386 -L$JAVA_HOME/jre/lib/i386/client/ -L/usr/lib/jvm/java-8-oracle/jre/lib/amd64/server -ljvm"


export PREFIX=${LIBRARIES}

if [ -e "${LIBRARIES}/bin/protoc" ]
then
   echo "protoc exists..."
else
   echo "building protobuffer!"
   mkdir ${LIBRARIES}
   tar -xjf protobuf-master.tar.bz2
   cd protobuf-master
   ./autogen.sh
   ./configure --prefix=${LIBRARIES}
   make
   make install
   #https://github.com/google/protobuf/tree/master/java
   cd java
   #use maven to create java package of protobuffer!
   mvn install
   mvn package
   cp target/protobuf-java-3.0.0-beta-1.jar ${LIBRARIES}/lib
   cd ..
   cd ..
   rm -rf protobuf-master
fi

if [ -e "${LIBRARIES}/lib/libuv.a" ]
then
   echo "libuv exists..."
else
   echo "building libuv!"
   mkdir ${LIBRARIES}
   tar -xjf libuv-1.x.tar.bz2

   cd libuv-1.x
   sh autogen.sh
   ./configure --prefix=${LIBRARIES}
   make
   #uncomment next line to make long check on libuv...
   #make check
   make install
   cd ..
   rm -rf libuv-1.x
fi
${LIBRARIES}/bin/protoc --java_out=. request.proto
${LIBRARIES}/bin/protoc --cpp_out=. request.proto

javac -classpath .:${CLASSPATH} Worker.java

${CC} -c main.cpp -I. -I${LIBRARIES}/include
${CC} -o server.exe main.o ${LIBRARIES}/lib/libuv.a -pthread

${CC} -c worker.cpp -I. -I${LIBRARIES}/include ${CFLAGS}
${CC} -o worker.exe worker.o ${LIBRARIES}/lib/libuv.a -pthread ${LDFLAGS}



rm *.o

#export LD_LIBRARY_PATH=`pwd`
./server.exe
