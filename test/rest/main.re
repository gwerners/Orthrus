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

//SCAN ERRORS
#define SCAN_ERROR_MISSING_METHOD    -1
#define SCAN_ERROR_MISSING_URI       -2
#define SCAN_ERROR_MISSING_VERSION   -3
#define SCAN_ERROR_MISSING_PARAMETER -4
#define SCAN_ERROR_MISSING_BODY      -5

#define string_missing_method    "metodo nao encontrado!"
#define string_missing_uri       "uri nao encontrado!"
#define string_missing_version   "version nao encontrado!"
#define string_missing_parameter "parametro nao encontrado!"
#define string_missing_body      "body nao encontrado!"

const char*StrScanErr(int error)
{
   switch(error)
   {
      case 1: return "sucess";
      case SCAN_ERROR_MISSING_METHOD   : return string_missing_method;
      case SCAN_ERROR_MISSING_URI      : return string_missing_uri;
      case SCAN_ERROR_MISSING_VERSION  : return string_missing_version;
      case SCAN_ERROR_MISSING_PARAMETER: return string_missing_parameter;
      case SCAN_ERROR_MISSING_BODY     : return string_missing_body;
   }
   return "invalid error number";
}

int scanner(m4u_interfaces::Request* request,const char * cursor, const char * const limit,int *var_method,int *var_uri,int *var_version,Position *uri,Position *body )
{
   m4u_interfaces::Request::Parameters* parameter;
   std::string key,value;
   const char * marker;
   const char * ctxmarker;
   char buffer[256];
   Position variable;
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
		"GET"{ *var_method=1;request->set_type(m4u_interfaces::Request::GET);goto lbl_method;}
		"HEAD"{ *var_method=2;request->set_type(m4u_interfaces::Request::HEAD);goto lbl_method;}
		"POST"{ *var_method=3;request->set_type(m4u_interfaces::Request::POST);goto lbl_method;}
		"PUT"{ *var_method=4;request->set_type(m4u_interfaces::Request::PUT);goto lbl_method;}
		"DELETE"{ *var_method=5;request->set_type(m4u_interfaces::Request::DELETE);goto lbl_method;}
		"CONNECT"{ *var_method=6;request->set_type(m4u_interfaces::Request::CONNECT);goto lbl_method;}
		"OPTIONS"{ *var_method=7;request->set_type(m4u_interfaces::Request::OPTIONS);goto lbl_method;}
		"TRACE"{ *var_method=8;request->set_type(m4u_interfaces::Request::TRACE);goto lbl_method;}
   */
lbl_method:
   if(!*var_method)
   {
      return SCAN_ERROR_MISSING_METHOD;
   }
   /*!re2c
      "" { return -1; }
      SP { goto lbl_space_uri;}
   */
lbl_space_uri:
   uri->begin=cursor;
   uri->end=0;
   /*!re2c
		""{ *var_uri=0;goto lbl_uri;}
		Request_URI{ *var_uri=1;goto lbl_uri;}
   */
lbl_uri:
   if(!*var_uri)
   {
      return SCAN_ERROR_MISSING_URI;
   }
   uri->end=cursor;
   value.assign((char*)uri->begin,uri->end-uri->begin);
   request->set_path(value);
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
      return SCAN_ERROR_MISSING_VERSION;
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
		"Accept"{ var_request_header=1;key="Accept";goto lbl_request_header;}
		"Accept-Encoding"{ var_request_header=2;key="Accept-Encoding";goto lbl_request_header;}
		"Accept-Language"{ var_request_header=3;key="Accept-Language";goto lbl_request_header;}
		"Cache-Control"{ var_request_header=4;key="Cache-Control";goto lbl_request_header;}
		"Connection"{ var_request_header=5;key="Connection";goto lbl_request_header;}
		"From"{ var_request_header=6;key="From";goto lbl_request_header;}
		"Host"{ var_request_header=7;key="Host";goto lbl_request_header;}
		"User-Agent"{ var_request_header=8;key="User-Agent";goto lbl_request_header;}
		"Content-Type"{ var_request_header=9;key="Content-Type";goto lbl_request_header;}
   */
lbl_request_header:
   if(!var_request_header)
   {
      //return SCAN_ERROR_MISSING_PARAMETER;
      //parameter opcional, pular para body!!!
      goto lbl_expect_body;
   }
   parameter = request->add_params();
   variable.end=cursor;
   if(*cursor==':')++cursor;
#ifdef _DEBUG_ME_
   strncpy(buffer,variable.begin,variable.end-variable.begin);
   buffer[variable.end-variable.begin]='\0';
   printf("variable[%s]\n",buffer);
#endif
   variable.begin=cursor;
   variable.end=0;
   /*!re2c
      "" { printf("position[%s]\n",cursor);return -4; }
      variable_value { }
   */
   variable.end=cursor;
   if(*cursor==':')++cursor;
#ifdef _DEBUG_ME_
   strncpy(buffer,variable.begin,variable.end-variable.begin);
   buffer[variable.end-variable.begin]='\0';
   printf("value[%s]\n",buffer);
#endif
   value.assign((char*)variable.begin,variable.end-variable.begin);
   std::cout << "Param[" <<key <<"] with value["<<value<<"]"<<std::endl;
   parameter->set_key(key);
   parameter->set_value(value);
   /*!re2c
      "" { printf(".position[%s]\n",cursor);return 1; }
      LF { goto lbl_space_request_header;}
      CRLF { goto lbl_expect_body;}
   */

lbl_expect_body:
   body->begin=cursor;
   body->end=0;

   while(*cursor!='\0')++cursor;
   body->end=cursor;

   if(body->begin==cursor)
   {
      body->begin=0;
      body->end=0;
      return SCAN_ERROR_MISSING_BODY;
   }
#ifdef _DEBUG_ME_
   strncpy(buffer,body->begin,body->end-body->begin);
   buffer[body->end-body->begin]='\0';
   printf("body[%s]\n",buffer);
#endif
   value.assign((char*)body->begin,body->end-body->begin);
   request->set_body(value);
   return 1;
}
void ListRequests(const m4u_interfaces::MessageBlock& msg)
{
   for (int i = 0; i < msg.request_size(); i++) {
      const m4u_interfaces::Request& request = msg.request(i);
      cout << "###########################################" << endl;
      cout << " path: " << request.path() << endl;

      cout << " Method : ";
      switch (request.type()) {
         case m4u_interfaces::Request::GET:
            cout << "GET" << endl;
            break;
         case m4u_interfaces::Request::HEAD:
            cout << "HEAD" << endl;
            break;
         case m4u_interfaces::Request::POST:
            cout << "POST" << endl;
            break;
         case m4u_interfaces::Request::PUT:
            cout << "PUT" << endl;
            break;
         case m4u_interfaces::Request::DELETE:
            cout << "DELETE" << endl;
            break;
         case m4u_interfaces::Request::CONNECT:
            cout << "CONNECT" << endl;
            break;
         case m4u_interfaces::Request::OPTIONS:
            cout << "OPTIONS" << endl;
            break;
         case m4u_interfaces::Request::TRACE:
            cout << "TRACE" << endl;
            break;
      }

      if(request.params_size()>0)
      {
         cout << " Parameters : "<<endl;
         for (int j = 0; j < request.params_size(); j++) {
            const m4u_interfaces::Request::Parameters& parameter = request.params(j);

            cout << "  " << parameter.key() << ":" <<parameter.value() << endl;
         }
      }
      cout << " body: " << request.body() << endl;
   }
}
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

POST /path/script.cgi HTTP/1.0
From: frog@jmarshall.com
User-Agent: HTTPTool/1.0
Content-Type: application/x-www-form-urlencoded
Content-Length: 32

home=Cosby&favorite+flavor=flies
*/
int main ()
{
   Position uri;
   Position body;
   int ret,var_method,var_uri,var_version;
   char *str;
   char buffer[127];
   GOOGLE_PROTOBUF_VERIFY_VERSION;
   m4u_interfaces::MessageBlock msg;

   str=(char*)malloc(1024);
   strcpy(str,
      "GET /path/cgi.script HTTP/1.1\n"
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
   ret=scanner(msg.add_request(),str, str+strlen(str),&var_method,&var_uri,&var_version,&uri,&body);
   printf("scanner [%d:%s] [%d,%d,%d]\n",ret,StrScanErr(ret),var_method,var_uri,var_version);
   if(ret)
   {
#ifdef _DEBUG_ME_
      strncpy(buffer,uri.begin,uri.end-uri.begin);
      buffer[uri.end-uri.begin]='\0';
      printf("uri[%s]\n",buffer);
#endif
      /*{//serialize parse result

         fstream output("request.output", ios::out | ios::trunc | ios::binary);
         if (!msg.SerializeToOstream(&output))
         {
            cerr << "Failed to request." << endl;
            return -1;
         }
      }*/
      ListRequests(msg);
   }else
      printf("http method failed\n");

   free(str);
   google::protobuf::ShutdownProtobufLibrary();
   return 0;
}

