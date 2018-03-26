--################## CONSTANTES ##################
--CONSTANTES -> NAO ALTERAR!
--log types
LOG_DEBUG   =  1
LOG_ERROR   =  2
LOG_WARNING =  4
LOG_INFO    =  8
LOG_COLOR   = 16

--BusinessObject types:
BO_JAVA = 1
BO_CPP  = 2
BO_LUA  = 3
--################## CONSTANTES ##################

port=8085
admin_port=8086

LogWithColor(true)--default is false!

DisableLog(LOG_DEBUG)--remove any debug entry from log!
LogDebug("appearing?")

--AbortOnError(true)

PrintPID()

LogInfo("Server configured on Port " .. port)
LogInfo("Server Administration on Port " .. admin_port)

--LogWithColor(false)
CreateTcp4Server(port)
CreateTcp4AdminServer(admin_port)
--LogWithColor(true)

CLASSPATH=
   "-Djava.class.path=" .. os.getenv("PWD") ..
   ":" .. os.getenv("PWD") .. "/BO/JAVA/USSD" ..
   ":" .. os.getenv("PWD") .. "/BO/JAVA/USSD/com" ..
   ":" .. os.getenv("PWD") .. "/BO/JAVA/USSD/com/interfaces" ..
   ":" .. os.getenv("PWD") .. "/BO/JAVA/USSD/com/interfaces/request" ..
   ":" .. os.getenv("PWD") .. "/libraries/lib/protobuf-java-3.0.0-beta-1.jar"

LogInfo(CLASSPATH)

SetJavaClassPath(CLASSPATH)

--[[
AddBusinessObject parameters:
1- Service Name
   - identifier for this service
2 - Object type
   BO_JAVA - will run a jvm for each instance
   BO_CPP  - will run a shared library
   BO_LUA  - will run a lua script
3 - regular expression
   - match this expression against rest path to fire one instance
4 - number of instances
   - how many objects will be running to handle connections
   - 0 means "run as much cpu cores are available!"
5 - path to binary (relative to server.exe)
   BO_JAVA - must end with entry point object_name.class
   BO_CPP  - must end with object_name.so
   BO_LUA  - must end with object_name.lua
]]
--DisableLog(LOG_COLOR)--remove color from log
AddBusinessObject("SMS",BO_CPP,"/tefmock/sms",0,"/BO/CPP/SMS/worker.exe")
AddBusinessObject("USSD",BO_JAVA,"/tefmock/ussd",0,"/BO/JAVA/USSD/worker.exe")
AddBusinessObject("BENCH",BO_CPP,"/tefmock/bench",0,"/BO/CPP/BENCH/worker.exe")
--AddBusinessObject("SMS",BO_CPP,"/SMS/[0-9]*/[0-9]*",0,"/BO/CPP/SMS/worker.exe")
--AddBusinessObject("SMS",BO_JAVA,"/SMS/[0-9]*/[0-9]*",1,"/BO/JAVA/SMS/Sample.class")
--AddBusinessObject("URA",BO_CPP,"/URA/[0-9]*/[0-9]*",2,"/BO/CPP/URA/object.so")
--AddBusinessObject("REGARGA",BO_LUA,"/RECARGA/[0-9]*/[0-9]*",0,"/BO/LUA/RECARGA/main.lua")

LogDebug(ListBusinessObjects())

--DisableLog(LOG_DEBUG)--remove any debug entry from log!

--RemoveBusinessObject("URA")

--EnableLog(LOG_DEBUG) --enable!
--EnableLog(LOG_COLOR)

--LogDebug(ListBusinessObjects())

EnableSigInt()

--load plugin!
LoadPluginLibrary("./libraries/lib/libRest-2.0.so","LoadScannerPlugin","LoadScannerPluginDummy")

--test plugin
request="GET /path/cgi.script HTTP/1.1\r\n" ..
      "Host: 127.0.0.1:7000\r\n" ..
      "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101 Firefox/42.0\r\n" ..
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* /*;q=0.8\r\n" ..
      "Accept-Language: en-US,en;q=0.5\r\n" ..
      "Accept-Encoding: gzip, deflate\r\n" ..
      "Connection: keep-alive\r\n" ..
      "Content-Type:  application/json\r\n" ..
      "Cache-Control: max-age=0\r\n" ..
      "Content-Length: 93\r\n\r\n"..
      "{" ..
      "    \"id\": 1,\n" ..
      "    \"name\": \"A green door\",\n" ..
      "    \"price\": 12.50,\n" ..
      "    \"tags\": [\"home\", \"green\"]\n"..
      "}"
LogDebug("test RestScanner")
RestScanner(request)
LogDebug("test RestScanner ENDED")


--server will start event processing:
ProcessEvents() --will block and do the server stuff...

--put here any step needed to run after server shutdown...
LogInfo "Server shutdown completed"
