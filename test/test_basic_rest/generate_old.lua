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
function StartPosition(name,b,e)
   print("   "..name .. ".begin="..b.. ";")
   print("   "..name .. ".end="..e..";")
end
function EndPosition(name)
   print("   "..name .. ".end=cursor;")
end
function CheckVarEmpty(name)
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
function CreateVar(name)
   print("int var_"..name ..";")
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
print [[
typedef struct {
   const char*begin;
   const char*end;
}POSITION;
POSITION uri;
int scanner(const char * cursor, const char * const limit)
{
    const char * marker;
    const char * ctxmarker;
]]
CreateVar("method")
CreateVar("uri")
CreateVar("version")
Re2cBegin()
print [[
      re2c:yyfill:enable = 0;
      CR             = '\r';
      LF             = '\n';
      SP             = ' ';
      HT             = [\011];
      CRLF           = CR LF;
      digit         = [0-9];
      hex           = digit | [A-F] | [a-f];
      escaped       = "%" hex hex;
      reserved      = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | "," ;
      mark          = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")" ;
      lowalpha      = [a-z];
      upalpha       = [A-Z];
      alpha         = lowalpha | upalpha;
      alphanum      = alpha | digit; 
      unreserved    = alphanum | mark;
      scheme        = alpha ( alpha | digit | "+" | "-" | "." )*;
      pchar         = unreserved | escaped | ":" | "@" | "&" | "=" | "+" | "$" | ","; 
      param         = pchar*;
      segment       = pchar* ( ";" param )*;
      path_segments = segment ( "/" segment )*;
      abs_path      = "/"  path_segments;
      userinfo      = ( unreserved | escaped | ";" | ":" | "&" | "=" | "+" | "$" | "," )*;
      toplabel      = alpha | alpha ( alphanum | "-" )* alphanum;
      domainlabel   = alphanum | alphanum ( alphanum | "-" )* alphanum;
      hostname      = ( domainlabel "." )* toplabel ( "." )?;
      IPv4address   = digit+ "." digit+ "." digit+ "." digit+;
      host          = hostname | IPv4address ;
      port          = digit*;
      hostport      = host ( ":" port )?;
      server        = ( (userinfo "@")? hostport)?;
      reg_name      = ( unreserved | escaped | "$" | "," | ";" | ":" | "@" | "&" | "=" | "+" )+;
      authority     = server | reg_name;
      uric          = reserved | unreserved | escaped;
       query         = uric*;
      net_path      = "//" authority abs_path?;
      hier_part     = ( net_path | abs_path ) ( "?" query )?;
      uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ",";
      opaque_part   = uric_no_slash uric*;
      absoluteURI   = scheme ":" ( hier_part | opaque_part );
      Request_URI    = "*" | absoluteURI | abs_path | authority;
      HTTP_Version   = "HTTP" "/" digit+ "." digit+ ;
]]
Re2cEnd()

Re2cBegin()
TablePrint(method_rules,"method")
Re2cEnd()

PrintLBL("method")
CheckVarEmpty("method")

PrintSkipSpace(-3,"space_uri")
PrintLBL("space_uri")

StartPosition("uri","cursor","0")

Re2cBegin()
TablePrint(uri_rules,"uri")
Re2cEnd()

PrintLBL("uri")
CheckVarEmpty("uri")
EndPosition("uri")

PrintSkipSpace(-3,"space_version")
PrintLBL("space_version")

Re2cBegin()
TablePrint(version_rules,"version")
Re2cEnd()

PrintLBL("version")
CheckVarEmpty("version")
function ReturnVar(name)
   print("   return var_"..name..";")
end
ReturnVar("version")
print [[
}
]]

print [[
//MAIN
int main ()
{
   int ret;
   char *str = (char*)malloc(1024);
   char buffer[127];
   strcpy(str,"GET /index.html HTTP/1.1");
   ret=scanner(str, str+strlen(str));
   printf("scanner [%d]\n",ret);
   if(ret==4)
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


