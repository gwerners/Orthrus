
![▒█████   ██▀███  ▄▄▄█████▓ ██░ ██  ██▀███   █    ██   ██████
▒██▒  ██▒▓██ ▒ ██▒▓  ██▒ ▓▒▓██░ ██▒▓██ ▒ ██▒ ██  ▓██▒▒██    ▒
▒██░  ██▒▓██ ░▄█ ▒▒ ▓██░ ▒░▒██▀▀██░▓██ ░▄█ ▒▓██  ▒██░░ ▓██▄   
▒██   ██░▒██▀▀█▄  ░ ▓██▓ ░ ░▓█ ░██ ▒██▀▀█▄  ▓▓█  ░██░  ▒   ██▒
░ ████▓▒░░██▓ ▒██▒  ▒██▒ ░ ░▓█▒░██▓░██▓ ▒██▒▒▒█████▓ ▒██████▒▒
░ ▒░▒░▒░ ░ ▒▓ ░▒▓░  ▒ ░░    ▒ ░░▒░▒░ ▒▓ ░▒▓░░▒▓▒ ▒ ▒ ▒ ▒▓▒ ▒ ░
 ░ ▒ ▒░   ░▒ ░ ▒░    ░     ▒ ░▒░ ░  ░▒ ░ ▒░░░▒░ ░ ░ ░ ░▒  ░ ░
░ ░ ░ ▒    ░░   ░   ░       ░  ░░ ░  ░░   ░  ░░░ ░ ░ ░  ░  ░  
   ░ ░     ░               ░  ░  ░   ░        ░           ░  ](https://raw.githubusercontent.com/gwerners/Orthrus/master/banner.jpg)


# Orthrus
In Greek mythology, Orthrus (Greek: Ὄρθρος, Orthros) or Orthus (Greek: Ὄρθος, Orthos) was, according to the mythographer Apollodorus, a two-headed dog who guarded Geryon's cattle and was killed by Heracles. He was the offspring of the monsters Echidna and Typhon, and the brother of Cerberus, who was also a multi-headed guard dog. This project (as the monster) have more than one "face". It is a REST framework done in C++, Lua, Java and aim to enable multilanguage REST modules.


## Getting Started

Just run "./fast.bash". This script will build and run the server.
Open your browser at "http://localhost:8085/tefmock/bench" and see the example answer.

Directories:

-BO_src - REST modules (C++ and Java)

-bash    - helper ksh functions to build opensource modules

-open_source - copy of used open source libraries (dependencies)

-plugin - http parser modules (1.0 is experimental and is not recomended)

-scripts- server initialization script in lua language

-src    - server source

-test   - examples! lots of experimentations before doing the main software.

### Prerequisites

This server uses libuv, lua, luajit, protobuffer and re2c. All libraries are
provided in the directory "open_source". I'll change in the future to use git
to download them...


### Installing

The server is made to be full standalone without needing system instalation.
You will need to copy the following to the destination folder:

```
server_jit.exe
BO
libraries
scripts
```

## Built With

* [libuv](https://github.com/libuv/libuv) - Cross-platform asynchronous I/O
* [lua](https://github.com/lua/lua) - Lua language
* [luajit](https://github.com/LuaJIT/LuaJIT) - Lua language with jit
* [re2c](https://github.com/skvadrik/re2c) - lexer generator for C/C++



## Authors

* **Gabriel Wernersbach** - *Initial work* - [gwerners](https://github.com/gwerners)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
