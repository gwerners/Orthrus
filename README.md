▒█████   ██▀███  ▄▄▄█████▓ ██░ ██  ██▀███   █    ██   ██████ 
▒██▒  ██▒▓██ ▒ ██▒▓  ██▒ ▓▒▓██░ ██▒▓██ ▒ ██▒ ██  ▓██▒▒██    ▒
▒██░  ██▒▓██ ░▄█ ▒▒ ▓██░ ▒░▒██▀▀██░▓██ ░▄█ ▒▓██  ▒██░░ ▓██▄   
▒██   ██░▒██▀▀█▄  ░ ▓██▓ ░ ░▓█ ░██ ▒██▀▀█▄  ▓▓█  ░██░  ▒   ██▒
░ ████▓▒░░██▓ ▒██▒  ▒██▒ ░ ░▓█▒░██▓░██▓ ▒██▒▒▒█████▓ ▒██████▒▒
░ ▒░▒░▒░ ░ ▒▓ ░▒▓░  ▒ ░░    ▒ ░░▒░▒░ ▒▓ ░▒▓░░▒▓▒ ▒ ▒ ▒ ▒▓▒ ▒ ░
 ░ ▒ ▒░   ░▒ ░ ▒░    ░     ▒ ░▒░ ░  ░▒ ░ ▒░░░▒░ ░ ░ ░ ░▒  ░ ░
░ ░ ░ ▒    ░░   ░   ░       ░  ░░ ░  ░░   ░  ░░░ ░ ░ ░  ░  ░  
   ░ ░     ░               ░  ░  ░   ░        ░           ░  


# Orthrus
In Greek mythology, Orthrus (Greek: Ὄρθρος, Orthros) or Orthus (Greek: Ὄρθος, Orthos) was, according to the mythographer Apollodorus, a two-headed dog who guarded Geryon's cattle and was killed by Heracles. He was the offspring of the monsters Echidna and Typhon, and the brother of Cerberus, who was also a multi-headed guard dog. This project (as the monster) have more than one "face". It is a REST framework done in C++, Lua, Java and aim to enable multilanguage REST modules.


Directories:

-BO_src - REST modules (C++ and Java)

-bash    - helper ksh functions to build opensource modules

-open_source - copy of used open source libraries

-plugin - http parser modules (1.0 is experimental and is not recomended)

-scripts- server initialization script in lua language

-src    - server source

-test   - examples! lots of experimentations before doing the main software.

TODO:

1)enable async processing of the request. Currently the request will block on
the C++/Java module waiting the response. This will be changed as the module
already have the socket descriptor to answer anytime in the future...

2)rebuild the http parser (the one in use is from nodejs project)

3)enable a fast url2module using regular expression.

4)review log library: need to be unified, log to file and check for performance.

Gabriel W Farinas
