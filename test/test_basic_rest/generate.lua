function Re2cBegin()
   print("   ".."/*!re2c")
end
function Re2cEnd()
   print("   ".."*/")
end
function PrintLBL(lblName)
   print ("lbl_" .. lblName .. ":")
end
function PrintRule(rule,var,value)
   local r= rule
   local b1="{ *var_"..var.."="..value..";"
   local b2="goto lbl_"..var..";"
   local b3="}"
   print("\t\t" .. r .. b1 .. b2 .. b3 )
end
function PrintRulel(rule,var,value)
   local r= rule
   local b1="{ var_"..var.."="..value..";"
   local b2="goto lbl_"..var..";"
   local b3="}"
   print("\t\t" .. r .. b1 .. b2 .. b3 )
end
function PrintSkipSpace(ret,next_rule)
   local r =[[
   /*!re2c
      "" { return ]] ..ret .. [[; }
      SP { goto lbl_]]..next_rule..[[;}
   */]]
   print(r)
end

function PrintSkipRule(ret,rule,next_rule)
   local r =[[
   /*!re2c
      "" { return ]] ..ret .. [[; }
      ]]..rule ..[[ { goto lbl_]]..next_rule..[[;}
   */]]
   print(r)
end
function StartPosition(name,b,e)
   print("   "..name .. ".begin="..b.. ";")
   print("   "..name .. ".end="..e..";")
end
function EndPosition(name)
   print("   "..name .. ".end=cursor;")
end
function CheckVarEmpty(name)
   print("   if(!*var_"..name..")")
   print("   {")
   print("      printf(\""..name.." nao encontrado!\\n\");")
   print("      return 0;")
   print("   }")
end
function CheckVarEmptyl(name)
   print("   if(!var_"..name..")")
   print("   {")
   print("      printf(\""..name.." nao encontrado!\\n\");")
   print("      return 0;")
   print("   }")
end
function TablePrint(tt,lbl)
   if type(tt) == "table" then
      for key, value in pairs (tt) do
         if key == "1" then
            print("skipped key1")
         else
            if type(value) == "table" then
               PrintRule(value[1],lbl,value[2])
            end
         end
      end
   end
end
function TablePrintl(tt,lbl)
   if type(tt) == "table" then
      for key, value in pairs (tt) do
         if key == "1" then
            print("skipped key1")
         else
            if type(value) == "table" then
               PrintRulel(value[1],lbl,value[2])
            end
         end
      end
   end
end
function CreateVar(name)
   io.write("int *var_"..name )
end
function CreateVarl(name)
   print("   int var_"..name ..";" )
end
print [[
/*
Source Generated with 'generate.lua'
*/
]]

print [[
//HEADERS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
]]

print [[
//DEFINES
#define YYCTYPE        char
#define YYPEEK()       (cursor >= limit ? 0 : *cursor)
#define YYSKIP()       ++cursor
#define YYBACKUP()     marker = cursor
#define YYBACKUPCTX()  ctxmarker = cursor
#define YYRESTORE()    cursor = marker
#define YYRESTORECTX() cursor = ctxmarker
]]

local method_rules={
   {'""',0},
   {'"GET"',1},
   {'"HEAD"',2},
   {'"POST"',3},
   {'"PUT"',4},
   {'"DELETE"',5},
   {'"CONNECT"',6},
   {'"OPTIONS"',7},
   {'"TRACE"',8}
}
local uri_rules={
   {'""',0},
   {'Request_URI',1}
}
local version_rules={
   {'""',0},
   {'HTTP_Version',4}
}
local request_header_rules={
   {'""',0},
   {'"Accept"',1},
   {'"Accept-Encoding"',2},
   {'"Accept-Language"',3},
   {'"Cache-Control"',4},
   {'"Connection"',5},
   {'"From"',6},
   {'"Host"',7},
   {'"User-Agent"',8},
   {'"Content-Type"',9}
}

io.write([[
typedef struct {
   const char*begin;
   const char*end;
}Position;
Position uri;
Position variable;
Position body;
int scanner(const char * cursor, const char * const limit]])
io.write(",")
CreateVar("method")
io.write(",")
CreateVar("uri")
io.write(",")
CreateVar("version")
io.write(",")
CreateVar("body")
print[[ )
{
   const char * marker;
   const char * ctxmarker;
   char buffer[127];
]]
CreateVarl("request_header")

Re2cBegin()
print [[
      re2c:yyfill:enable = 0;
      CR             = '\r';
      LF             = '\n';
      SP             = ' ';
      HT             = [\011];
      CRLF           = CR LF;
      digit          = [0-9];
      CTL            = [\000-\037] | [\177];
      SEP            = [()<>@,;:\\"/\[\]?={}] | SP | HT;
      ctoken         = [\000-\377] \(CTL | SEP);
      token          = ctoken+;
      Request_URI    = ('/'|'.'|[A-Z]|[a-z]|[0-9])+;
      HTTP_Version   = "HTTP" "/" digit+ "." digit+ ;
      variable_value = ([a-z]|[A-Z]|[0-9]|[+-=,.;* :/()_])*;
      Body           = (ctoken | SEP | CR | LF)*;
]]
Re2cEnd()
--################################################################
Re2cBegin()
TablePrint(method_rules,"method")
Re2cEnd()

PrintLBL("method")
CheckVarEmpty("method")

--################################################################
PrintSkipSpace(-1,"space_uri")
PrintLBL("space_uri")
--################################################################
StartPosition("uri","cursor","0")
Re2cBegin()
TablePrint(uri_rules,"uri")
Re2cEnd()

PrintLBL("uri")
CheckVarEmpty("uri")
EndPosition("uri")

--################################################################
PrintSkipRule(-2,"SP","space_version")
PrintLBL("space_version")
--################################################################

Re2cBegin()
TablePrint(version_rules,"version")
Re2cEnd()

PrintLBL("version")
CheckVarEmpty("version")
--################################################################
PrintSkipRule(-3,"LF","space_request_header")
PrintLBL("space_request_header")
--################################################################
StartPosition("variable","cursor","0")
Re2cBegin()
TablePrintl(request_header_rules,"request_header")
Re2cEnd()

PrintLBL("request_header")
CheckVarEmptyl("request_header")
EndPosition("variable")
print [[
   if(*cursor==':')++cursor;
      strncpy(buffer,variable.begin,variable.end-variable.begin);
      buffer[variable.end-variable.begin]='\0';
      printf("variable[%s]\n",buffer);
]]
StartPosition("variable","cursor","0")
print [[
   /*!re2c
      "" { printf("position[%s]\n",cursor);return -4; }
      variable_value { }
   */
]]
EndPosition("variable")
print [[
   if(*cursor==':')++cursor;
      strncpy(buffer,variable.begin,variable.end-variable.begin);
      buffer[variable.end-variable.begin]='\0';
      printf("value[%s]\n",buffer);
]]
--################################################################
print [[
   /*!re2c
      "" { printf(".position[%s]\n",cursor);return -4; }
      LF { goto lbl_space_request_header;}
      CRLF { goto lbl_expect_body;}
   */
]]

--################################################################

PrintLBL("expect_body")
--################################################################
StartPosition("body","cursor","0")
print [[
   /*!re2c
		""{ *var_body=0;goto lbl_body;}
		Body{ *var_body=1;goto lbl_body;}
   */
]]

PrintLBL("body")
CheckVarEmpty("body")
EndPosition("body")

print [[
   if(*var_body){
      strncpy(buffer,body.begin,body.end-body.begin);
      buffer[body.end-body.begin]='\0';
      printf("body[%s]\n",buffer);
    }
]]
print [[
   return 1;
}
]]

print [[
//MAIN
/*GET / HTTP/1.1
Host: 127.0.0.1:7000
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101 Firefox/42.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* /*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Cache-Control: max-age=0

{
    "id": 1,
    "name": "A green door",
    "price": 12.50,
    "tags": ["home", "green"]
}
*/
int main ()
{
   int ret,var_method,var_uri,var_version,var_body;
   char *str = (char*)malloc(1024);
   char buffer[127];
   strcpy(str,
"GET / HTTP/1.1\n"
"Host: 127.0.0.1:7000\n"
"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:42.0) Gecko/20100101 Firefox/42.0\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* /*;q=0.8\n"
"Accept-Language: en-US,en;q=0.5\n"
"Accept-Encoding: gzip, deflate\n"
"Connection: keep-alive\n"
"Content-Type:  application/json\n"
"Cache-Control: max-age=0\r\n"
"{"
"    \"id\": 1,\n"
"    \"name\": \"A green door\",\n"
"    \"price\": 12.50,\n"
"    \"tags\": [\"home\", \"green\"]\n"
"}"
);
   printf("input:\n%s\n",str);
   ret=scanner(str, str+strlen(str),&var_method,&var_uri,&var_version,&var_body);
   printf("scanner [%d] [%d,%d,%d]\n",ret,var_method,var_uri,var_version);
   if(ret)
   {
      printf("http method match\n");
      strncpy(buffer,uri.begin,uri.end-uri.begin);
      buffer[uri.end-uri.begin]='\0';
      printf("uri[%s]\n",buffer);

   }else
      printf("http method failed\n");

   free(str);
   return 0;
}
]]


