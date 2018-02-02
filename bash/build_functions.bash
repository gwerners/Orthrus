#!/bin/bash

#$1 = LIBRARIES   PATH - install location
#$2 = OPEN_SOURCE PATH - libraries repository

OS="$(uname)"

if [[ ${1} == "" ]]; then
   LIBRARIES=`pwd`/libraries
   echo "LIBRARIES=${LIBRARIES}"
else
   LIBRARIES=`pwd`/${1}
   echo "LIBRARIES=${LIBRARIES}"
fi

if [[ ${1} == "" ]]; then
   OPEN_SOURCE=`pwd`/open_source
   echo "OPEN_SOURCE=${OPEN_SOURCE}"
else
   OPEN_SOURCE=`pwd`/${2}
   echo "OPEN_SOURCE=${OPEN_SOURCE}"
fi

#luajit needs this:
export PREFIX=${LIBRARIES}

function copy_lib {
   echo "copy ${OPEN_SOURCE}/$1"
   cp ${OPEN_SOURCE}/$1 $1
}
function build {
   if [[ $1 == "libuv" ]]; then
      if [[ -e "${LIBRARIES}/lib/libuv.a" ]]; then
         echo "libuv exists..."
      else
         echo "building libuv!"
         mkdir ${LIBRARIES}
         copy_lib libuv-1.x.tar.bz2
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
         rm libuv-1.x.tar.bz2
      fi
   fi
   if [[ $1 == "lua" ]]; then
      if [[ -e "${LIBRARIES}/lib/liblua.a" ]]; then
         echo "liblua.a exists"
      else
         #build luajit
         mkdir ${LIBRARIES}
         copy_lib lua-5.1.5.tar.gz
         tar -xzf lua-5.1.5.tar.gz
         cd lua-5.1.5
         if [[ "${OS}" = "OSX" ]]; then
            make macosx
         else
            make linux
         fi
         make install
         cd ..
         rm -rf lua-5.1.5
         rm lua-5.1.5.tar.gz
      fi
   fi
   if [[ $1 == "luajit" ]]; then
      #extract luajit
      #http://luajit.org/
      if [[ -e "${LIBRARIES}/lib/libluajit-5.1.a" ]]; then
         echo "libluajit-5.1.a exists"
      else
         #build luajit
         mkdir ${LIBRARIES}
         copy_lib LuaJIT-2.0.3.tar.gz
         tar -xzf LuaJIT-2.0.3.tar.gz
         cd LuaJIT-2.0.3
         make
         make install
         cd ..
         rm -rf LuaJIT-2.0.3
         rm LuaJIT-2.0.3.tar.gz
      fi
   fi
   if [[ $1 == "re2c" ]]; then
      if [[ -e "${LIBRARIES}/bin/re2c" ]]; then
         echo "re2c exists..."
      else
         echo "building re2c!"
         mkdir ${LIBRARIES}
         copy_lib re2c-0.14.3.tar.gz
         tar -xzf re2c-0.14.3.tar.gz

         cd re2c-0.14.3
         ./configure --prefix=${LIBRARIES}
         make
         make install
         cd ..
         rm -rf re2c-0.14.3
         rm re2c-0.14.3.tar.gz
      fi
   fi
   if [[ $1 == "protobuffer" ]]; then
      if [[ -e "${LIBRARIES}/bin/protoc" ]]; then
         echo "protoc exists..."
      else
         echo "building protobuffer!"
         mkdir ${LIBRARIES}
         copy_lib protobuf-master.tar.bz2
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

         rm protobuf-master.tar.bz2
      fi
   fi
}
