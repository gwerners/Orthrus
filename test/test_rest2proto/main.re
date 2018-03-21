/*
Source Generated with 'generate.lua'
*/

//HEADERS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include "request.pb.h"

using namespace std;


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
}Position;
Position uri;
Position variable;
Position body;
int scanner(const char * cursor, const char * const limit,int *var_method,int *var_uri,int *var_version,int *var_body )
{
   const char * marker;
   const char * ctxmarker;
   char buffer[127];

   int var_request_header;
   /*!re2c
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

   */
   /*!re2c
		""{ *var_method=0;goto lbl_method;}
		"GET"{ *var_method=1;goto lbl_method;}
		"HEAD"{ *var_method=2;goto lbl_method;}
		"POST"{ *var_method=3;goto lbl_method;}
		"PUT"{ *var_method=4;goto lbl_method;}
		"DELETE"{ *var_method=5;goto lbl_method;}
		"CONNECT"{ *var_method=6;goto lbl_method;}
		"OPTIONS"{ *var_method=7;goto lbl_method;}
		"TRACE"{ *var_method=8;goto lbl_method;}
   */
lbl_method:
   if(!*var_method)
   {
      printf("method nao encontrado!\n");
      return 0;
   }
   /*!re2c
      "" { return -1; }
      SP { goto lbl_space_uri;}
   */
lbl_space_uri:
   uri.begin=cursor;
   uri.end=0;
   /*!re2c
		""{ *var_uri=0;goto lbl_uri;}
		Request_URI{ *var_uri=1;goto lbl_uri;}
   */
lbl_uri:
   if(!*var_uri)
   {
      printf("uri nao encontrado!\n");
      return 0;
   }
   uri.end=cursor;
   /*!re2c
      "" { return -2; }
      SP { goto lbl_space_version;}
   */
lbl_space_version:
   /*!re2c
		""{ *var_version=0;goto lbl_version;}
		HTTP_Version{ *var_version=4;goto lbl_version;}
   */
lbl_version:
   if(!*var_version)
   {
      printf("version nao encontrado!\n");
      return 0;
   }
   /*!re2c
      "" { return -3; }
      LF { goto lbl_space_request_header;}
   */
lbl_space_request_header:
   variable.begin=cursor;
   variable.end=0;
   /*!re2c
		""{ var_request_header=0;goto lbl_request_header;}
		"Accept"{ var_request_header=1;goto lbl_request_header;}
		"Accept-Encoding"{ var_request_header=2;goto lbl_request_header;}
		"Accept-Language"{ var_request_header=3;goto lbl_request_header;}
		"Cache-Control"{ var_request_header=4;goto lbl_request_header;}
		"Connection"{ var_request_header=5;goto lbl_request_header;}
		"From"{ var_request_header=6;goto lbl_request_header;}
		"Host"{ var_request_header=7;goto lbl_request_header;}
		"User-Agent"{ var_request_header=8;goto lbl_request_header;}
		"Content-Type"{ var_request_header=9;goto lbl_request_header;}
   */
lbl_request_header:
   if(!var_request_header)
   {
      printf("request_header nao encontrado!\n");
      return 0;
   }
   variable.end=cursor;
   if(*cursor==':')++cursor;
      strncpy(buffer,variable.begin,variable.end-variable.begin);
      buffer[variable.end-variable.begin]='\0';
      printf("variable[%s]\n",buffer);

   variable.begin=cursor;
   variable.end=0;
   /*!re2c
      "" { printf("position[%s]\n",cursor);return -4; }
      variable_value { }
   */

   variable.end=cursor;
   if(*cursor==':')++cursor;
      strncpy(buffer,variable.begin,variable.end-variable.begin);
      buffer[variable.end-variable.begin]='\0';
      printf("value[%s]\n",buffer);

   /*!re2c
      "" { printf(".position[%s]\n",cursor);return -4; }
      LF { goto lbl_space_request_header;}
      CRLF { goto lbl_expect_body;}
   */

lbl_expect_body:
   body.begin=cursor;
   body.end=0;
   /*!re2c
		""{ *var_body=0;goto lbl_body;}
		Body{ *var_body=1;goto lbl_body;}
   */

lbl_body:
   if(!*var_body)
   {
      printf("body nao encontrado!\n");
      return 0;
   }
   body.end=cursor;
   if(*var_body){
      strncpy(buffer,body.begin,body.end-body.begin);
      buffer[body.end-body.begin]='\0';
      printf("body[%s]\n",buffer);
    }

   return 1;
}

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


void PromptForRequest(interfaces::Request* request)
{
   /*cin.ignore(256, '\n');
   cout << "Enter path: ";
   getline(cin, *request->mutable_path());

   cout << "Enter method(GET,HEAD,POST,PUT,DELETE,CONNECT,OPTIONS,TRACE):";

   string type;
   getline(cin, type);
   if (type == "GET") {
      request->set_type(interfaces::Request::GET);
   } else if (type == "HEAD") {
      request->set_type(interfaces::Request::HEAD);
   } else if (type == "POST") {
      request->set_type(interfaces::Request::POST);
   } else if (type == "PUT") {
      request->set_type(interfaces::Request::PUT);
   } else if (type == "DELETE") {
      request->set_type(interfaces::Request::DELETE);
   } else if (type == "CONNECT") {
      request->set_type(interfaces::Request::CONNECT);
   } else if (type == "OPTIONS") {
      request->set_type(interfaces::Request::OPTIONS);
   } else if (type == "TRACE") {
      request->set_type(interfaces::Request::TRACE);
   } else {
      cout << "Unknown method type.  Using default." << endl;
   }

   while (true) {
      cout << "Enter parameter list" << endl;
      cout << "key: ";
      string key;
      getline(cin, key);
      if (key.empty()) {
         break;
      }

      interfaces::Request::Parameters* parameter = request->add_params();
      parameter->set_key(key);
      cout << "value: ";
      string value;
      getline(cin, value);
      parameter->set_value(value);
   }
   cout << "Enter body: ";
   getline(cin, *request->mutable_body());*/
}
int main()
{
   int ret,var_method,var_uri,var_version,var_body;
   char *str = (char*)malloc(1024);
   char buffer[127];
   // Verify that the version of the library that we linked against is
   // compatible with the version of the headers we compiled against.
   GOOGLE_PROTOBUF_VERIFY_VERSION;

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

   // Optional:  Delete all global objects allocated by libprotobuf.
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}

