#!/bin/bash

source `pwd`/bash/build_functions.bash

CC=g++
CFLAGS="-O3"
#CFLAGS="-g -O0"
LIBRARIES=`pwd`/libraries
OPEN_SOURCE=`pwd`/open_source
OS="$(uname)"
#PROF=-pg
#export JAVA_HOME=/usr/lib/jvm/default-java
export JAVA_HOME=/usr/lib/jvm/default-java
export PATH=$PATH:$JAVA_HOME/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/jvm/default-java/jre/lib/amd64/server
export CLASSPATH=`pwd`/BO/JAVA/USSD/com/interfaces/request:${LIBRARIES}/lib/protobuf-java-3.0.0-beta-1.jar

#rpath set the default shared library search path inside the binary!
RPATH="-Wl,-rpath ./libraries/lib"

export PREFIX=${LIBRARIES}


USE_JIT="YES"

build "libuv"
build "protobuffer"

cd src
${LIBRARIES}/bin/protoc --cpp_out=. request.proto
${LIBRARIES}/bin/protoc --cpp_out=. response.proto
cd ..

if [[ "${USE_JIT}" == "YES" ]]; then
   SERVER_NAME="server_jit.exe"
   build "luajit"
else
   SERVER_NAME="server.exe"
   build "lua"
fi



if [[ "${USE_JIT}" == "YES" ]]; then
   ${CC} ${PROF} ${CFLAGS} -c src/main.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0 -I${LIBRARIES}/include/luajit-2.0
   ${CC} ${PROF} ${CFLAGS} -c src/logger.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0 -I${LIBRARIES}/include/luajit-2.0
   ${CC} ${PROF} ${CFLAGS} -c src/plugin.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0 -I${LIBRARIES}/include/luajit-2.0
   ${CC} ${PROF} ${CFLAGS} -c src/lua_utils.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0 -I${LIBRARIES}/include/luajit-2.0.0
   ${CC} ${PROF} ${CFLAGS} -c src/request.pb.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0 -I${LIBRARIES}/include/luajit-2.0.0
   ${CC} ${PROF} ${CFLAGS} -c src/response.pb.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/luajit-2.0 -I${LIBRARIES}/include/luajit-2.0.0
   if [[ "${OS}" = "OSX" ]]; then
      ${CC} ${PROF} ${CFLAGS} -o ${SERVER_NAME} main.o logger.o plugin.o lua_utils.o request.pb.o response.pb.o ${RPATH} -L${LIBRARIES}/lib/ ${LIBRARIES}/lib/libluajit-5.1.a ${LIBRARIES}/lib/libuv.a ${LIBRARIES}/lib/libprotobuf.a -lpthread -ldl -lm -pagezero_size 10000 -image_base 100000000
   else
   #"Linux"
      ${CC} ${PROF} ${CFLAGS} -o ${SERVER_NAME} main.o logger.o plugin.o lua_utils.o request.pb.o response.pb.o ${RPATH} -L${LIBRARIES}/lib/ ${LIBRARIES}/lib/libluajit-5.1.a ${LIBRARIES}/lib/libuv.a ${LIBRARIES}/lib/libprotobuf.a -lpthread -ldl -lm
   fi
else
   ${CC} ${PROF} ${CFLAGS} -c src/main.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/lua
   ${CC} ${PROF} ${CFLAGS} -c src/logger.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/lua
   ${CC} ${PROF} ${CFLAGS} -c src/plugin.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/lua
   ${CC} ${PROF} ${CFLAGS} -c src/lua_utils.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/lua
   ${CC} ${PROF} ${CFLAGS} -c src/request.pb.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/lua
   ${CC} ${PROF} ${CFLAGS} -c src/response.pb.cc -Isrc -I${LIBRARIES}/include -I${LIBRARIES}/include/lua
   ${CC} ${PROF} ${CFLAGS} -o ${SERVER_NAME} main.o logger.o plugin.o lua_utils.o request.pb.o response.pb.o ${RPATH} -L${LIBRARIES}/lib/  ${LIBRARIES}/lib/liblua.a ${LIBRARIES}/lib/libprotobuf.a ${LIBRARIES}/lib/libuv.a -lpthread -ldl -lm
fi

echo "Building libraries!"
if [[ -e "${LIBRARIES}/lib/libRest-2.0.so" ]]; then
   echo "lib exists:${LIBRARIES}/lib/libRest-2.0.so, skipping build!"
else
   cd plugin/Rest-2.0
   ./fast.bash
   cp ./libRest-2.0.so ${LIBRARIES}/lib/libRest-2.0.so
   cd ../../
fi

if [[ -e "${LIBRARIES}/lib/libRest-2.0.so" ]]; then
   echo "Sucessfull build of libRest-2.0.so"
else
   echo "Problem building libRest-2.0.so, please check!"
fi
rm *.o

if [[ "${OS}" = "OSX" ]]; then
   #para listar as libs com seus caminhos:
   #otool -L libraries/lib/libRest-1.0.so
   #fix para o caminho da libprotobuf dentro da lib:
   install_name_tool -change `pwd`/plugin/Rest-2.0/libraries/lib/libprotobuf.10.dylib `pwd`/libraries/lib/libprotobuf.10.dylib `pwd`/libraries/lib/libRest-2.0.so
   echo "libprotobuf.so corrected for osx"
else
   #mostra o rpath da aplicacao!
   # readelf --dynamic libraries/lib/libRest-1.0.so | grep PATH
   #0x000000000000000f (RPATH)              Library rpath: [./libraries/lib/]

   #lista bibliotecas dinamicas e os respectivos caminhos!
   #ldd libraries/lib/libRest-1.0.so
	#libprotobuf.so.10 => ./libraries/lib/libprotobuf.so.10 (0x00007f3af39f8000)

   #sudo apt-get install chrpath

   #essa linha corrige o rpath da libprotobuf para o diretorio de libs!
   #chrpath -r libprotobuf.so.10 --replace `pwd`/libraries/lib/libprotobuf.so.10
   echo "libprotobuf.so corrected for linux"
fi
################################################################################
echo "Build business objects"
if [[ -e "`pwd`/BO/CPP/SMS/worker.exe" ]]; then
   rm -rf  "`pwd`/BO/CPP/SMS/worker.exe"
fi
cd BO_src/CPP/SMS
./fast.bash
mkdir -p ../../../BO/CPP/SMS
cp worker.exe ../../../BO/CPP/SMS
cd ../../..
if [[ -e "`pwd`/BO/CPP/SMS/worker.exe" ]]; then
   echo "build component ok"
else
   echo "build worker.exe failed"
fi

################################################################################

if [[ -e "`pwd`/BO/JAVA/USSD/worker.exe" ]]; then
   rm -rf  "`pwd`/BO/JAVA/USSD/worker.exe"
fi
cd BO_src/JAVA/USSD
./fast.bash
mkdir -p ../../../BO/JAVA/USSD
cp worker.exe ../../../BO/JAVA/USSD
cp Worker.class ../../../BO/JAVA/USSD
cp -r com ../../../BO/JAVA/USSD

cd ../../..
if [[ -e "`pwd`/BO/JAVA/USSD/worker.exe" ]]; then
   echo "build component ok"
else
   echo "build worker.exe failed"
fi
################################################################################
if [[ -e "`pwd`/BO/CPP/BENCH/worker.exe" ]]; then
   rm -rf  "`pwd`/BO/CPP/BENCH/worker.exe"
   rm -rf  "`pwd`/BO/CPP/BENCH/scripts"
fi
cd BO_src/CPP/BENCH
./fast.bash
mkdir -p ../../../BO/CPP/BENCH
mkdir -p ../../../BO/CPP/BENCH/scripts
cp worker.exe ../../../BO/CPP/BENCH
cp scripts/*.lua ../../../BO/CPP/BENCH/scripts

cd ../../..
if [[ -e "`pwd`/BO/CPP/BENCH/worker.exe" ]]; then
   echo "build component ok"
else
   echo "build worker.exe failed"
fi

#valgrind valgrind --leak-check=full --show-leak-kinds=all --log-file="valgrind.txt" ./server_jit.exe
./server_jit.exe
