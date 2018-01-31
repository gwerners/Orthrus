/*
Source Generated with 'generate.lua'
*/

//HEADERS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//DEFINES
#define YYCTYPE        char
#define YYPEEK()       (cursor >= limit ? 0 : *cursor)
#define YYSKIP()       ++cursor
#define YYBACKUP()     marker = cursor
#define YYBACKUPCTX()  ctxmarker = cursor
#define YYRESTORE()    cursor = marker
#define YYRESTORECTX() cursor = ctxmarker

typedef struct {
   const char*begin;
   const char*end;
}POSITION;
POSITION uri;
int scanner(const char * cursor, const char * const limit)
{
    const char * marker;
    const char * ctxmarker;

int var_method;
int var_uri;
int var_version;
   /*!re2c
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

   */
   /*!re2c
		""{ var_method=0;goto lbl_method;}
		"GET"{ var_method=1;goto lbl_method;}
		"HEAD"{ var_method=2;goto lbl_method;}
		"POST"{ var_method=3;goto lbl_method;}
		"PUT"{ var_method=4;goto lbl_method;}
		"DELETE"{ var_method=5;goto lbl_method;}
		"CONNECT"{ var_method=6;goto lbl_method;}
		"OPTIONS"{ var_method=7;goto lbl_method;}
		"TRACE"{ var_method=8;goto lbl_method;}
   */
lbl_method:
   if(!var_method)
   {
      printf("method nao encontrado!\n");
      return 0;
   }
   /*!re2c

      "" { return -3; }
      SP { goto lbl_space_uri;}
   */
lbl_space_uri:
   uri.begin=cursor;
   uri.end=0;
   /*!re2c
		""{ var_uri=0;goto lbl_uri;}
		Request_URI{ var_uri=1;goto lbl_uri;}
   */
lbl_uri:
   if(!var_uri)
   {
      printf("uri nao encontrado!\n");
      return 0;
   }
   uri.end=cursor;
   /*!re2c

      "" { return -3; }
      SP { goto lbl_space_version;}
   */
lbl_space_version:
   /*!re2c
		""{ var_version=0;goto lbl_version;}
		HTTP_Version{ var_version=4;goto lbl_version;}
   */
lbl_version:
   if(!var_version)
   {
      printf("version nao encontrado!\n");
      return 0;
   }
   return var_version;
}

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

