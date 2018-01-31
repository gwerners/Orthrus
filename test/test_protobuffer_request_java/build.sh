#!/bin/sh
CC=g++

LIBRARIES=`pwd`/libraries
export JAVA_HOME=/usr/lib/jvm/java-8-oracle;
export PATH=$PATH:$JAVA_HOME/bin
export PREFIX=${LIBRARIES}
export CLASSPATH=`pwd`/com/example/tutorial:${LIBRARIES}/lib/protobuf-java-3.0.0-beta-1.jar
#re2c-0.14.3

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


#${LIBRARIES}/bin/protoc --java_out=. AddressBook.proto
${LIBRARIES}/bin/protoc --java_out=. request.proto

#javac -classpath .:${CLASSPATH} AddPerson.java
#javac -classpath .:${CLASSPATH} ListPerson.java
#java -classpath .:${CLASSPATH} AddPerson
#java -classpath .:${CLASSPATH} ListPerson

javac -classpath .:${CLASSPATH} AddRequest.java
javac -classpath .:${CLASSPATH} ListRequest.java
java -classpath .:${CLASSPATH} AddRequest message.bin
sleep 2
echo "run ListRequest"
java -classpath .:${CLASSPATH} ListRequest message.bin


#rm *.o
#rm request.pb.*
#export LD_LIBRARY_PATH=`pwd`
#./client.exe request.book
